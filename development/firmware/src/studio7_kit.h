/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave.
 * Bounded, stateless kit development from the user's unmodified pattern.
 * Lane roles: kick, snare, closed hat, open hat, four percussion roles. */
#pragma once
#include <stdint.h>
#include <string.h>
typedef struct {
    uint8_t enabled,density[8],role[8],variation,fill,human,ghost,accent;
    uint8_t anchors[64];
} s7_kit_t;
typedef struct {
    uint8_t hit,accent,velocity[8];
    int8_t offset[8]; /* signed 1/256 step; caller schedules with lookahead */
} s7_kit_out_t;
static uint32_t s7_hash(uint32_t x)
{x^=x>>16;x*=0x7FEB352Du;x^=x>>15;x*=0x846CA68Bu;return x^(x>>16);}
static s7_kit_t s7_kit_default(void)
{
    s7_kit_t k={0};k.ghost=40;k.accent=115;
    for(uint32_t i=0;i<8;i++){k.role[i]=(uint8_t)i;k.density[i]=50;}
    return k;
}
static int s7_kit_valid(const s7_kit_t *k)
{
    if(k->enabled>1||k->variation>100||k->fill>100||k->human>100||
       !k->ghost||k->ghost>127||!k->accent||k->accent>127)return 0;
    uint32_t roles=0;
    for(uint32_t i=0;i<8;i++){
        if(k->density[i]>100||k->role[i]>7||(roles&(1u<<k->role[i])))return 0;
        roles|=1u<<k->role[i];
    }
    return roles==255;
}
static int s7_kit_generate(s7_kit_out_t *out,const s7_kit_t *k,uint32_t seed,
                           uint32_t step,uint32_t bar,uint32_t base_hit,
                           uint32_t base_accent,uint32_t base_velocity,
                           uint32_t group_accent,int phrase_end)
{
    if(!s7_kit_valid(k)||step>=64||base_hit>255||base_accent>255||
       (base_accent&~base_hit)||base_velocity>127||group_accent>2)return 0;
    s7_kit_out_t o={0};
    uint32_t lock=k->anchors[step]&base_hit,closed=0,open=0;
    for(uint32_t l=0;l<8;l++){
        uint32_t bit=1u<<l,role=k->role[l];
        if(role==2)closed=bit;if(role==3)open=bit;
        uint32_t h=s7_hash(seed^step*0x9E3779B9u^role*0x85EBCA6Bu);
        /* Variation is a chance to depart from the repeatable core. At zero,
         * all bars repeat; returning to a bar index is reproducible. */
        if(bar&&s7_hash(h^bar)%100u<k->variation)h=s7_hash(h^bar*0xC2B2AE35u);
        uint32_t density=k->density[l],chance=(base_hit&bit)?density:density/4u;
        if(density&&group_accent&&role<2)chance+=(100u-chance)/3u;
        if(density&&phrase_end)chance+=(100u-chance)*k->fill/100u;
        int play=!k->enabled?(base_hit&bit)!=0:(lock&bit)!=0||h%100u<chance;
        if(!play)continue;
        o.hit|=(uint8_t)bit;
        if((lock&bit)||!k->enabled){if(base_accent&bit)o.accent|=(uint8_t)bit;}
        else if(group_accent&&(h>>8)%100u<k->accent)o.accent|=(uint8_t)bit;
        uint32_t v=base_velocity?base_velocity:96u;
        if(k->enabled&&!(lock&bit)){
            v=(o.accent&bit)?k->accent:((h>>16)%100u<k->variation?k->ghost:v);
            o.offset[l]=(int8_t)((int32_t)((h>>24)%33u)-16)*(int32_t)k->human/100;
        }
        o.velocity[l]=(uint8_t)v;
    }
    if(k->enabled&&(o.hit&closed)&&(o.hit&open)&&!((lock&closed)&&(lock&open))){
        uint32_t remove=(lock&open)?closed:open;
        o.hit&=(uint8_t)~remove;o.accent&=o.hit;
        for(uint32_t l=0;l<8;l++)if(remove&(1u<<l)){o.velocity[l]=0;o.offset[l]=0;}
    }
    *out=o;return 1;
}
