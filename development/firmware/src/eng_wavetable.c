/* SPDX-License-Identifier: GPL-3.0-only
 * Original spectral wavetable engine — By: Inputsignwave.
 * Four wave frames, three fold spectra and four pitch-dependent harmonic limits.
 * No nonlinear sample-rate fold: folding is baked into bandwidth-limited tables.
 * Linear phase/frame/fold/band interpolation and block parameter ramps are bounded.
 * The immutable bank is shared across tracks; CPU0 prepares it before rendering.
 * Per-voice state: s[0..1] tone poles, s[2..5] wave/fold/tone/band ramps,
 * s[6] initialised; ph[0] oscillator, ph[1] scan, ph[2] sub oscillator. */
#if MELODEE_WAVETABLE
#include "wavetable_bank.h"
static const wt_bank_t *wt_bank;
static int wt_prepare(void); /* engines.c: audio-owner allocation, no worker mutation */
static void wt_note_on(track_t *t,voice_t *v)
{
    (void)t;v->ph[1]=0;v->ph[2]=v->ph[0]/2u;
    for(uint32_t i=0;i<8u;i++)v->s[i]=0;
}
static inline __attribute__((always_inline)) int32_t wt_lerp(int32_t a,int32_t b,uint32_t f)
{ return a+(int32_t)(((b-a)*(int32_t)f)>>16); }
static inline __attribute__((always_inline)) int32_t wt_point(const int16_t *row,uint32_t ph)
{
    uint32_t i=ph>>25,f=(ph>>9)&65535u;
    return wt_lerp(row[i],row[i+1u],f);
}
static inline __attribute__((always_inline)) int32_t wt_face(uint32_t frame,uint32_t fold,uint32_t band,uint32_t ph,uint32_t wf,uint32_t ff)
{
    const wt_bank_t *b=wt_bank;
    int32_t a=wt_lerp(wt_point((*b)[frame][fold][band],ph),wt_point((*b)[frame+1u][fold][band],ph),wf);
    int32_t z=wt_lerp(wt_point((*b)[frame][fold+1u][band],ph),wt_point((*b)[frame+1u][fold+1u][band],ph),wf);
    return wt_lerp(a,z,ff);
}
static int32_t wt_band_position(uint32_t inc)
{
    for(uint32_t b=0;b<WT_BANDS-1u;b++){
        uint32_t high=0x80000000u/WT_HARMONICS[b],low=high-high/4u;
        if(inc<=low)return (int32_t)(b*65536u);
        if(inc<high)return (int32_t)(b*65536u+(uint32_t)clamp((int32_t)((inc-low)/((high-low)>>16)),0,65535));
    }
    return (WT_BANDS-1u)*65536;
}
static __attribute__((noinline)) void wt_render(track_t *t,voice_t *v,int32_t *out,uint32_t n,const vmod_t *m)
{
    if(!n || !wt_bank)return;
    if(n>CTL)n=CTL;
    const int16_t *p=t->p;
    uint32_t inc=cents_inc(m->pitch16+clamp(p[P_E6],-24,24)*16,clamp(p[P_E7],-50,50),m->fine);
    if(inc>0x7fffffffu)inc=0x7fffffffu;
    int32_t wave=clamp(p[P_E0],0,127)*256+m->shape-64*256;
    if(p[P_E3])wave+=sine_i(v->ph[1])*clamp(p[P_E3],0,127)/128;
    v->ph[1]+=LFO_INC[clamp(p[P_E2],0,127)]*n/CTL;
    int32_t target[4];
    target[0]=clamp(wave,0,127*256)*3*256/127;
    target[1]=clamp(p[P_E1],0,127)*2*65536/127;
    int32_t tone=clamp(p[P_E4]*256+m->cutoff,0,127*256),hz=80+(tone/256)*(tone/256)*18000/16129;
    target[2]=tone==127*256?32768:hz*32768/(hz+7000);
    target[3]=wt_band_position(inc);
    if(!v->s[6]){for(uint32_t k=0;k<4u;k++)v->s[2u+k]=target[k];v->s[6]=1;}
    int32_t cur[4],step[4];for(uint32_t k=0;k<4u;k++){cur[k]=v->s[2u+k];step[k]=(target[k]-cur[k])/(int32_t)n;}
    uint32_t phase=v->ph[0],sub=v->ph[2];int32_t low=v->s[0],low2=v->s[1],submix=clamp(p[P_E5],0,127);
    for(uint32_t i=0;i<n;i++){
        for(uint32_t k=0;k<4u;k++)cur[k]+=step[k];
        uint32_t a=(uint32_t)cur[0]>>16,fa=(uint32_t)cur[0]&65535u;
        uint32_t f=(uint32_t)cur[1]>>16,ff=(uint32_t)cur[1]&65535u;
        uint32_t b=(uint32_t)cur[3]>>16,bf=(uint32_t)cur[3]&65535u;
        if(a>=WT_FRAMES-1u){a=WT_FRAMES-2u;fa=65536u;}
        if(f>=WT_FOLDS-1u){f=WT_FOLDS-2u;ff=65536u;}
        if(b>=WT_BANDS-1u){b=WT_BANDS-2u;bf=65536u;}
        int32_t x=wt_lerp(wt_face(a,f,b,phase,fa,ff),wt_face(a,f,b+1u,phase,fa,ff),bf);
        int32_t subwave=sine_i(sub)*14000/32768;
        x+=(subwave-x)*submix/254;
        low+=(int32_t)(((x-low)*cur[2])>>15);
        low2+=(int32_t)(((low-low2)*cur[2])>>15);
        out[i]+=low2*2;phase+=inc;sub+=inc/2u;
    }
    v->ph[0]=phase;v->ph[2]=sub;v->s[0]=low;v->s[1]=low2;
    for(uint32_t k=0;k<4u;k++)v->s[k+2u]=target[k];
}
static const preset_t WT_PRESETS[]={
    {"INIT WAVE",{0,0,0,0,127,0,0,0},{0,45,127,35},0,0},
    {"HOLLOW KEYS",{42,20,25,8,112,0,0,0},{0,55,90,45},0,0},
    {"GLASS DRIFT",{85,45,38,22,120,0,0,0},{25,65,110,65},0,0},
    {"FOLDED BASS",{110,80,0,0,95,45,-12,0},{0,45,90,25},0,1},
    {"SLOW SCAN",{64,35,24,56,110,0,0,0},{65,60,120,80},0,0},
    {"BRIGHT PLUCK",{127,65,0,0,120,0,0,0},{0,70,0,35},35,0},
};
static const engine_t ENG_WAVETABLE={
    .name="WAVETABLE",.page_title={"WAVE SCAN","WAVE TONE"},
    .edit={{"WAVE",F_PCT,0,127,0,0,0},{"FOLD",F_PCT,0,127,0,0,0},
           {"RATE",F_LFOHZ,0,127,24,0,0},{"DEPTH",F_PCT,0,127,0,0,0},
           {"TONE",F_PCT,0,127,127,0,0},{"SUB",F_PCT,0,127,0,0,0},
           {"SEMI",F_INT,-24,24,0,0,"st"},{"FINE",F_INT,-50,50,0,0,"ct"}},
    .presets=WT_PRESETS,.npresets=NELEM(WT_PRESETS),.poly=8,
    .note_on=wt_note_on,.render=wt_render,.knob={P_E0,P_E1,P_E4,P_E3},
};
#endif
