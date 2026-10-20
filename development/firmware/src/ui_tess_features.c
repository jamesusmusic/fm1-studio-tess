/* SPDX-License-Identifier: GPL-3.0-only */
static void features_ui_knob(uint32_t graph, uint32_t slot, int32_t delta)
{
    if (slot >= 4u || !delta) return;
    if(graph>=GR_DURATION){
        performance_t *p=&performance[song.sel];
        int32_t s=tess_knob_delta(delta);
        fm1_irq_off();
        if(graph==GR_DURATION){
            if(slot==0){tessera_kill(TSEL);p->mode=(uint8_t)clamp((int32_t)p->mode+(s>0?1:-1),0,DUR_HIT);}
            if(slot==1)p->length=(uint8_t)clamp((int32_t)p->length+(s>0?1:-1),0,5);
            if(slot==2)p->fade=(uint8_t)clamp((int32_t)p->fade+s,0,127);
            if(slot==3){tessera_kill(TSEL);p->overlap=(uint8_t)clamp((int32_t)p->overlap+(s>0?1:-1),0,1);}
        }else if(graph==GR_FM_AMP){
            uint8_t *v=&p->attack;v[slot]=(uint8_t)clamp((int32_t)v[slot]+s,0,127);p->amp=1;
        }else if(graph==GR_FM_TONE){
            uint8_t *v=&p->brightness;v[slot]=(uint8_t)clamp((int32_t)v[slot]+s,0,127);
        }else{
            if(slot==0)p->amp=(uint8_t)clamp((int32_t)p->amp+(s>0?1:-1),0,1);
            if(slot==1)p->fade=(uint8_t)clamp((int32_t)p->fade+s,0,127);
        }
        fm1_irq_on();ui.force=1;return;
    }
    if ((graph == GR_OUTPUT && slot >= 2u) || (graph == GR_SONG_KEY && slot == 3u)) return;
    int32_t s = tess_knob_delta(delta);
    harm_track_t *h = &harm_track[song.sel];
    if (graph >= GR_SONG_CHORD) {
        if (!chain_config.count) { ui_message("ADD A SONG ROW"); return; }
        if (slot == 0) {
            harm_edit_row = (uint8_t)clamp((int32_t)harm_edit_row + s, 0, chain_config.count - 1u);
            ui.song_row = harm_edit_row;
            return;
        }
        if (chain_busy()) { ui_message("STOP TO EDIT"); return; }
        harm_row_t *r = &harm_song[harm_edit_row % CHAIN_ROWS];
        fm1_irq_off();
        if (graph == GR_SONG_CHORD) {
            if (slot == 1) r->root = (uint8_t)clamp((int32_t)r->root + s, 0, 12);
            if (slot == 2) r->chord = (uint8_t)clamp((int32_t)r->chord + s, 0, CH_POW);
            if (slot == 3) r->bars = (uint8_t)clamp((int32_t)r->bars + s, 0, 16);
        } else {
            if (slot == 1) r->key = (uint8_t)clamp((int32_t)r->key + s, 0, 12);
            if (slot == 2) r->scale = (uint8_t)clamp((int32_t)r->scale + s, 0, 16);
        }
        fm1_irq_on();
    } else {
        if (graph == GR_HARM && slot == 0 && s > 0 && TSEL->p[P_SCALE] >= (int32_t)SCALE_LEGACY) {
            ui_message("USE 12T SCALE"); return;
        }
        fm1_irq_off();
        if (graph == GR_HARM) {
            if (slot == 0) { h->mode = (uint8_t)clamp((int32_t)h->mode + (s > 0 ? 1 : -1), 0, HARM_SONG); feature_run.have[song.sel] = 0; }
            if (slot == 1) h->outside = (uint8_t)clamp((int32_t)h->outside + s * 2, 0, 100);
            if (slot == 2) h->bias = (uint8_t)clamp((int32_t)h->bias + s * 2, 0, 100);
            if (slot == 3) h->resolve = (uint8_t)clamp((int32_t)h->resolve + s * 2, 0, 100);
        } else if (graph == GR_RANGE) {
            if (slot == 0) h->low = (uint8_t)clamp((int32_t)h->low + s, 0, h->high);
            if (slot == 1) h->high = (uint8_t)clamp((int32_t)h->high + s, h->low, 127);
        } else if (graph == GR_OUTPUT) {
            features_midi_stop(song.sel);
            if (slot == 0) h->midi = (uint8_t)clamp((int32_t)h->midi + (s > 0 ? 1 : -1), 0, 1);
            if (slot == 1) h->channel = (uint8_t)clamp((int32_t)h->channel + s, 0, 15);
        }
        fm1_irq_on();
    }
    ui.force = 1;
}
static int features_record_ui(track_t *t)
{
    if (!tessera.mode[trk_index(t)]) return 1;
    fm1_irq_off();
    int bad = features_capture_prepare(t);
    fm1_irq_on();
    if (bad) {
        ui_message(features_notice == 1 ? "USE EMPTY PATTERN" : features_notice == 2 ? "RECORDING FULL" : "STOP SONG TO RECORD");
        features_notice = 0;
        return 0;
    }
    return 1; /* recording_note takes upstream's complete before/after snapshot */
}
