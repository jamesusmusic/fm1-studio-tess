/* Studio Tess 7 integration and validation modifications — By: Inputsignwave.
 * Upstream authorship and licence notices below remain applicable. */
/* SPDX-License-Identifier: GPL-3.0-only */
#define main hostsim_main
#include "hostsim.c"
#undef main
#include <assert.h>

static void reset(void)
{
    usb.config = 0;
    memset(&song, 0, sizeof song);
    memset(&chain, 0, sizeof chain);
    memset(trk, 0, sizeof trk);
    memset(&feature_run, 0, sizeof feature_run);
    memset(live_held, 0, sizeof live_held);
    mo_w = mo_r = 0;
    eng_state_reset();
    host_tracks_init();
    recording_reset();
    tessera_reset();
    chain_defaults(&chain_config);
    song.g[G_BPM] = 240;
    for (uint32_t tr = 0; tr < NTRK; tr++) {
        trk[tr].p[P_SLEN] = 4;
        trk[tr].p[P_SDIV] = 0;
        host_preset(&trk[tr], ENGI_CZ, 0);
    }
    song.sel = 0;
}
static void advance(uint32_t n)
{
    while (n) { uint32_t k = n > CTL ? CTL : n; seq_advance(k); n -= k; }
}
static uint32_t captured(uint32_t tr)
{
    uint32_t n = 0;
    for (uint32_t i = 0; i < RECORD_MAX; i++)
        n += recording[i].vel && (recording[i].owner & 31u) == recording_owner(&trk[tr]);
    return n;
}
static void harmony(void)
{
    reset();
    harm_track[0].mode = HARM_KEY;
    for (uint32_t scale = 0; scale < SCALE_LEGACY; scale++)
        for (uint32_t root = 0; root < 12; root++) {
            trk[0].p[P_SCALE] = (int16_t)scale; trk[0].p[P_ROOT] = (int16_t)root;
            for (uint32_t n = 0; n < 100; n++) {
                int32_t note = features_pitch(&trk[0], n % 128u);
                assert(note >= 36 && note <= 84);
                assert((SCALE_MASK[scale] >> ((note + 12 - root) % 12)) & 1u);
            }
        }
    trk[0].p[P_ROOT] = 9; trk[0].p[P_SCALE] = 2;
    harm_track[0].outside = 100;
    for (uint32_t n = 0; n < 1000; n++) {
        int32_t note = features_pitch(&trk[0], 60);
        assert(!((SCALE_MASK[2] >> ((note + 3) % 12)) & 1u));
    }
    harm_track[0].resolve = 100;
    for (uint32_t n = 0; n < 100; n++) {
        (void)features_pitch(&trk[0], 60); assert(!feature_run.outside[0]);
        (void)features_pitch(&trk[0], 60); assert(feature_run.outside[0]);
    }
    harm_track[0].outside = harm_track[0].resolve = 0;
    harm_track[0].bias = 100;
    trk[0].p[P_CHRD] = CH_MIN;
    for (uint32_t n = 0; n < 1000; n++) {
        uint32_t pc = (uint32_t)features_pitch(&trk[0], 72) % 12;
        assert(pc == 9 || pc == 0 || pc == 4);
    }
    harm_track[0].mode = HARM_SONG;
    harm_song[0] = (harm_row_t){2, CH_MIN, 2, 2, 1};
    for (uint32_t n = 0; n < 1000; n++) {
        uint32_t pc = (uint32_t)features_pitch(&trk[0], 50) % 12;
        assert(pc == 2 || pc == 5 || pc == 9);
    }
    harm_track[0].low = harm_track[0].high = 60;
    harm_track[0].outside = 100;
    assert(features_pitch(&trk[0], 127) == 60); /* empty category falls back within RANGE */
    harm_track[0].mode = HARM_OFF;
    uint32_t rng_before = rng_state;
    assert(features_pitch(&trk[0], 17) == 17 && rng_state == rng_before);
    puts("features: all 16 12-tone scales/12 roots, inside/outside extremes, resolution, chord bias, SONG key override, range fallback and OFF RNG identity passed");
}
static void midi(void)
{
    reset(); usb.config = 1;
    harm_track[0].midi = 1; harm_track[0].channel = 4;
    features_midi_on(&trk[0], 60, 87);
    assert(mo_w == 1 && midi_out_q[0] == (0x09u | 0x94u << 8 | 60u << 16 | 87u << 24));
    features_midi_on(&trk[0], 60, 100);
    assert(mo_w == 3 && ((midi_out_q[1] >> 8) & 255u) == 0x84);
    mo_r = mo_w;
    while (mo_w - mo_r < MQ) midi_out_event(0);
    features_midi_off(&trk[0], 60);
    assert(feature_run.pending && (feature_run.held[0][1] & (1u << 28)));
    mo_r = mo_w;
    features_midi_poll();
    assert(!feature_run.pending && !feature_run.held[0][1]);
    assert(((midi_out_q[(mo_w - 1) % MQ] >> 8) & 255u) == 0x84);
    features_midi_on(&trk[0], 62, 99);
    harm_track[0].channel = 12;
    uint32_t at = mo_w;
    features_midi_on(&trk[0], 64, 88);
    assert(mo_w == at + 2 && ((midi_out_q[at % MQ] >> 8) & 255u) == 0x84);
    assert(((midi_out_q[(at + 1) % MQ] >> 8) & 255u) == 0x9C);
    features_midi_stop(0);
    assert(!feature_run.pending);
    for (uint32_t k = 0; k < 4; k++) assert(!feature_run.held[0][k]);
    mo_r = mo_w;
    trk[0].step[0] = (step_t){.time=ST_NOTE,.n=2,.note={0,67},.vel=91};
    trk[0].seq_idx = 0;
    at = mo_w;
    seq_step(&trk[0], &trk[0].step[0], 100, 0);
    assert(mo_w == at + 2);
    seq_release(&trk[0]);
    assert(mo_w == at + 4 && !feature_run.held[0][0]);
    harm_track[1].midi=1;harm_track[1].channel=harm_track[0].channel;
    mo_r=mo_w;at=mo_w;
    features_midi_on(&trk[0],72,80);features_midi_on(&trk[1],72,90);
    assert(mo_w==at+1);
    features_midi_off(&trk[0],72);assert(mo_w==at+1);
    features_midi_off(&trk[1],72);assert(mo_w==at+2);
    puts("features: USB MIDI channel/velocity, retrigger, queue-full deferred note-offs, channel changes and STEP note 0 passed");
}
static void capture(void)
{
    for (uint32_t mode = TESS_ENG_TURING; mode < TESS_ENG_GRIDS; mode++) {
        reset();
        tessera_knob(0, (int32_t)mode);
        tessera.turing[0].reg = 0xFFFF;
        tessera.euclid[0].pulses = 16;
        tessera.morph[0].snap_a = tessera.morph[0].snap_b = 0xFFFF;
        trk[0].p[P_TRANS] = 12;
        harm_track[0].mode = HARM_KEY; harm_track[0].low = 60; harm_track[0].high = 80;
        harm_track[0].midi = 1; usb.config = 1;
        seq_start(); advance(1);
        uint32_t loop = recording_loop(&trk[0], seq_div_samples(0));
        advance(loop / 2);
        rec_set(&trk[0], 1);
        assert(song.rec_wait & 1u);
        advance(loop / 4);
        assert(!captured(0));
        advance(loop * 2);
        assert(!(song.rec & 1u) && song.playing && captured(0) == 4 && !feature_run.capture[0]);
        for (uint32_t k = 0; k < 4; k++) {
            assert(trk[0].step[k].time == ST_NOTE && trk[0].step[k].n == 1 && trk[0].step[k].flags & SF_RECORDED);
            assert(trk[0].step[k].note[0] >= 60 && trk[0].step[k].note[0] <= 80);
        }
        for (uint32_t i = 0; i < RECORD_MAX; i++) if (recording[i].vel) {
            assert(recording[i].on == 0 && recording[i].duration > 1 && recording_valid(&recording[i]));
        }
        uint32_t count = captured(0);
        advance(loop * 2);
        assert(captured(0) == count); /* no stacking after one pass */
        seq_stop();
        tessera_knob(0, -(int32_t)mode);
        mo_w = mo_r = 0;
        seq_start(); advance(loop + 1);
        assert(mo_w > 0); /* performance notes replay through STEP's timed event path */
        seq_stop();
        assert(!feature_run.pending);
        for (uint32_t k = 0; k < 4; k++) assert(!feature_run.held[0][k]);
        step_t before[NSTEP]; memcpy(before, trk[0].step, sizeof before);
        tessera_knob(0, TESS_ENG_EUCLID);
        rec_set(&trk[0], 1);
        assert(!(song.rec & 1u) && features_notice == 1 && !memcmp(before, trk[0].step, sizeof before));
    }
    puts("features: REC waits for loop zero, records one pass for all four generators, preserves final pitches/velocities/gates, prevents stacking, replays through STEP/MIDI and protects occupied patterns");
}
static void song_time(void)
{
    reset();
    chain_config.count = 2;
    chain_config.row[0].repeat = chain_config.row[1].repeat = 1;
    harm_song[0].bars = harm_song[1].bars = 1;
    chain.config = chain_config;
    chain.armed = 1;
    seq_start(); advance(1);
    uint32_t bar = 4u * seq_div_samples(0);
    advance(bar - CTL);
    assert(chain.running && chain.row == 0);
    advance(2u * CTL);
    assert(chain.running && chain.row == 1);
    advance(bar + CTL);
    assert(!chain.running && !song.playing);
    puts("features: explicit SONG bar durations advance harmony rows and stop at song end");
}
static void capture_edges(void)
{
    reset(); tessera_knob(0,TESS_ENG_EUCLID); tessera.euclid[0].pulses=16;
    harm_track[0]=(harm_track_t){HARM_KEY,0,0,0,0,0,1,0};
    trk[0].p[P_SCALE]=0;trk[0].p[P_ROOT]=0;usb.config=1;
    rec_set(&trk[0],1);seq_start();advance(1);
    uint32_t period=seq_div_samples(0);advance(period*4u+CTL);
    assert(!(song.rec&1u) && captured(0)==4);
    seq_stop();tessera_knob(0,-TESS_ENG_EUCLID);
    seq_start();advance(1);advance(period*3u/4u);
    for(uint32_t v=0;v<NVOICE;v++)assert(!trk[0].v[v].gate);
    assert(!feature_run.held[0][0]);seq_stop();
    reset();tessera_knob(0,TESS_ENG_EUCLID);seq_start();advance(1);rec_set(&trk[0],1);
    assert(song.rec_wait&1u);seq_stop();assert(!(song.rec&1u) && !captured(0));
    reset();tessera_knob(0,TESS_ENG_EUCLID);seq_start();advance(1);rec_set(&trk[0],1);
    trk[0].pattern=1;features_capture_guard(&trk[0]);assert(!(song.rec&1u) && features_notice==5 && !captured(0));seq_stop();
    reset();tessera_knob(0,TESS_ENG_EUCLID);
    trk[1].step[0]=(step_t){.time=ST_NOTE,.flags=SF_RECORDED,.n=1,.note={60},.vel=90};
    for(uint32_t i=0;i<RECORD_MAX;i++) recording[i]=(recorded_note_t){.duration=1,.note=60,.vel=90,.owner=NPAT,.step=0};
    recording_reindex();rec_set(&trk[0],1);
    assert(!(song.rec&1u) && features_notice==2);
    for(uint32_t i=0;i<RECORD_MAX;i++)assert(recording[i].vel==90);
    puts("features: captured MIDI note 0 releases during STEP replay; Stop, bank changes and exhausted recording capacity are safe");
}
static void persistence(void)
{
    reset();
    uint8_t full[TESS_FULL_BYTES], copy[TESS_FULL_BYTES], old[TESSERA_PK_BYTES];
    harm_track[0] = (harm_track_t){2,24,96,13,70,85,1,8};
    harm_song[3] = (harm_row_t){5,CH_MAJ,9,2,4};
    tess_full_pack(full); tessera_pack(old);
    tessera_reset(); assert(tess_full_unpack(full, sizeof full));
    tess_full_pack(copy); assert(!memcmp(full, copy, sizeof full));
    full[TESSERA_PK_BYTES + 4 + 1] = 127; /* LOW > HIGH */
    assert(!tess_full_unpack(full, sizeof full));
    tess_full_pack(full); assert(!memcmp(full, copy, sizeof full));
    assert(!tess_full_valid(full, 1) && !tess_full_valid(full, TESS_FULL_BYTES-1));
    assert(tess_full_unpack(old, sizeof old) && harm_track[0].mode == HARM_OFF && !harm_track[0].midi);
    puts("features: full settings/SONG round trip, malformed load atomicity and legacy-180-byte import passed");
}

#ifndef TESS_FEATURE_TEST_NO_MAIN
int main(void)
{
    harmony(); midi(); capture(); capture_edges(); song_time(); persistence();
    return 0;
}

#endif
