/* SPDX-License-Identifier: GPL-3.0-only
 * Shared plate routing, warm drive and slot modulation — By: Inputsignwave.
 * Included by fx.c after the unchanged quarter-rate converter/return helpers. */
#if !MELODEE_RETIRED_REFERENCE
#define PLATE_RATE 11025
#include "plate.c"
static plate_state fx_plate;
static uint8_t fx_plate_owner=255;
static uint32_t fx_perform_increment(uint32_t rate,uint32_t sync,uint32_t n)
{
 static const uint8_t div[13]={9,8,7,6,0,1,4,2,5,3,0,1,2};
 if(!sync)return LFO_INC[rate&127u]*n/CTL;
 rate%=13u;uint32_t cycle=div_samples(div[rate]);if(rate>=10u)cycle=cycle*3u/2u;
 return cycle?(0xffffffffu/cycle)*n:0;
}
static int32_t fx_mod_value(fx_state_t *f,uint32_t si,uint32_t n,uint32_t *destination)
{
 const uint8_t *c=sound_fx.slot[si];*destination=(c[0]>>2)&7u;
 if(!*destination || !c[2])return 0;
 f->mod_phase+=fx_perform_increment(c[1],c[0]&32u,n);
 int32_t wave;
 switch(c[0]&3u){
 case 1: wave=plate_tri(f->mod_phase);break;
 case 2: wave=(int32_t)(f->mod_phase>>16)-32768;break;
 case 3: wave=f->mod_phase&0x80000000u?32767:-32768;break;
 default:wave=sine_i(f->mod_phase);break;
 }
 return wave*c[2]/32768;
}
static void fx_plate_set(fx_state_t *f)
{
 const uint8_t *p=sound_fx.plate;
 plate_params a={0};
 a.feedback=12000+f->character*19129/127;
 a.damping=32700-p[4]*32000/127;
 a.tone=clamp(((int32_t)p[1]-64)+(int32_t)f->q[0][3].b2,-64,63)*256;
 a.lowcut=1+p[2]*p[2]*14000/16129;
 a.highcut=256+p[3]*p[3]*32444/16129;
 a.width=p[5]*32768/127;a.depth=p[6]*PLATE_SCALE(24)*256/127;
 a.increment=fx_perform_increment(p[8]?p[8]-1u:p[7],p[8]!=0u,4u);
 a.predelay=p[0]*(PLATE_PRE-1)/127;
 a.size=8192+f->rate*24576/127;a.wobble=p[9]*32768/127;
 a.swell_samples=p[10]?(PLATE_RATE/50+p[11]*p[11]*PLATE_RATE/1024):0;
 a.hold=p[12];plate_set(&fx_plate,&a);
}
static void fx_plate_run(fx_state_t *f,int16_t *memory,uint32_t n,int32_t target)
{
 fx_plate.memory=memory; /* immutable handle snapshot, CPU0 prepared capacity */
 uint32_t (*volatile down)(fx_state_t *,uint32_t)=fx_down;
 uint32_t count=down(f,n);
 for(uint32_t i=0;i<count;i++){
  int32_t l,r;int32_t mono=f->mono?fx_low_l[i]:(fx_low_l[i]+fx_low_r[i])/2;
  plate_tick(&fx_plate,mono,&l,&r);fx_low_l[i]=l*2;fx_low_r[i]=r*2;
 }
 f->mono=0;
 void (*volatile up)(fx_state_t *,uint32_t,int32_t)=fx_up;up(f,n,target);
}
static float fx_warm_shape(float x)
{ return x/(1.0f+(x<0?-x:x)); }
static void fx_drive_run(fx_state_t *f,uint32_t n,int32_t target)
{
 int32_t mix=f->mix;
 float drive=1.0f+f->rate*(15.0f/127.0f),output=f->character*(2.0f/127.0f);
 for(uint32_t i=0;i<n;i++){
  f->q[0][2].b0+=(drive-f->q[0][2].b0)*(1.0f/256.0f);
  f->q[0][2].b1+=(output-f->q[0][2].b1)*(1.0f/256.0f);
  f->q[0][2].b2+=(f->q[0][3].b1-f->q[0][2].b2)*(1.0f/256.0f);
  int32_t wet[2];
  for(uint32_t ch=0;ch<2u;ch++){
   fx_bq_t *a=&f->q[ch][0],*b=&f->q[ch][1];
   float x=(float)(ch?fx_dry_r[i]:fx_dry_l[i])*(1.0f/32768.0f);
   float g=f->q[0][2].b0,bias=0.08f;
   /* Two interpolated nonlinear evaluations with a short reconstruction
    * average. This limits harsh images; it is not an alias-free claim. */
   float half=fx_warm_shape((x+a->z1)*0.5f*g+bias)-fx_warm_shape(bias);
   float y=(half+fx_warm_shape(x*g+bias)-fx_warm_shape(bias))*0.5f;a->z1=x;
   a->z2+=(y-a->z2)*0.00285f;y-=a->z2;
   float tone=f->q[0][2].b2;
   float coefficient=0.025f+(tone+63.0f)*(0.7f/126.0f);
   b->z1+=(y-b->z1)*coefficient;
   wet[ch]=(int32_t)fx_bound(b->z1*f->q[0][2].b1*49152.0f,65536.0f);
  }
  fx_return(target,0,0,i,wet[0],wet[1],&mix);
 }
 f->mix=mix;
}
#endif
