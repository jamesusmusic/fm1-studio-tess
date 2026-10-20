/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave.
 * Lossless 61-bit encoding of the existing 64-bit step wire record.
 * The hit/accent pair has 3^8 ordinary states plus 255 hit-free gate states.
 * No note, inactive field, velocity, probability or bank is discarded. */
#pragma once
#include <stdint.h>
#if __STDC_HOSTED__
#include <string.h>
#endif /* target unity build supplies its freestanding libc */
#define STUDIO7_STEP_BITS 61u
#define STUDIO7_STEP_PAIR_COUNT 6816u
static uint32_t s7_bits_get(const uint8_t *b,uint32_t *at,uint32_t n)
{
    uint32_t v=0;
    for(uint32_t i=0;i<n;i++,(*at)++)v|=(uint32_t)((b[*at>>3]>>(*at&7u))&1u)<<i;
    return v;
}
static void s7_bits_put(uint8_t *b,uint32_t *at,uint32_t v,uint32_t n)
{
    for(uint32_t i=0;i<n;i++,(*at)++){
        uint8_t m=(uint8_t)(1u<<(*at&7u));
        b[*at>>3]=(uint8_t)((b[*at>>3]&~m)|((v>>i&1u)?m:0u));
    }
}
static int s7_step_pack(uint8_t *out,uint32_t *at,const uint8_t in[8])
{
    uint32_t p=41,hit=s7_bits_get(in,&p,8),acc=s7_bits_get(in,&p,8),rank=0,m=1;
    if(hit && (acc&~hit))return 0;
    if(!hit)rank=acc?6560u+acc:0u;
    else for(uint32_t i=0;i<8;i++,m*=3u)rank+=((hit>>i&1u)?(acc>>i&1u)?2u:1u:0u)*m;
    p=0;
    s7_bits_put(out,at,s7_bits_get(in,&p,28),28);
    s7_bits_put(out,at,s7_bits_get(in,&p,13),13);
    s7_bits_put(out,at,rank,13);
    p=57;s7_bits_put(out,at,s7_bits_get(in,&p,7),7);
    return 1;
}
static int s7_step_unpack(uint8_t out[8],const uint8_t *in,uint32_t *at)
{
    uint32_t start=*at,p=0,hit=0,acc=0;
    uint8_t tmp[8]={0};
    s7_bits_put(tmp,&p,s7_bits_get(in,at,28),28);
    s7_bits_put(tmp,&p,s7_bits_get(in,at,13),13);
    uint32_t rank=s7_bits_get(in,at,13);
    if(rank>=STUDIO7_STEP_PAIR_COUNT){*at=start;return 0;}
    if(rank>6560u)acc=rank-6560u;
    else for(uint32_t i=0;i<8;i++,rank/=3u){uint32_t d=rank%3u;if(d)hit|=1u<<i;if(d==2u)acc|=1u<<i;}
    s7_bits_put(tmp,&p,hit,8);s7_bits_put(tmp,&p,acc,8);
    s7_bits_put(tmp,&p,s7_bits_get(in,at,7),7);
    memcpy(out,tmp,8);return 1;
}
