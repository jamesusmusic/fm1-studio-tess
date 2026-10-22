/* SPDX-License-Identifier: GPL-3.0-only
 * Sound extension codec validation — By: Inputsignwave. */
#include "codec.h"
#include <assert.h>
#include <stdio.h>
static uint8_t raw[27904],original[27904],wire[30000],restored[27904],extension[256],copy[256];
int main(void)
{
 uint32_t seed=1;
 for(unsigned mode=0;mode<5;mode++){
  for(size_t i=0;i<sizeof raw;i++){seed=seed*1664525u+1013904223u;raw[i]=mode==0?0:mode==1?(uint8_t)i:mode==2?(uint8_t)(seed>>24):mode==3?(uint8_t)(i/64):(uint8_t)(i%7?0:seed>>24);}
  memcpy(original,raw,sizeof raw);for(unsigned i=0;i<sizeof extension;i++)extension[i]=(uint8_t)(i*173);
  size_t n=isx_encode(wire,sizeof wire,raw,sizeof raw,extension,sizeof extension);assert(n);
  assert(isx_decode(restored,sizeof restored,copy,sizeof copy,wire,n));assert(!memcmp(raw,restored,sizeof raw)&&!memcmp(copy,extension,sizeof copy)&&!memcmp(original,raw,sizeof raw));
  if(mode==0)assert(n<27904);if(mode==2)assert(!isx_encode(wire,27904,raw,sizeof raw,extension,sizeof extension));
  n=isx_encode(wire,sizeof wire,raw,sizeof raw,extension,sizeof extension);assert(n);
  memset(restored,0x5a,sizeof restored);memset(copy,0x5a,sizeof copy);
  for(size_t k=0;k<n;k+=17){uint8_t old=wire[k];wire[k]^=1;assert(!isx_decode(restored,sizeof restored,copy,sizeof copy,wire,n));wire[k]=old;for(size_t i=0;i<sizeof restored;i++)assert(restored[i]==0x5a);for(size_t i=0;i<sizeof copy;i++)assert(copy[i]==0x5a);}
  for(size_t k=0;k<n;k+=13)assert(!isx_decode(restored,sizeof restored,copy,sizeof copy,wire,k));
  printf("codec pattern=%u packed=%zu all legacy bytes + 256 extension bytes preserved\n",mode,n);
 }
 puts("PASS: round trip, corruption/truncation rejection, atomic decode and capacity failure");
}
