/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave.
 * Generator-local musical time; no hardware, float accumulation or LCM buffer. */
#pragma once
#include <stdint.h>
#ifndef MELODEE_FREESTANDING
#include <string.h>
#endif /* target unity build supplies its freestanding libc */
#define S7_QUARTER 1048576u
#define S7_PHASE_UNITS 4096u
typedef struct {
    uint8_t enabled,numerator,denominator,group_count,group[8];
    uint8_t ratio_n,ratio_d,freeze,reset,condition,reference;
    uint16_t phase;
    int16_t drift; /* basis points: -100..100; positive runs faster */
} s7_meter_t;
typedef struct {
    uint64_t next,last,remainder,denominator;
    uint32_t index;
    uint8_t started;
} s7_clock_t;
static s7_meter_t s7_meter_default(void)
{
    s7_meter_t m={0};m.numerator=4;m.denominator=4;m.group_count=4;
    for(uint32_t i=0;i<4;i++)m.group[i]=1;
    m.ratio_n=m.ratio_d=1;return m;
}
static int s7_meter_valid(const s7_meter_t *m)
{
    if(m->enabled>1||!m->numerator||m->numerator>16||
       (m->denominator!=4&&m->denominator!=8&&m->denominator!=16)||
       !m->group_count||m->group_count>8||!m->ratio_n||m->ratio_n>8||
       !m->ratio_d||m->ratio_d>8||m->phase>=S7_PHASE_UNITS||
       m->drift < -100||m->drift>100||m->freeze>1||m->reset>3||
       m->condition>10||m->reference>3)return 0;
    uint32_t sum=0;
    for(uint32_t i=0;i<8;i++){
        if(i<m->group_count){if(!m->group[i]||m->group[i]>16)return 0;sum+=m->group[i];}
        else if(m->group[i])return 0;
    }
    return sum==m->numerator;
}
static uint32_t s7_bar_units(const s7_meter_t *m)
{return S7_QUARTER*4u*m->numerator/m->denominator;}
static int s7_group_accent(const s7_meter_t *m,uint64_t time)
{
    uint32_t pos=(uint32_t)(time%s7_bar_units(m)),at=0;
    for(uint32_t i=0;i<m->group_count;i++){
        if(pos==at)return i?1:2;
        at+=S7_QUARTER*4u*m->group[i]/m->denominator;
    }
    return 0;
}
static int s7_clock_start(s7_clock_t *c,const s7_meter_t *m,uint64_t now)
{
    if(!s7_meter_valid(m))return 0;
    memset(c,0,sizeof *c);
    c->next=now+(uint64_t)s7_bar_units(m)*m->phase/S7_PHASE_UNITS;
    return 1;
}
/* One event at most per call. The caller bounds its queue and calls again only
 * for an admitted due event. Rejected configuration never changes the clock. */
static int s7_clock_take(s7_clock_t *c,const s7_meter_t *m,uint64_t now,
                         uint32_t base_n,uint32_t base_d,uint64_t *onset)
{
    if(!s7_meter_valid(m)||!base_n||base_n>16||!base_d||base_d>8||c->next>now)return 0;
    uint64_t den=(uint64_t)base_d*m->ratio_n*(uint32_t)(10000+(m->freeze?0:m->drift));
    uint64_t num=(uint64_t)S7_QUARTER*base_n*m->ratio_d*10000u;
    if(c->denominator&&c->denominator!=den)c->remainder=c->remainder*den/c->denominator;
    c->denominator=den;
    *onset=c->last=c->next;c->started=1;c->index++;
    uint64_t value=num+c->remainder;
    c->next+=value/den;c->remainder=value%den;
    return 1;
}
/* Re-align changes future placement only: a hit already emitted at `now`
 * cannot be duplicated, and the caller retains ownership of sounding notes. */
static int s7_clock_realign(s7_clock_t *c,const s7_meter_t *m,uint64_t now,int quantized)
{
    if(!s7_meter_valid(m))return 0;
    uint64_t span=s7_bar_units(m),at=quantized?(now/span+1u)*span:now;
    at+=(uint64_t)span*m->phase/S7_PHASE_UNITS;
    if(c->started&&at<=c->last)at+=span;
    c->next=at;c->remainder=c->denominator=0;return 1;
}
