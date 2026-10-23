/* SPDX-License-Identifier: GPL-3.0-only
 * Versioned performance FX controls — By: Inputsignwave.
 * Ordinary track IDs and all legacy settings remain untouched. */
#pragma once
#include "studio7_step.h"
#define SFX_SLOTS 15u
#define SFX_BYTES 65u
#define SFX_WIRE_BYTES 61u
typedef struct { uint8_t slot[SFX_SLOTS][3],plate[20]; } sound_fx_t;
_Static_assert(sizeof(sound_fx_t)==SFX_BYTES,"flat seven-bit FX state");
static const sound_fx_t SFX_DEFAULT={.plate={25,64,6,112,80,127,24,24,0,0,0,60,0}};
static sound_fx_t sound_fx={.plate={25,64,6,112,80,127,24,24,0,0,0,60,0}};
static int sfx_valid(const sound_fx_t *s)
{
 const uint8_t *p=(const uint8_t *)s;for(uint32_t i=0;i<SFX_BYTES;i++)if(p[i]>127u)return 0;
 for(uint32_t i=0;i<SFX_SLOTS;i++)if(((s->slot[i][0]>>2)&7u)>4u)return 0;
 if(s->plate[8]>10u || s->plate[10]>1u || s->plate[12]>1u)return 0;
 for(uint32_t i=13;i<20;i++)if(s->plate[i])return 0;
 return 1;
}
static int sfx_encode(uint8_t *raw,const sound_fx_t *s)
{
 if(!sfx_valid(s))return 0;
 memset(raw,0,SFX_WIRE_BYTES);raw[0]='S';raw[1]='F';raw[2]='X';raw[3]=1;
 uint32_t bit=32;const uint8_t *p=(const uint8_t *)s;
 for(uint32_t i=0;i<SFX_BYTES;i++)s7_bits_put(raw,&bit,p[i],7u);
 return 1;
}
static int sfx_decode(sound_fx_t *out,const uint8_t *raw)
{
 sound_fx_t state;uint8_t *p=(uint8_t *)&state;uint32_t bit=32;
 if(raw[0]!='S'||raw[1]!='F'||raw[2]!='X'||raw[3]!=1u || (raw[SFX_WIRE_BYTES-1u]&128u))return 0;
 for(uint32_t i=0;i<SFX_BYTES;i++)p[i]=(uint8_t)s7_bits_get(raw,&bit,7u);
 if(!sfx_valid(&state))return 0;
 if(out)*out=state;return 1;
}
