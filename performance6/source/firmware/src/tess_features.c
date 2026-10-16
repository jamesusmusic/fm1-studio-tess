/* SPDX-License-Identifier: GPL-3.0-only */
/* Studio generative harmony, USB note output and one-pass performance capture.
 * Bounded control-rate work only. No allocation, UI, flash or storage in ticks. */
static struct {
    uint32_t held[NTRK][4], off[NTRK][4];
    uint8_t channel[NTRK], pending;
    uint8_t previous[NTRK], have[NTRK], outside[NTRK];
    uint32_t context[NTRK];
    uint8_t capture[NTRK], bank[NTRK];
} feature_run;

static void features_song_clear(void)
{
    for (uint32_t r = 0; r < CHAIN_ROWS; r++)
        harm_song[r] = (harm_row_t){12, 0, 12, 16, 0};
    harm_edit_row = 0;
}
static void features_init(void)
{
    /* Pending output note-offs survive project loads until USB can accept them. */
    for (uint32_t tr = 0; tr < NTRK; tr++) {
        features_duration_stop(&trk[tr]);
        features_midi_stop(tr);
        performance[tr]=performance_default();
        harm_track[tr] = (harm_track_t){HARM_OFF, 36, 84, 0, 0, 0, 0, (uint8_t)tr};
        feature_run.have[tr] = feature_run.capture[tr] = 0;
    }
    features_song_clear();
    features_chain_time = 0;
    features_notice = 0;
}

static void features_midi_poll(void)
{
    if (!feature_run.pending || !usb.config) return;
    uint32_t budget = 8;
    for (uint32_t tr = 0; tr < NTRK; tr++) {
        for (uint32_t note = 0; note < 128; note++) {
            uint32_t mask = 1u << (note & 31u), word = note >> 5;
            if (!(feature_run.off[tr][word] & mask)) continue;
            uint32_t shared = 0;
            for (uint32_t other = 0; other < NTRK; other++)
                if (other != tr && feature_run.channel[other] == feature_run.channel[tr] &&
                    (feature_run.held[other][word] & ~feature_run.off[other][word] & mask)) shared = 1;
            if (shared) { /* another sequencer track still owns this channel/pitch */
                feature_run.off[tr][word] &= ~mask;
                feature_run.held[tr][word] &= ~mask;
                continue;
            }
            if (!budget || !midi_try_out(0x08u | (0x80u | feature_run.channel[tr]) << 8 | note << 16)) return;
            feature_run.off[tr][word] &= ~mask;
            feature_run.held[tr][word] &= ~mask;
            budget--;
        }
    }
    feature_run.pending = 0;
}
static void features_midi_off(track_t *t, uint32_t note)
{
    uint32_t tr = trk_index(t), word = (note & 127u) >> 5, bit = 1u << (note & 31u);
    if (tr >= NTRK || !(feature_run.held[tr][word] & bit)) return;
    feature_run.off[tr][word] |= bit;
    feature_run.pending = 1;
    features_midi_poll();
}
static void features_midi_stop(uint32_t tr)
{
    if (tr >= NTRK) return;
    for (uint32_t k = 0; k < 4; k++) {
        feature_run.off[tr][k] |= feature_run.held[tr][k];
        if (feature_run.off[tr][k]) feature_run.pending = 1;
    }
    features_midi_poll();
}
static void features_midi_on(track_t *t, uint32_t note, uint32_t velocity)
{
    uint32_t tr = trk_index(t);
    if (tr >= NTRK || !harm_track[tr].midi || !usb.config || note > 127u) return;
    features_midi_poll();
    if (feature_run.pending) return; /* release old notes before accepting more */
    if (feature_run.channel[tr] != harm_track[tr].channel) {
        features_midi_stop(tr);
        if (feature_run.pending) return;
        feature_run.channel[tr] = harm_track[tr].channel;
    }
    uint32_t word = note >> 5, bit = 1u << (note & 31u);
    if (feature_run.held[tr][word] & bit) {
        features_midi_off(t, note);
        if (feature_run.pending) return;
    }
    for (uint32_t other = 0; other < NTRK; other++)
        if (other != tr && feature_run.channel[other] == feature_run.channel[tr] && (feature_run.held[other][word] & bit)) {
            feature_run.held[tr][word] |= bit;
            return;
        }
    if (midi_try_out(0x09u | (0x90u | feature_run.channel[tr]) << 8 | note << 16 | clamp(velocity, 1, 127) << 24))
        feature_run.held[tr][word] |= bit;
}

static uint32_t harmony_chord(uint32_t scale, uint32_t key, uint32_t root, uint32_t chord)
{
    uint32_t mask = 0;
    if (chord >= CH_MAJ && chord <= CH_POW) {
        for (uint32_t k = 0; k < CHORD_MAX && CHORD_SHAPE[chord - CH_MAJ][k] >= 0; k++)
            mask |= 1u << ((root + (uint32_t)CHORD_SHAPE[chord - CH_MAJ][k]) % 12u);
    } else {
        uint32_t degrees[12], count = 0, at = 0, sm = SCALE_MASK[scale];
        if (sm == 0xFFFu) sm = SCALE_MASK[1];
        for (uint32_t k = 0; k < 12; k++) if ((sm >> k) & 1u) degrees[count++] = k;
        uint32_t degree = (root + 12u - key) % 12u;
        for (uint32_t k = 0; k < count; k++) if (degrees[k] <= degree) at = k;
        for (uint32_t k = 0; k < (chord == CH_DIA7 ? 4u : 3u); k++)
            mask |= 1u << ((key + degrees[(at + 2u * k) % count]) % 12u);
    }
    return mask;
}

/* 12-tone scales have chromatic alternatives on the MIDI grid. Extended
 * microtonal layouts use degree indices, so this first harmony build refuses
 * that combination rather than pretending those indices are semitones. */
static int32_t features_pitch(track_t *t, int32_t proposed)
{
    uint32_t tr = trk_index(t);
    const harm_track_t *h = &harm_track[tr];
    if (!h->mode) return proposed;
    uint32_t key = (uint32_t)clamp(t->p[P_ROOT], 0, 11), scale = (uint32_t)t->p[P_SCALE];
    uint32_t chord = (uint32_t)clamp(t->p[P_CHRD], 0, CH_POW), root = key;
    if (h->mode == HARM_SONG) {
        const harm_row_t *r = &harm_song[chain.running ? chain.row % CHAIN_ROWS : harm_edit_row % CHAIN_ROWS];
        if (r->key < 12) key = r->key;
        if (r->scale < 16) scale = r->scale;
        root = r->root < 12 ? r->root : key;
        if (r->chord) chord = r->chord;
    }
    if (scale >= SCALE_LEGACY) return proposed;
    uint32_t sm = SCALE_MASK[scale], cm = harmony_chord(scale, key, root, chord);
    uint32_t signature = key | scale << 4 | root << 9 | chord << 13;
    int32_t target = proposed;
    uint32_t want_out = h->outside && tess_rnd(100) < h->outside;
    if (feature_run.have[tr] && feature_run.outside[tr] && h->resolve && tess_rnd(100) < h->resolve) {
        want_out = 0;
        target = feature_run.previous[tr];
    } else if (feature_run.have[tr] && feature_run.context[tr] != signature)
        target = feature_run.previous[tr]; /* nearest movement on a harmonic change */
    uint32_t want_chord = h->bias && tess_rnd(100) < h->bias;
    uint16_t weights[128];
    uint32_t total = 0;
    for (uint32_t pass = 0; pass < 3; pass++) {
        total = 0;
        for (uint32_t note = h->low; note <= h->high; note++) {
            uint32_t inside = (sm >> ((note + 12u - key) % 12u)) & 1u;
            uint32_t eligible = (inside != want_out) && (!want_chord || ((cm >> (note % 12u)) & 1u));
            uint32_t distance = note > (uint32_t)target ? note - (uint32_t)target : (uint32_t)target - note;
            weights[note] = eligible ? 1u + 48u / (1u + distance) : 0u;
            total += weights[note];
        }
        if (total) break;
        if (want_chord) want_chord = 0;
        else want_out ^= 1u; /* requested category may have no notes in RANGE */
    }
    uint32_t chosen = (uint32_t)clamp(proposed, h->low, h->high);
    if (total) {
        uint32_t draw = tess_rnd(total);
        for (uint32_t note = h->low; note <= h->high; note++) {
            if (draw < weights[note]) { chosen = note; break; }
            draw -= weights[note];
        }
    }
    feature_run.previous[tr] = (uint8_t)chosen;
    feature_run.have[tr] = 1;
    feature_run.context[tr] = signature;
    feature_run.outside[tr] = (uint8_t)!((sm >> ((chosen + 12u - key) % 12u)) & 1u);
    return (int32_t)chosen;
}

/* Capture is deliberately non-destructive: an empty destination and enough
 * free performance entries for all 64 possible steps are required up front. */
static int features_capture_prepare(track_t *t)
{
    uint32_t tr = trk_index(t), free = 0;
    if (chain.armed || chain.running) { features_notice = 3; return 1; }
    uint32_t bank=t->pattern_next<NPAT?t->pattern_next:t->pattern;
    const step_t *steps=bank==t->pattern?t->step:pattern_at(tr,bank)->step;
    for (uint32_t k = 0; k < NSTEP; k++)
        if (steps[k].n || steps[k].hit || steps[k].time == ST_TIE) { features_notice = 1; return 1; }
    if (bank==t->pattern) recording_prune(t);
    for (uint32_t i = 0; i < RECORD_MAX; i++) if (!recording[i].vel) free++;
    if (free < NSTEP) { features_notice = 2; return 1; }
    feature_run.capture[tr] = 1;
    feature_run.bank[tr] = (uint8_t)bank;
    return 0;
}
static void features_capture_end(track_t *t)
{
    uint32_t tr = trk_index(t);
    if (feature_run.capture[tr]) {
        features_capture_close(t);
        feature_run.capture[tr] = 0;
        features_notice = 4;
    }
}
static void features_capture_guard(track_t *t)
{
    uint32_t tr = trk_index(t);
    uint32_t waiting=(song.rec_wait & (1u<<tr)) && t->pattern_next==feature_run.bank[tr];
    if (feature_run.capture[tr] && ((t->pattern != feature_run.bank[tr] && !waiting) || chain.running)) {
        rec_set(t, 0); features_notice = 5;
    }
}
static void features_capture_note(track_t *t, uint32_t note, uint32_t vel, uint32_t gate)
{
    uint32_t tr = trk_index(t), bit = 1u << tr;
    if (!feature_run.capture[tr] || !(song.rec & ~song.rec_wait & bit) || !song.playing) return;
    features_capture_guard(t);
    if (!feature_run.capture[tr]) return;
    uint32_t step = t->seq_idx % NSTEP, period = seq_div_samples((uint32_t)t->p[P_SDIV]);
    if (!recording_note(t, note, vel, step)) { rec_set(t, 0); features_notice = 2; return; }
    uint32_t i = recording_head[recording_owner(t)];
    recorded_note_t *r = &recording[i];
    r->on = 0; /* the generator triggered at this grid boundary */
    uint32_t span = step_samples(t, period, step);
    if (gate > span) gate = span; /* the generator releases on its next step */
    recording_run[i].held = (uint32_t)((uint64_t)(gate ? gate : 1u) * RECORD_UNIT / period);
    recording_finish(i);
    recording_flags[i] = 4; /* already heard, not an input hold or quantized take */
    step_t *s = &t->step[step];
    memset(s, 0, sizeof *s);
    s->time = ST_NOTE; s->n = 1; s->note[0] = (uint8_t)note; s->vel = (uint8_t)vel;
    s->flags = SF_RECORDED;
    s->acc = (uint8_t)clamp((int32_t)((uint64_t)gate * 255u / span), 1, 255);
    t->seq_active = 1;
}

/* The old 180-byte generator block remains a byte-identical prefix. */
static void features_legacy_defaults(uint8_t *out)
{
    out[0] = 'H'; out[1] = 'M'; out[2] = 'F'; out[3] = 1;
    out += 4;
    for (uint32_t tr = 0; tr < NTRK; tr++) {
        harm_track_t h = {HARM_OFF, 36, 84, 0, 0, 0, 0, (uint8_t)tr};
        memcpy(out, &h, 8); out += 8;
    }
    for (uint32_t r = 0; r < CHAIN_ROWS; r++) {
        harm_row_t h = {12, 0, 12, 16, 0};
        memcpy(out, &h, 5); out += 5;
    }
}
static uint32_t features_legacy_pack(uint8_t *out)
{
    features_legacy_defaults(out);
    memcpy(out + 4, harm_track, sizeof harm_track);
    memcpy(out + 4 + sizeof harm_track, harm_song, sizeof harm_song);
    return TESS_FEATURE_BYTES;
}
static int features_legacy_valid(const uint8_t *in)
{
    if (in[0] != 'H' || in[1] != 'M' || in[2] != 'F' || in[3] != 1) return 0;
    in += 4;
    for (uint32_t tr = 0; tr < NTRK; tr++, in += 8)
        if (in[0] > HARM_SONG || in[1] > in[2] || in[2] > 127 || in[3] > 100 || in[4] > 100 || in[5] > 100 || in[6] > 1 || in[7] > 15) return 0;
    for (uint32_t r = 0; r < CHAIN_ROWS; r++, in += 5)
        if (in[0] > 12 || in[1] > CH_POW || in[2] > 12 || in[3] > 16 || in[4] > 16) return 0;
    return 1;
}
static void features_legacy_apply(const uint8_t *in)
{
    memcpy(harm_track, in + 4, sizeof harm_track);
    memcpy(harm_song, in + 4 + sizeof harm_track, sizeof harm_song);
}
/* HMF2 bit-packs the existing fields plus performance controls into the same
 * 116 bytes. HMF1 imports default to bypass; no sectors or event limits change. */
static const uint8_t PERF_BITS[]={2,3,7,1,1,7,7,7,7,7,7,7,7};
static const uint8_t HARM_BITS[]={2,7,7,7,7,7,1,4};
static const uint8_t ROW_BITS[]={4,4,4,5,5};
_Static_assert(4u+(4u*42u+16u*22u+4u*77u+7u)/8u<=TESS_FEATURE_BYTES,"HMF2 fits");
static uint32_t features_encode(uint8_t *out,const harm_track_t *ht,const harm_row_t *hr,const performance_t *pf)
{
    memset(out,0,TESS_FEATURE_BYTES);out[0]='H';out[1]='M';out[2]='F';out[3]=2;
    uint32_t pos=32;
    for(uint32_t t=0;t<NTRK;t++)for(uint32_t k=0;k<8;k++)wire_bits_put(out,&pos,((const uint8_t *)&ht[t])[k],HARM_BITS[k]);
    for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<5;k++)wire_bits_put(out,&pos,((const uint8_t *)&hr[r])[k],ROW_BITS[k]);
    for(uint32_t t=0;t<NTRK;t++)for(uint32_t k=0;k<sizeof(performance_t);k++)wire_bits_put(out,&pos,((const uint8_t *)&pf[t])[k],PERF_BITS[k]);
    return TESS_FEATURE_BYTES;
}
static int features_decode(const uint8_t *in,harm_track_t *ht,harm_row_t *hr,performance_t *pf)
{
    for(uint32_t t=0;t<NTRK;t++)pf[t]=performance_default();
    if(in[3]==1u){
        if(!features_legacy_valid(in))return 0;
        memcpy(ht,in+4,sizeof harm_track);memcpy(hr,in+4+sizeof harm_track,sizeof harm_song);return 1;
    }
    if(in[0]!='H'||in[1]!='M'||in[2]!='F'||in[3]!=2u)return 0;
    uint32_t pos=32;
    for(uint32_t t=0;t<NTRK;t++)for(uint32_t k=0;k<8;k++)((uint8_t *)&ht[t])[k]=(uint8_t)wire_bits_get(in,&pos,HARM_BITS[k]);
    for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<5;k++)((uint8_t *)&hr[r])[k]=(uint8_t)wire_bits_get(in,&pos,ROW_BITS[k]);
    for(uint32_t t=0;t<NTRK;t++)for(uint32_t k=0;k<sizeof(performance_t);k++)((uint8_t *)&pf[t])[k]=(uint8_t)wire_bits_get(in,&pos,PERF_BITS[k]);
    for(uint32_t t=0;t<NTRK;t++){
        const harm_track_t *h=&ht[t];if(h->mode>HARM_SONG||h->low>h->high||h->outside>100||h->bias>100||h->resolve>100||pf[t].mode>DUR_HIT||pf[t].length>5)return 0;
    }
    for(uint32_t r=0;r<CHAIN_ROWS;r++)if(hr[r].root>12||hr[r].chord>CH_POW||hr[r].key>12||hr[r].scale>16||hr[r].bars>16)return 0;
    while(pos<TESS_FEATURE_BYTES*8u)if(wire_bits_get(in,&pos,1))return 0;
    return 1;
}
static uint32_t features_pack(uint8_t *out)
{ return features_encode(out,harm_track,harm_song,performance); }
static void features_defaults(uint8_t *out)
{
    harm_track_t ht[NTRK];harm_row_t hr[CHAIN_ROWS];performance_t pf[NTRK];
    for(uint32_t t=0;t<NTRK;t++){ht[t]=(harm_track_t){HARM_OFF,36,84,0,0,0,0,t};pf[t]=performance_default();}
    for(uint32_t r=0;r<CHAIN_ROWS;r++)hr[r]=(harm_row_t){12,0,12,16,0};
    features_encode(out,ht,hr,pf);
}
static int features_valid(const uint8_t *in)
{ harm_track_t ht[NTRK];harm_row_t hr[CHAIN_ROWS];performance_t pf[NTRK];return features_decode(in,ht,hr,pf); }
static void features_apply(const uint8_t *in)
{ (void)features_decode(in,harm_track,harm_song,performance); }
static uint32_t tess_full_pack(uint8_t *out)
{
    tessera_pack(out);
    features_pack(out + TESSERA_PK_BYTES);
    return TESS_FULL_BYTES;
}
static int tess_full_valid(const uint8_t *in, uint32_t size)
{
    return in && (size == TESSERA_PK_BYTES || size == TESS_FULL_BYTES) && tessera_valid(in) &&
        (size == TESSERA_PK_BYTES || features_valid(in + TESSERA_PK_BYTES));
}
static int tess_full_unpack(const uint8_t *in, uint32_t size)
{
    if (!tess_full_valid(in, size)) return 0;
    tessera_unpack(in);
    if (size == TESS_FULL_BYTES) features_apply(in + TESSERA_PK_BYTES);
    return 1;
}

/* Default one packed generator without touching another track's playhead. */
static void features_default_generator(uint8_t *p,uint32_t tr)
{
    memset(p,0,TESSERA_PK_BYTES/NTRK);
    p[1]=0xE1;p[2]=0xAC;p[3]=16;p[5]=2;
    p[6]=16;p[7]=tr==1u?5u:4u;p[10]=25;p[11]=100;p[12]=2;
    p[14]=p[15]=0x88;p[16]=p[17]=0xA5;p[18]=2;p[21]=30;p[24]=60;
}
static void features_packed_remove(uint8_t *state,uint32_t track)
{
    harm_track_t ht[NTRK];harm_row_t hr[CHAIN_ROWS];performance_t pf[NTRK];
    if(!features_decode(state+TESSERA_PK_BYTES,ht,hr,pf))return;
    const uint32_t stride=TESSERA_PK_BYTES/NTRK;
    for(uint32_t j=track;j+1u<NTRK;j++){
        memcpy(state+j*stride,state+(j+1u)*stride,stride);ht[j]=ht[j+1u];pf[j]=pf[j+1u];
    }
    features_default_generator(state+(NTRK-1u)*stride,NTRK-1u);
    ht[NTRK-1u]=(harm_track_t){HARM_OFF,36,84,0,0,0,0,NTRK-1u};pf[NTRK-1u]=performance_default();
    features_encode(state+TESSERA_PK_BYTES,ht,hr,pf);
    for(uint32_t j=0;j<NTRK;j++){
        uint8_t *follow=state+j*stride+28u;
        if(*follow==track+1u)*follow=0;else if(*follow>track+1u)(*follow)--;
    }
}
static void features_reset_track(uint32_t k)
{
    tessera_kill(&trk[k]);features_midi_stop(k);
    performance[k]=performance_default();
    memset(&tessera.turing[k],0,sizeof tessera.turing[k]);memset(&tessera.euclid[k],0,sizeof tessera.euclid[k]);
    memset(&tessera.markov[k],0,sizeof tessera.markov[k]);memset(&tessera.morph[k],0,sizeof tessera.morph[k]);
    memset(&tessera.evolve[k],0,sizeof tessera.evolve[k]);memset(tessera.sec[k],0,sizeof tessera.sec[k]);
    uint8_t p[TESSERA_PK_BYTES/NTRK];features_default_generator(p,k);(void)tess_pk_track(p,k,1);
    harm_track[k]=(harm_track_t){HARM_OFF,36,84,0,0,0,0,(uint8_t)k};
    feature_run.have[k]=feature_run.capture[k]=0;tessera.last_note[k]=0;
}

/* Four generator key owners per track, independently timed. Released audio
 * tails belong to the synth's voice allocator, not this held-key table. */
static struct {uint32_t left,held,fraction;uint16_t rec;uint8_t note,on;} duration_run[NTRK][4] __attribute__((section(".pool")));
static uint8_t duration_active;
static void features_duration_release(track_t *t,uint32_t slot)
{
    uint32_t tr=trk_index(t);
    if(!duration_run[tr][slot].on)return;
    uint32_t i=duration_run[tr][slot].rec;
    if(i<RECORD_MAX && (recording_flags[i]&1u)){
        recording_run[i].held=duration_run[tr][slot].held?duration_run[tr][slot].held:1;
        recording_finish(i);recording_flags[i]=4;
    }
    uint32_t note=duration_run[tr][slot].note;
    duration_run[tr][slot].on=0;
    uint32_t any=0;for(uint32_t k=0;k<4;k++)any|=duration_run[tr][k].on;
    if(!any)duration_active&=(uint8_t)~(1u<<tr);
    features_midi_off(t,note);
    if(!(live_held[tr][note>>5]&(1u<<(note&31))) && !recording_owns(t,note))trk_note_off(t,note);
}
static void features_duration_stop(track_t *t)
{
    uint32_t tr=trk_index(t);if(tr>=NTRK)return;
    for(uint32_t k=0;k<4;k++)features_duration_release(t,k);
}
static void features_capture_close(track_t *t)
{
    uint32_t tr=trk_index(t);
    for(uint32_t k=0;k<4;k++){
        uint32_t i=duration_run[tr][k].rec;
        if(duration_run[tr][k].on && i<RECORD_MAX && (recording_flags[i]&1u)){
            recording_run[i].held=duration_run[tr][k].held?duration_run[tr][k].held:1;
            recording_finish(i);recording_flags[i]=4;
        }
        duration_run[tr][k].rec=RECORD_MAX;
    }
}
static void features_duration_tick(track_t *t,uint32_t n)
{
    uint32_t tr=trk_index(t);
    if(!(duration_active & (1u<<tr)))return;
    uint32_t period=seq_div_samples((uint32_t)t->p[P_SDIV]);
    if(!tessera.mode[tr] || t->p[P_MUTE] || !song.playing){features_duration_stop(t);return;}
    for(uint32_t k=0;k<4;k++)if(duration_run[tr][k].on){
        uint32_t held=n, left=duration_run[tr][k].left;
        if(left!=0xFFFFFFFFu && held>left)held=left;
        uint64_t u=(uint64_t)held*RECORD_UNIT+duration_run[tr][k].fraction;
        duration_run[tr][k].fraction=(uint32_t)(u%period);
        duration_run[tr][k].held+=(uint32_t)(u/period);
        if(duration_run[tr][k].held>8388607u)duration_run[tr][k].held=8388607u;
        if(left!=0xFFFFFFFFu){
            if(left<=n)features_duration_release(t,k);else duration_run[tr][k].left-=n;
        }
    }
}
static void features_duration_on(track_t *t,uint32_t note,uint32_t velocity)
{
    uint32_t tr=trk_index(t),slot=4;
    const performance_t *p=&performance[tr];
    /* HIT and REPLACE are monophonic key ownership, with natural audio tails.
     * OVERLAP permits four held pitches; same-pitch retrigger ends its old key. */
    for(uint32_t k=0;k<4;k++)if(duration_run[tr][k].on && (p->mode==DUR_HIT || !p->overlap || duration_run[tr][k].note==note))features_duration_release(t,k);
    for(uint32_t k=0;k<4;k++)if(!duration_run[tr][k].on){slot=k;break;}
    if(slot==4){slot=0;for(uint32_t k=1;k<4;k++)if(duration_run[tr][k].held>duration_run[tr][slot].held)slot=k;features_duration_release(t,slot);}
    static const uint8_t divs[]={3,2,1,0,6,7};
    duration_run[tr][slot].left=p->mode==DUR_HIT?0xFFFFFFFFu:seq_div_samples(divs[p->length%6]);
    duration_run[tr][slot].held=duration_run[tr][slot].fraction=0;
    duration_run[tr][slot].rec=RECORD_MAX;duration_run[tr][slot].note=(uint8_t)note;duration_run[tr][slot].on=1;duration_active|=(uint8_t)(1u<<tr);
    trk_note_on(t,note,velocity);features_midi_on(t,note,velocity);
    if(feature_run.capture[tr] && (song.rec & ~song.rec_wait & (1u<<tr))){
        uint32_t step=t->seq_idx%NSTEP;
        if(!recording_note(t,note,velocity,step)){rec_set(t,0);features_notice=2;return;}
        uint32_t i=recording_head[recording_owner(t)];duration_run[tr][slot].rec=(uint16_t)i;
        recording[i].on=0;recording_flags[i]=5; /* held and heard, no quantization */
        step_t *s=&t->step[step];memset(s,0,sizeof *s);
        s->time=ST_NOTE;s->n=1;s->note[0]=(uint8_t)note;s->vel=(uint8_t)velocity;s->flags=SF_RECORDED;s->acc=255;t->seq_active=1;
    }
}
