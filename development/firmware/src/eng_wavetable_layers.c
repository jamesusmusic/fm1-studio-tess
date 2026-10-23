/* SPDX-License-Identifier: GPL-3.0-only
 * Independent layered spectral engine — By: Inputsignwave.
 * New engine identity; existing WAVETABLE sounds and parameter meanings survive.
 * All sound values use the existing versioned parameter, MIDI and undo contracts.
 * Wave assets retain Surge/Adventure Kid provenance in wavetable_layers_bank.h. */
#if MELODEE_WAVETABLE
#include "wavetable_layers_bank.h"
#define WL_POLY 4u
static const wl_bank_t *wl_bank;
static uint8_t wl_prepared;
typedef struct {
    uint32_t phase, scan, age;
    int32_t envelope, low, low2, position, cutoff, gain;
    uint16_t fade;
    uint8_t stage, bank, previous, ready;
} wl_layer_t;
typedef struct { wl_layer_t layer[WL_POLY][2]; uint32_t morph; } wl_part_t;
static wl_part_t *wl_part(const track_t *t); /* CPU0-managed engine handle */
static uint32_t div_samples(uint32_t div);
static const uint8_t WL_PARAM[2][15]={
 {P_ATK,P_DEC,P_SUS,P_REL,P_FM1_LEVEL,P_FM2_ATK,P_FM2_DEC,P_FM2_SUS,P_FM3_SUS,P_FM4_ATK,P_FM4_DEC,P_LN0,P_LN2,P_LN4,P_LN6},
 {P_FM1_ATK,P_FM1_DEC,P_FM1_SUS,P_FM1_REL,P_FM2_LEVEL,P_FM2_REL,P_FM3_ATK,P_FM3_DEC,P_FM3_REL,P_FM4_SUS,P_FM4_REL,P_LN1,P_LN3,P_LN5,P_LN7}
};
/* Names/ranges are scoped to ID 22; they do not reinterpret any old engine. */
static const param_desc_t WL_ED[20]={
 {"BATK",F_TIME,0,127,0,0,0},{"BDEC",F_TIME,0,127,55,0,0},
 {"BSUS",F_PCT,0,127,100,0,0},{"BREL",F_TIME,0,127,60,0,0},
 {"ALVL",F_PCT,0,127,127,0,0},{"ACUT",F_PCT,0,127,110,0,0},
 {"ARES",F_PCT,0,100,0,0,0},{"AENV",F_OFS,0,127,64,0,0},
 {"BCUT",F_PCT,0,127,110,0,0},{"BLVL",F_PCT,0,127,0,0,0},
 {"BRES",F_PCT,0,100,0,0,0},{"BENV",F_OFS,0,127,64,0,0},
 {"ATUN",F_OFS,0,96,48,0,"st"},{"BTUN",F_OFS,0,96,48,0,"st"},
 {"FINE",F_OFS,0,100,50,0,"ct"},{"RATE",F_LFOHZ,0,127,24,0,0},
 {"SCAN",F_PCT,0,127,0,0,0},{"RATE",F_LFOHZ,0,127,30,0,0},
 {"SCAN",F_PCT,0,127,0,0,0},{"TRIG",F_ONOFF,0,1,0,0,0}
};
static const param_desc_t WL_SCAN_ED[8]={
 {"RISE",F_TIME,0,127,0,0,0},{"RISE",F_TIME,0,127,0,0,0},
 {"AEND",F_PCT,0,127,100,0,0},{"BEND",F_PCT,0,127,20,0,0},
 {"ACUT",F_PCT,0,127,127,0,0},{"BCUT",F_PCT,0,127,80,0,0},
 {"SYNC",F_INT,0,10,0,0,0},{"SYNC",F_INT,0,10,0,0,0}
};
static const param_desc_t *wl_param(uint32_t id)
{
 if(id>=P_FM1_ATK && id<=P_FM4_LEVEL)return &WL_ED[id-P_FM1_ATK];
 if(id>=P_LN0 && id<=P_LN7)return &WL_SCAN_ED[id-P_LN0];
 return 0;
}
static uint32_t wl_increment(int32_t rate,int32_t sync,uint32_t n)
{
 static const uint8_t div[11]={0,9,8,7,6,0,1,4,2,5,3};
 if(!sync)return LFO_INC[clamp(rate,0,127)]*n/CTL;
 uint32_t period=div_samples(div[clamp(sync,1,10)]);
 return period?(uint32_t)((0xffffffffu/period)*n):0;
}
static void wl_note_on(track_t *t,voice_t *v)
{
 wl_layer_t *l=wl_part(t)->layer[v-t->v];
 for(uint32_t k=0;k<2;k++){
  /* Sounding retriggers retain phase, filters and amplitude; attack rises
   * from the current level. Cold notes start at zero. */
  if(!v->env && !v->env_out)memset(&l[k],0,sizeof l[k]);
  l[k].stage=1;l[k].age=0;
  if(t->p[P_FM4_LEVEL])l[k].scan=0;
 }
}
static int wl_done(track_t *t,voice_t *v)
{
 wl_layer_t *l=wl_part(t)->layer[v-t->v];
 return !v->gate && (l[0].envelope<2048 || (!t->p[P_FM1_LEVEL]&&!l[0].gain)) &&
        (l[1].envelope<2048 || (!t->p[P_FM2_LEVEL]&&!l[1].gain));
}
static int32_t wl_point(uint32_t bank,uint32_t frame,uint32_t band,uint32_t phase)
{
 static const uint8_t offset[4]={0,65,98,115},shift[4]={26,27,28,29};
 const int8_t *p=(*wl_bank)[bank][frame]+offset[band];
 if(band==3u)return clamp((sine_i(phase)*p[1]+sine_i(phase+0x40000000u)*p[0])>>7,-32512,32512);
 uint32_t i=phase>>shift[band],f=(phase>>(shift[band]-15u))&32767u;
 return (int32_t)p[i]*256+(((int32_t)p[i+1u]-p[i])*(int32_t)f>>7);
}
static int32_t wl_lerp(int32_t a,int32_t b,uint32_t f)
{ return a+(((b-a)*(int32_t)(f>>1))>>15); }
static int32_t wl_frame(uint32_t bank,uint32_t band,uint32_t phase,int32_t position)
{
 uint32_t f=(uint32_t)clamp(position,0,127*65536)*(WL_FRAMES-1u)/127u;
 uint32_t row=f>>16,blend=f&65535u;
 if(row>=WL_FRAMES-1u){row=WL_FRAMES-2u;blend=65536;}
 return wl_lerp(wl_point(bank,row,band,phase),wl_point(bank,row+1u,band,phase),blend);
}
static int32_t wl_sample(wl_layer_t *l,uint32_t inc,int32_t position)
{
 static const uint8_t harmonics[4]={15,7,3,1};
 uint32_t band=0,bf=0;
 for(;band<3u;band++){
  uint32_t high=0x80000000u/harmonics[band],low=high-high/4u;
  if(inc<high){if(inc>low)bf=(inc-low)/((high-low)>>16);break;}
 }
 int32_t x=wl_frame(l->bank,band,l->phase,position);
 if(band<3u && bf)x=wl_lerp(x,wl_frame(l->bank,band+1u,l->phase,position),bf);
 if(l->fade<32768u){
  int32_t old=wl_frame(l->previous,band,l->phase,position);
  if(band<3u && bf)old=wl_lerp(old,wl_frame(l->previous,band+1u,l->phase,position),bf);
  x=old+((x-old)*(int32_t)l->fade>>15);l->fade+=256;
 }
 l->phase+=inc;return x;
}
static void wl_block(track_t *t)
{
 wl_part_t *p=wl_part(t);p->morph+=wl_increment(t->p[P_E6],t->p[P_E7],CTL);
}
static __attribute__((noinline)) void wl_render(track_t *t,voice_t *v,int32_t *out,uint32_t n,const vmod_t *m)
{
 if(!n || !wl_bank)return;if(n>CTL)n=CTL;
 wl_part_t *part=wl_part(t);wl_layer_t *layers=part->layer[v-t->v];
 int32_t morph=(sine_i(part->morph)+32768)*clamp(t->p[P_E5],0,127)/65536;
 int32_t blend=clamp(t->p[P_E4]+morph-clamp(t->p[P_E5],0,127)/2,0,127);
 for(uint32_t k=0;k<2u;k++){
  wl_layer_t *l=&layers[k];const uint8_t *ids=WL_PARAM[k];
  int32_t level=clamp(t->p[ids[4]],0,127),start=l->envelope;
  int cold=!l->ready;
  uint32_t bank=(uint32_t)clamp(t->p[P_E0+k*2u],0,7);
  if(!l->ready){l->bank=l->previous=(uint8_t)bank;l->fade=32768;l->ready=1;}
  else if(l->bank!=bank && l->fade==32768u){l->previous=l->bank;l->bank=(uint8_t)bank;l->fade=0;}
  if(!v->gate)l->stage=3;
  if(l->stage==1){l->envelope+=(int32_t)ENV_LIN[t->p[ids[0]]&127]*(int32_t)n/CTL;
   if(l->envelope>=1<<24){l->envelope=1<<24;l->stage=2;}}
  else {int32_t target=l->stage==3?0:clamp(t->p[ids[2]],0,127)*132104;
   l->envelope+=mulq16(target-l->envelope,ENV_EXP[t->p[ids[l->stage==3?3:1]]&127])*(int32_t)n/CTL;}
  if(l->envelope<0)l->envelope=0;
  int32_t estep=(l->envelope-start)/(int32_t)n;
  l->scan+=wl_increment(t->p[ids[9]],t->p[ids[14]],n);l->age+=n;
  int32_t scan=sine_i(l->scan)*clamp(t->p[ids[10]],0,127)/32768;
  int32_t position=clamp(t->p[P_E1+k*2u],0,127),endpoint=clamp(t->p[ids[12]],0,127);
  position+=(endpoint-position)*morph/127;
  position+=(m->shape-64*256)/256;
  /* A separate slow position rise uses the same endpoint; amplitude ADSR
   * remains independent. Zero rise/depth does not move the initial position. */
  int32_t rise=clamp(t->p[ids[11]],0,127);
  if(rise){uint32_t duration=FS/20u+(uint32_t)rise*rise*FS/1024u;
   uint32_t age=l->age<duration?l->age:duration;position+=(endpoint-position)*(int32_t)(age*127u/duration)/127;}
  int32_t ptarget=clamp(position+scan,0,127)*65536,pstep=(ptarget-l->position)/(int32_t)n;
  if(cold){l->position=ptarget;pstep=0;}
  int32_t cut=clamp(t->p[ids[5]],0,127);
  cut+=(clamp(t->p[ids[13]],0,127)-cut)*morph/127;
  cut+=((t->p[ids[7]]-64)*(l->envelope>>16))/256+m->cutoff/256;
  cut=clamp(cut,0,127);int32_t hz=80+cut*cut*17000/16129;
  int32_t coef=hz*32768/(hz+12000),cstep=(coef-l->cutoff)/(int32_t)n;
  uint32_t inc=cents_inc(clamp(m->pitch16+(t->p[ids[8]]-48)*16,0,2047),k?t->p[P_FM3_LEVEL]-50:0,m->fine);
  if(inc>0x7fffffffu)inc=0x7fffffffu;
  int32_t resonance=clamp(t->p[ids[6]],0,100)*160;
  int32_t gain=level*(k?blend:127-blend)*v->vel/127;
  int32_t current_gain=l->gain,gain_step=(gain-current_gain)/(int32_t)n;
  if(!gain && !current_gain){l->position=ptarget;l->cutoff=coef;l->phase+=inc*n;continue;}
  for(uint32_t i=0;i<n;i++){
   start+=estep;current_gain+=gain_step;l->position+=pstep;l->cutoff+=cstep;
   int32_t x=wl_sample(l,inc,l->position);
   /* Two stable bounded poles; resonance feeds only the bounded difference. */
   x-=((l->low2-l->low)*resonance)>>15;
   l->low+=((clamp(x,-32768,32767)-l->low)*l->cutoff)>>15;
   l->low2+=((l->low-l->low2)*l->cutoff)>>15;
   int32_t y=mulq15(l->low2,start>>9);y=y*current_gain/16129;
   out[i]+=voice_amp(y,m,i);
  }
  l->position=ptarget;l->cutoff=coef;l->gain=gain;
 }
}
static const char *const WL_NAMES[]={"VOWL","METL","HLLO","PRIM","RIPL","PHAS","VOX","FM"};
static const preset_t WL_PRESETS[]={
 {"VOWEL GLASS",{0,30,6,60,48,45,20,2},{45,60,110,70},0,0},
 {"METALLIC PM",{1,20,7,65,50,65,28,0},{0,65,70,60},0,0},
 {"HOLLOW MOTION",{2,35,5,70,45,55,24,3},{25,65,110,65},0,0},
 {"PRIME SPECTRUM",{3,25,1,90,35,60,20,2},{60,75,115,80},0,0},
 {"RIPPLE DIGITAL",{4,35,5,45,50,70,30,5},{0,60,90,50},0,0},
 {"PHASE PRISM",{5,20,2,80,55,65,24,2},{45,65,110,75},0,0},
 {"ORGANIC VOICE",{6,30,0,95,40,65,18,1},{65,75,105,80},0,0},
 {"FM SPECTRAL",{7,25,1,75,50,70,22,2},{30,65,100,70},0,0},
 {"INIT LAYERS",{0,0,1,0,0,0,24,0},{0,45,127,35},0,0}
};
static void wl_preset_loaded(track_t *t,uint32_t preset)
{
 if(t->eng_req!=ENGI_WAVE_LAYERS)return;
 for(uint32_t i=P_FM1_ATK;i<=P_FM4_LEVEL;i++)t->p[i]=wl_param(i)->def;
 for(uint32_t i=P_LN0;i<=P_LN7;i++)t->p[i]=wl_param(i)->def;
 if(preset!=8u){t->p[P_FM2_LEVEL]=105;t->p[P_FM4_DEC]=12;t->p[P_FM4_REL]=18;t->p[P_LN2]=110;t->p[P_LN3]=15;}
}
static const engine_t ENG_WAVE_LAYERS={
 .name="WAVE LAYERS",.page_title={"TABLES A/B","BLEND MORPH"},
 .edit={{"ABNK",F_ENUM,0,7,0,WL_NAMES,0},{"APOS",F_PCT,0,127,0,0,0},
        {"BBNK",F_ENUM,0,7,1,WL_NAMES,0},{"BPOS",F_PCT,0,127,0,0,0},
        {"A/B",F_PCT,0,127,0,0,0},{"MRPH",F_PCT,0,127,0,0,0},
        {"RATE",F_LFOHZ,0,127,24,0,0},{"SYNC",F_INT,0,10,0,0,0}},
 .presets=WL_PRESETS,.npresets=NELEM(WL_PRESETS),.poly=WL_POLY,
 .note_on=wl_note_on,.render=wl_render,.block=wl_block,.ownenv=1,.done=wl_done,
 .knob={P_E1,P_E3,P_E4,P_E5}
};
#endif
