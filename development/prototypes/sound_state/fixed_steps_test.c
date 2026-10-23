/* SPDX-License-Identifier: GPL-3.0-only
 * Full valid state-space components and fixed size checks — By: Inputsignwave. */
#include "fixed_steps.h"
#include <assert.h>
#include <stdio.h>
static uint8_t wire[27904],original[4][8],decoded[4][8];
int main(void)
{
 uint32_t rng=1;
 for(uint32_t rank=0;rank<6816;rank++)for(unsigned h=0;h<14;h++){
  for(unsigned i=0;i<4;i++){
   rng=rng*1664525u+1013904223u;uint8_t b[8]={0};uint32_t p=0;
   s7_bits_put(b,&p,rng&0xfffffff,28);s7_bits_put(b,&p,(h%7u)|((h/7u)<<3),4);s7_bits_put(b,&p,i,2);s7_bits_put(b,&p,(rng>>24)&127u,7);s7_bits_put(b,&p,(rank+i)%6816,13);s7_bits_put(b,&p,(rng>>16)%102u,7);p=0;assert(s7_step_unpack(original[i],b,&p));
  }
  uint32_t at=5,read=5;assert(s10_steps_pack(wire,sizeof wire*8u,&at,original));assert(at==5+241);
  assert(s10_steps_unpack(decoded,wire,sizeof wire*8u,&read));assert(read==at&&!memcmp(original,decoded,sizeof original));
 }
 uint32_t at=0;for(unsigned i=0;i<1792/4;i++)assert(s10_steps_pack(wire,sizeof wire*8u,&at,original));
 assert(at==1792u/4u*241u);assert(1792u*61u-at==1344u);
 uint32_t read=0;for(unsigned i=0;i<1792/4;i++){assert(s10_steps_unpack(decoded,wire,at,&read));assert(!memcmp(original,decoded,sizeof original));}
 memset(wire,255,sizeof wire);memset(decoded,0x5a,sizeof decoded);read=0;
 assert(!s10_steps_unpack(decoded,wire,241,&read)&&!read);for(unsigned i=0;i<sizeof decoded;i++)assert(((uint8_t *)decoded)[i]==0x5a);
 puts("PASS: 95424 valid rank/head combinations, all note bytes and inactive fields, 168 B guaranteed bank saving; invalid input atomic");
}
