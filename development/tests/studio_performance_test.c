/* SPDX-License-Identifier: GPL-3.0-only */
#define TESS_FEATURE_TEST_NO_MAIN
#include "tess_features_test.c"
static void timing(void)
{
    reset();song.playing=1;tessera.mode[0]=TESS_ENG_EUCLID;
    performance[0].mode=DUR_HIT;
    features_duration_on(&trk[0],60,100);
    tessera.euclid[0].pulses=0;
    for(uint32_t k=0;k<20;k++){tessera_clock_step(&trk[0]);features_duration_tick(&trk[0],1000);}
    assert(duration_run[0][0].on && trk[0].v[0].gate);
    features_duration_on(&trk[0],62,90);
    assert(!trk[0].v[0].gate);
    for(uint32_t k=0;k<4;k++)assert(!duration_run[0][k].on || duration_run[0][k].note==62);
    performance[0].mode=DUR_FIXED;performance[0].overlap=1;performance[0].length=3;
    features_duration_stop(&trk[0]);
    features_duration_on(&trk[0],60,100);
    features_duration_on(&trk[0],64,100);
    uint32_t quarter=seq_div_samples(0);
    assert(duration_run[0][0].left==quarter && duration_run[0][1].left==quarter);
    features_duration_tick(&trk[0],quarter-1);assert(duration_active&1);
    features_duration_tick(&trk[0],1);assert(!(duration_active&1));
    for(uint32_t k=0;k<8;k++)features_duration_on(&trk[0],60+k,100);
    uint32_t held=0;for(uint32_t k=0;k<4;k++)held+=duration_run[0][k].on;assert(held==4);
    trk[0].p[P_MUTE]=1;features_duration_tick(&trk[0],1);assert(!(duration_active&1));
    puts("performance: hold across Euclid rests, next-hit release, independent fixed time, four-key overlap limit and mute cleanup passed");
}
static void capture_lengths(void)
{
    for(uint32_t mode=DUR_FIXED;mode<=DUR_HIT;mode++){
        reset();track_t *t=&trk[0];
        t->p[P_SLEN]=16;t->p[P_SDIV]=2;
        tessera.mode[0]=TESS_ENG_EUCLID;tessera.euclid[0].pulses=4;
        performance[0].mode=mode;performance[0].length=3;
        assert(!features_capture_prepare(t));rec_set(t,1);
        seq_start();advance(1);
        uint32_t period=seq_div_samples(2);advance(period*16+CTL);
        assert(!(song.rec&1) && song.playing && captured(0)==4);
        for(uint32_t i=0;i<RECORD_MAX;i++)if(recording[i].vel){
            uint32_t d=recording[i].duration << (recording[i].owner>>5);
            /* Last note is sealed at capture end; earlier notes span four
             * steps instead of being shortened at every rest. */
            if((recording[i].step&63)<12)assert(d>RECORD_UNIT*3 && d<=RECORD_UNIT*4+RECORD_UNIT*CTL/period+2);
            assert(!(recording_flags[i]&1));
        }
        seq_stop();assert(!duration_active);
        tessera.mode[0]=0;seq_start();advance(period*16);seq_stop();
        assert(!feature_run.pending);
    }
    puts("performance: fixed and next-hit captures retain multi-step durations; one pass seals holds, playback/STOP leave no pending MIDI keys");
}
static void fm_audio(void)
{
    reset();track_t *t=&trk[0];host_preset(t,ENGI_FM6,11);
    /* A deliberately slow native release must survive generic P_REL=0. */
    for(uint32_t k=0;k<6;k++)fm6_patch[0][k*FP_OP+FP_R1+3]=20;
    fm6_pgen[0]++;t->p[P_REL]=0;
    trk_note_on(t,60,100);int32_t pcm[CTL];
    for(uint32_t b=0;b<400;b++)track_render(t,pcm,CTL);
    trk_note_off(t,60);
    for(uint32_t b=0;b<20;b++)track_render(t,pcm,CTL);
    uint32_t active=0;for(uint32_t k=0;k<NVOICE;k++)active+=t->v[k].active;assert(active);
    performance[0].fade=5;
    for(uint32_t b=0;b<500;b++)track_render(t,pcm,CTL);
    /* A newly keyed note snapshots its own release edge. */
    trk_note_on(t,64,100);for(uint32_t b=0;b<100;b++)track_render(t,pcm,CTL);
    trk_note_off(t,64);for(uint32_t b=0;b<100;b++)track_render(t,pcm,CTL);
    for(uint32_t k=0;k<NVOICE;k++)if(t->v[k].note==64)assert(!t->v[k].active);
    uint8_t patch[FP_SIZE+1];memcpy(patch,fm6_patch[0],sizeof patch);
    performance[0].contour=100;performance[0].brightness=32;performance[0].amount=90;
    fm6_eff_build(t);assert(!memcmp(patch,fm6_patch[0],sizeof patch));
    uint32_t car=fm6_carriers(fm6_eff[0].p[FP_ALG]);
    for(uint32_t k=0;k<6;k++)if(car>>k&1)assert(!memcmp(patch+k*FP_OP,fm6_eff[0].p+k*FP_OP,FP_OP));
    voice_t v={.active=1,.gate=1};
    /* Exercise ADSR using the actual allocated voice address. */
    t->v[0]=v;performance[0]=performance_default();performance[0].amp=1;
    performance[0].attack=5;performance[0].release=5;performance_start(t,&t->v[0]);
    int32_t before=0;
    for(uint32_t b=0;b<30;b++){int32_t a=performance_gain(t,&t->v[0],1);assert(a>=before);before=a;}
    t->v[0].gate=0;for(uint32_t b=0;b<100;b++)performance_gain(t,&t->v[0],1);assert(!t->v[0].active);
    puts("performance: FM6 native release survives generic ADSR, user fade ends voices, FM AMP attack/release and carrier-safe non-destructive tone mapping passed");
}
static void fade_overrides_fast_native_release(void)
{
    reset();track_t *t=&trk[0];host_preset(t,ENGI_FM6,24);
    uint8_t patch[FP_SIZE+1];fm6_unpack(FM6_INIT,patch);
    uint8_t *op=patch+5u*FP_OP;
    for(uint32_t k=0;k<4;k++)op[FP_R1+k]=99;
    op[FP_L1]=op[FP_L1+1]=op[FP_L1+2]=99;op[FP_L1+3]=0;
    fm6_put_patch(0,patch,1);performance[0].fade=20;
    trk_note_on(t,60,100);int32_t pcm[CTL];uint32_t voice=0;
    for(uint32_t k=0;k<NVOICE;k++)if(t->v[k].gate)voice=k;
    for(uint32_t b=0;b<100;b++)track_render(t,pcm,CTL);
    trk_note_off(t,60);uint64_t energy=0;
    for(uint32_t b=0;b<50;b++){track_render(t,pcm,CTL);for(uint32_t i=0;i<CTL;i++)energy+=(uint32_t)abs(pcm[i]);}
    assert(energy && t->v[voice].active && fm6_state(t,&t->v[voice])->down);
    for(uint32_t b=0;b<800;b++)track_render(t,pcm,CTL);
    assert(!t->v[voice].active);
    puts("performance: explicit FM6 FADE retains native key state during outer release, so fast R4 cannot defeat it; voice retires at fade end");
}
static void declick(void)
{
    fm6_voice_t s={0};vmod_t m={.amp0=32767,.amp1=32767};
    float sum[CTL];int32_t out[CTL];for(uint32_t i=0;i<CTL;i++)sum[i]=-0.1f;
    s.output_last=1000;s.correction_pending=1;
    fm6_output(&s,sum,out,CTL,&m,VOICE_FS,0);assert(out[0]==1000);
    for(uint32_t k=0;k<8;k++)fm6_output(&s,sum,out,CTL,&m,VOICE_FS,0);
    assert(!s.correction);
    puts("performance: FM6 reused-voice first sample continuity and bounded correction settling passed");
}
static void save_versions(void)
{
    reset();uint8_t legacy[TESS_FEATURE_BYTES],full[TESS_FULL_BYTES],copy[TESS_FULL_BYTES];
    harm_track[0].bias=72;features_legacy_pack(legacy);
    performance[0].amp=1;performance[0].mode=DUR_HIT;performance[0].fade=25;
    features_apply(legacy);assert(!performance[0].amp && performance[0].mode==DUR_STEP && harm_track[0].bias==72);
    for(uint32_t k=0;k<NTRK;k++)performance[k]=(performance_t){2,5,127,1,1,127,127,127,127,127,127,127,127};
    tess_full_pack(full);tessera_reset();assert(tess_full_unpack(full,sizeof full));tess_full_pack(copy);assert(!memcmp(full,copy,sizeof full));
    full[sizeof full-1]=128;assert(!tess_full_valid(full,sizeof full));
    puts("performance: HMF1 migration defaults to bypass, full-range HMF2 round-trip and reserved-bit rejection passed");
}
int main(void){timing();capture_lengths();fm_audio();fade_overrides_fast_native_release();declick();save_versions();return 0;}
