/* SPDX-License-Identifier: GPL-3.0-only */
#define UI_TEST_NO_MAIN
#include "ui_test.c"
#include <assert.h>
int main(void)
{
    ui_power_on();
    tessera_knob(0,TESS_ENG_EUCLID);
    ui.home=0; ui.page=page_titled("HARMONY");
    edit_param(0,1); edit_param(1,5); edit_param(2,20); edit_param(3,30);
    assert(harm_track[0].mode==HARM_KEY && harm_track[0].outside==10 && harm_track[0].bias==40 && harm_track[0].resolve==60);
    ui.page=page_titled("NOTE RANGE"); edit_param(0,-1000); edit_param(1,1000);
    assert(harm_track[0].low==0 && harm_track[0].high==127);
    ui.page=page_titled("OUTPUT"); edit_param(0,1); edit_param(1,4);
    assert(harm_track[0].midi && harm_track[0].channel==4);
    chain_config.count=2; chain_config.row[0].repeat=chain_config.row[1].repeat=1;
    ui.page=page_titled("SONG CHORD"); edit_param(0,1); edit_param(1,-7); edit_param(2,3); edit_param(3,4);
    assert(harm_edit_row==1 && harm_song[1].root==5 && harm_song[1].chord==CH_MAJ && harm_song[1].bars==4);
    ui.page=page_titled("SONG KEY"); edit_param(1,-3); edit_param(2,-14);
    assert(harm_song[1].key==9 && harm_song[1].scale==2);
    /* Remove the song before testing a free-running generator capture. */
    chain_defaults(&chain_config);
    for(uint32_t i=0;i<NSTEP;i++){memset(&trk[0].step[i],0,sizeof(step_t));trk[0].step[i].time=ST_REST;}
    trk[0].p[P_SLEN]=4;trk[0].p[P_SDIV]=0;song.g[G_BPM]=240;
    tessera.euclid[0].pulses=16;harm_track[0].outside=0;
    ui.page=page_titled("PATTERN");
    rec_tap(); assert((song.rec&1u) && !(song.rec_latch&1u));
    seq_start(); seq_advance(1);
    uint32_t samples=4u*seq_div_samples(0)+CTL;
    for(uint32_t i=0;i<samples;i+=CTL)seq_advance(CTL);
    assert(!(song.rec&1u) && song.playing && trk[0].step[0].n);
    seq_stop();
    step_t captured[NSTEP];memcpy(captured,trk[0].step,sizeof captured);
    undo_step(0);
    for(uint32_t i=0;i<NSTEP;i++)assert(!trk[0].step[i].n && !trk[0].step[i].hit);
    undo_step(1);assert(!memcmp(captured,trk[0].step,sizeof captured));
    rec_tap();assert(!(song.rec&1u) && str_eq(ui.msg,"USE EMPTY PATTERN"));
    trk[0].p[P_SCALE]=16;scale_share(&trk[0]);assert(!harm_track[0].mode);
    /* The REC+PLAY chord also stays one-pass for generators. */
    chain_defaults(&chain_config);seq_stop();tessera_knob(0,TESS_ENG_EUCLID);
    pattern_switch(&trk[0],1);ui.page=page_titled("PATTERN");rec_play();
    assert((song.rec&1u) && !(song.rec_latch&1u));rec_set(&trk[0],0);transport_req=0;
    /* Upstream's explicit keyboard recording remains continuous in STEP. */
    tessera_knob(0,-128);rec_tap();assert((song.rec&1u)&&(song.rec_latch&1u));
    tessera_knob(0,TESS_ENG_TURING);assert(!(song.rec&1u)&&!(song.rec_latch&1u));
    /* Selecting an empty queued bank arms that destination, not the used bank
     * still playing until the boundary. This is upstream's empty-pattern shortcut. */
    trk[0].step[0]=(step_t){{64},1,ST_NOTE,0,100};trk[0].p[P_SLEN]=4;
    trk[0].p[P_SCALE]=0;seq_start();seq_advance(1);
    assert(!pattern_request(&trk[0],2));rec_toggle(0);
    assert((song.rec_wait&1u)&&feature_run.bank[0]==2u);
    uint32_t pass=32u*seq_div_samples(2)+8u*seq_div_samples(0);
    for(uint32_t i=0;i<pass;i+=CTL)seq_advance(CTL);
    assert(trk[0].pattern==2u&&!(song.rec&1u));seq_stop();
    assert(pattern_at(0,1)->step[0].note[0]==64u);
    puts("features UI: five pages, harmony/range/MIDI/SONG controls, one-pass REC, undo/redo and overwrite protection passed");
    return 0;
}
