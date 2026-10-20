/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess integration verification — By: Inputsignwave. */
#define TESSERA_NOTE_HOOK s7_heard
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
static uint32_t heard,heard_hash,heard_tr[4],midi_heard;
static void s7_heard(track_t *t,uint32_t note,uint32_t vel)
{assert(note<128&&vel&&vel<128);heard++;heard_tr[trk_index(t)]++;heard_hash=(heard_hash^note^vel*131u)*16777619u;}
static void s7_advance(uint32_t n)
{while(n){uint32_t part=n>CTL?CTL:n;seq_advance(part);n-=part;while(mo_r!=mo_w){uint32_t packet=midi_out_q[mo_r++%MQ];if(((packet>>8)&0xF0u)==0x90u&&((packet>>24)&127u))midi_heard++;}}}
static void s7_reset(uint32_t mode)
{
    ui_power_on();tessera_reset();recording_reset();usb.config=0;
    song.ntrk=4;song.g[G_BPM]=240;song.sel=0;
    tessera.mode[0]=(uint8_t)mode;studio7_track[0].mode=(uint8_t)mode;studio7_track[0].seeded=mode>=TESS_ENG_GRIDS;
    trk[0].p[P_SLEN]=16;trk[0].p[P_SDIV]=2;trk[0].p[P_RECQ]=0;
    if(mode==TESS_ENG_KIT){host_preset(&trk[0],ENGI_DRUM,0);studio7_track[0].kit.enabled=1;for(uint32_t l=0;l<8;l++)studio7_track[0].kit.density[l]=100;}
    else host_preset(&trk[0],ENGI_CZ,0);
    heard=heard_hash=0;memset(heard_tr,0,sizeof heard_tr);
}
static void new_generators(void)
{
    for(uint32_t mode=TESS_ENG_GRIDS;mode<TESS_ENG_COUNT;mode++){
        uint32_t hash=0,count=0;
        for(uint32_t pass=0;pass<2;pass++){
            s7_reset(mode);seq_start();s7_advance(seq_beat_samples()*16u);seq_stop();
            assert(heard>0);if(pass){assert(hash==heard_hash&&count==heard);}else{hash=heard_hash;count=heard;}
            assert(!trk[0].seq_n&&!tessera.active_gate[0]);
            for(uint32_t l=0;l<8;l++)assert(!studio7_run[0].drum[l].on);
        }
    }
    puts("studio7: all five new modes play repeatably through sequencer, release on STOP");
}
static void independent_clock(void)
{
    s7_reset(TESS_ENG_TURING);tessera.turing[0].reg=65535;
    s7_meter_t *m=&studio7_track[0].meter;m->enabled=1;m->numerator=5;m->denominator=8;m->group_count=2;memset(m->group,0,8);m->group[0]=3;m->group[1]=2;m->ratio_n=3;m->ratio_d=2;
    seq_start();uint32_t quarter=seq_beat_samples();s7_advance(quarter*10u);
    assert(studio7_run[0].clock.index==61);assert(studio7_run[0].bar==6);
    assert(trk[0].seq_idx==8);uint32_t before=studio7_run[0].clock.index;
    m->drift=100;m->freeze=1;s7_advance(quarter*10u);assert(studio7_run[0].clock.index-before==60);
    seq_stop();puts("studio7: 5/8 grouped 3+2, 3:2 clock preserves master grid, freeze retains ratio");
}
static void performed_capture(void)
{
    s7_reset(TESS_ENG_AWAKE);s7_meter_t *m=&studio7_track[0].meter;m->enabled=1;m->ratio_n=3;m->ratio_d=2;m->phase=341;
    assert(!features_capture_prepare(&trk[0]));rec_set(&trk[0],1);seq_start();s7_advance(seq_beat_samples()*5u);seq_stop();
    uint32_t count=0,fractional=0;for(uint32_t i=0;i<RECORD_MAX;i++)if(recording[i].vel){count++;fractional+=recording[i].on!=0;assert(recording_valid(&recording[i]));}
    assert(count>=20&&fractional>10);assert(!(song.rec&1));
    tessera.mode[0]=0;harm_track[0].midi=1;usb.config=1;heard=midi_heard=0;seq_start();s7_advance(seq_beat_samples()*4u);seq_stop();assert(midi_heard==count);
    puts("studio7: phase/3:2 performance captured with fractional onsets and replayed in STEP");
}
static void chord_and_kit_owners(void)
{
    s7_reset(TESS_ENG_BOUNCE);s7_track_t *s=&studio7_track[0];s->bounce.balls=3;s->bounce.speed=64;s->bounce.angle=0;
    performance[0].mode=DUR_HIT;performance[0].overlap=0;seq_start();
    for(uint32_t l=0;l<3;l++){s->bounce.x[l]=250;s->bounce.y[l]=(uint8_t)(40+l*60);s->bounce.off[l]=0;}s->bounce.dir=0;
    s7_advance(1);uint32_t owners=0;for(uint32_t l=0;l<4;l++)owners+=duration_run[0][l].on;
    assert(owners==3);seq_stop();for(uint32_t l=0;l<4;l++)assert(!duration_run[0][l].on);
    s7_reset(TESS_ENG_KIT);s=&studio7_track[0];s->kit.human=100;
    for(uint32_t i=0;i<16;i++){trk[0].step[i]=(step_t){.time=ST_NOTE,.hit=255,.acc=85,.vel=93};s->kit.anchors[i]=255;}
    pattern_commit(&trk[0]);uint32_t source_hash=proj_hash(pattern_at(0,0),sizeof(pattern_t));pattern_request(&trk[0],1);
    for(uint32_t l=0;l<8;l++)s->kit.density[l]=0;
    assert(!features_capture_prepare(&trk[0]));rec_set(&trk[0],1);seq_start();s7_advance(seq_beat_samples()*4u+CTL);seq_stop();
    uint32_t captured=0,notes=0;for(uint32_t i=0;i<RECORD_MAX;i++)if(recording[i].vel){captured++;for(uint32_t l=0;l<8;l++)if(recording[i].note==DRUM_LANE_NOTE[l]){notes|=1u<<l;assert(recording[i].vel==((85u>>l&1u)?127u:93u));}}
    assert(captured==128&&notes==255);assert(source_hash==proj_hash(pattern_at(0,0),sizeof(pattern_t)));
    for(uint32_t l=0;l<8;l++)assert(!studio7_run[0].drum[l].on);
    puts("studio7: BOUNCE retains three HIT keys; eight anchored lanes capture to another bank without changing source");
}
static void phase_form_and_midi(void)
{
    s7_reset(TESS_ENG_KIT);trk[0].step[0]=(step_t){.time=ST_NOTE,.hit=1,.vel=100};studio7_track[0].kit.anchors[0]=1;seq_start();s7_advance(1);
    uint32_t owners=0;for(uint32_t l=0;l<8;l++)owners+=studio7_run[0].drum[l].on;
    assert(owners);studio7_ui_knob(GR_S7_PHASE,3,1);
    uint32_t after=0;for(uint32_t l=0;l<8;l++)after+=studio7_run[0].drum[l].on;
    assert(owners==after&&studio7_run[0].clock.next>studio7_run[0].now);seq_stop();
    s7_reset(TESS_ENG_AWAKE);s7_meter_t *m=&studio7_track[0].meter;
    m->enabled=1;m->numerator=1;m->group_count=1;memset(m->group,0,8);m->group[0]=1;m->phase=1024;m->reset=2;
    tessera.evolve[0].every=1;tessera.evolve[0].form=1;tessera.evolve[0].contrast=0;
    seq_start();for(uint32_t n=0;n<seq_beat_samples()*8u;n+=CTL){uint32_t before=heard;s7_advance(CTL);assert(heard-before<=1u);}
    assert(heard>16);seq_stop();
    s7_reset(TESS_ENG_AWAKE);m=&studio7_track[0].meter;m->enabled=1;m->ratio_n=5;m->ratio_d=4;
    midi_sync_active=1;midi_beat_samples=22050;seq_start();s7_advance(MIDI_BEAT_UNITS*4u);
    uint32_t count=studio7_run[0].clock.index;midi_beat_samples=44100;s7_advance(MIDI_BEAT_UNITS*4u);
    assert(count==21&&studio7_run[0].clock.index==41);seq_stop();midi_sync_active=0;
    s7_reset(TESS_ENG_KIT);m=&studio7_track[0].meter;m->enabled=1;m->ratio_n=8;trk[0].p[P_SLEN]=64;
    assert(features_capture_prepare(&trk[0])&&features_notice==2&&!(song.rec&1));
    puts("studio7: alignment retains drum owners; phased FORM resets emit once; 5:4 follows MIDI ticks across tempo estimates; over-capacity capture refuses cleanly");
}
static void disabled_meter_and_kit_condition(void)
{
    s7_reset(TESS_ENG_KIT);s7_meter_t *m=&studio7_track[0].meter;
    m->phase=3000;m->ratio_n=8;m->drift=100;m->freeze=1;
    seq_start();s7_advance(seq_beat_samples());assert(studio7_run[0].clock.index==5);
    uint64_t next=studio7_run[0].clock.next;studio7_ui_knob(GR_S7_PHASE,0,-1);assert(studio7_run[0].clock.next==next);seq_stop();
    s7_reset(TESS_ENG_KIT);studio7_track[0].meter.condition=2;
    seq_start();s7_advance(seq_beat_samples()*2u);assert(!heard);s7_advance(seq_beat_samples()*5u);assert(heard);seq_stop();
    puts("studio7: inactive phase/ratio/drift edits stay bypassed, and KIT conditions work with LOCAL off");
}
static void project_extension(void)
{
    s7_reset(TESS_ENG_AWAKE);s7_track_t *t=&studio7_track[0];t->meter.enabled=1;t->meter.drift=-99;t->meter.phase=4095;t->kit.anchors[63]=255;t->kit.source=7;t->awake.a[7]=0x87;
    uint8_t before[S7_STATE_BYTES],after[S7_STATE_BYTES];assert(s7_state_encode(before,sizeof before,studio7_track));
    assert(!project_save(0));tessera_reset();project_load(0);assert(s7_state_encode(after,sizeof after,studio7_track));assert(!memcmp(before,after,sizeof before));assert(tessera.mode[0]==TESS_ENG_AWAKE);
    song.ntrk=4;song.sel=0;studio7_track[1]=studio7_track[0];studio7_track[1].meter.drift=53;assert(!track_remove(0));assert(studio7_track[0].meter.drift==53&&studio7_track[0].awake.a[7]==0x87);
    assert(studio7_track[3].meter.drift==0&&studio7_track[3].kit.anchors[63]==0);
    puts("studio7: full project saves and track removal retain complete new records");
}
static void ui_ranges(void)
{
    s7_reset(TESS_ENG_AWAKE);
    for(uint32_t g=GR_S7_METER;g<=GR_S7_POLY;g++)for(uint32_t k=0;k<4;k++){
        studio7_ui_knob(g,k,2147483647);studio7_ui_knob(g,k,-2147483647);
        assert(s7_track_valid(&studio7_track[0]));char value[16];studio7_ui_value(g,k,value);assert(value[0]);
    }
    puts("studio7: control extremes keep all settings valid");
}
int main(void)
{setbuf(stdout,0);new_generators();independent_clock();performed_capture();chord_and_kit_owners();phase_form_and_midi();disabled_meter_and_kit_condition();project_extension();ui_ranges();return 0;}
