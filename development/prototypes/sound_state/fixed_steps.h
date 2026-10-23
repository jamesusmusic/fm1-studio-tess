/* SPDX-License-Identifier: GPL-3.0-only
 * Fixed-capacity sound-state packing research — By: Inputsignwave.
 * Four valid steps: 112 note bits + a 129-bit mixed-radix control record = 241.
 * Existing 61-bit encoding takes 244 bits. All unused notes/fields survive.
 * No compression ratio assumptions, flash access or production format changes. */
#ifndef INPUTSIGNWAVE_FIXED_STEPS_H
#define INPUTSIGNWAVE_FIXED_STEPS_H
#include "../../firmware/src/studio7_step.h"
#define S10_GROUP_BITS 241u
static void s10_muladd(uint32_t a[5],uint32_t base,uint32_t value)
{
 uint64_t carry=value;for(unsigned i=0;i<5;i++){carry+=(uint64_t)a[i]*base;a[i]=(uint32_t)carry;carry>>=32;}
}
static uint32_t s10_div(uint32_t a[5],uint32_t base)
{
 uint64_t rem=0;for(int i=4;i>=0;i--){uint64_t x=(rem<<32)|a[i];a[i]=(uint32_t)(x/base);rem=x%base;}return (uint32_t)rem;
}
static int s10_steps_pack(uint8_t *out,uint32_t cap,uint32_t *at,const uint8_t raw[4][8])
{
 if(!out||!at||!raw||*at>cap||cap-*at<S10_GROUP_BITS)return 0;
 uint32_t a[5]={0},notes[4];
 for(unsigned i=0;i<4;i++){
  uint8_t packed[8]={0};uint32_t p=0;if(!s7_step_pack(packed,&p,raw[i]))return 0;
  p=0;notes[i]=s7_bits_get(raw[i],&p,28);uint32_t h=s7_bits_get(raw[i],&p,4),flags=s7_bits_get(raw[i],&p,2),velocity=s7_bits_get(raw[i],&p,7);
  if((h&7u)>6u)return 0;
  p=41;uint32_t rank=s7_bits_get(packed,&p,13),prob=s7_bits_get(packed,&p,7);if(prob>101u)return 0;
  s10_muladd(a,14,(h&7u)+(h>>3)*7u);s10_muladd(a,4,flags);s10_muladd(a,128,velocity);s10_muladd(a,6816,rank);s10_muladd(a,102,prob);
 }
 for(unsigned i=0;i<4;i++)s7_bits_put(out,at,notes[i],28);
 for(unsigned i=0;i<129;i++)s7_bits_put(out,at,(a[i/32]>>(i%32))&1u,1);
 return 1;
}
static int s10_steps_unpack(uint8_t raw[4][8],const uint8_t *in,uint32_t cap,uint32_t *at)
{
 if(!raw||!in||!at||*at>cap||cap-*at<S10_GROUP_BITS)return 0;
 uint32_t a[5]={0},p=*at,notes[4];uint8_t tmp[4][8];
 for(unsigned i=0;i<4;i++)notes[i]=s7_bits_get(in,&p,28);
 for(unsigned i=0;i<129;i++)a[i/32]|=s7_bits_get(in,&p,1)<<(i%32);
 for(int i=3;i>=0;i--){
  uint32_t prob=s10_div(a,102),rank=s10_div(a,6816),velocity=s10_div(a,128),flags=s10_div(a,4),h=s10_div(a,14);
  uint8_t packed[8]={0};uint32_t q=0;s7_bits_put(packed,&q,notes[i],28);s7_bits_put(packed,&q,(h%7u)|((h/7u)<<3),4);s7_bits_put(packed,&q,flags,2);s7_bits_put(packed,&q,velocity,7);s7_bits_put(packed,&q,rank,13);s7_bits_put(packed,&q,prob,7);
  q=0;if(!s7_step_unpack(tmp[i],packed,&q))return 0;
 }
 for(unsigned i=0;i<5;i++)if(a[i])return 0;
 memcpy(raw,tmp,sizeof tmp);*at=p;return 1;
}
#endif
