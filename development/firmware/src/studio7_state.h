/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave.
 * Versioned extension; all ranges/reserved bytes validate before application.
 * S7G1: 4-byte marker followed by four independently encoded 160-byte records. */
#pragma once
#include "gen.h"
#include "studio7_clock.h"
#include "studio7_kit.h"
#define S7_TRACK_BYTES 160u
#define S7_STATE_BYTES (4u+4u*S7_TRACK_BYTES)
typedef struct {
    gen_grids_t grids;
    gen_bounce_t bounce;
    gen_awake_t awake;
    gen_poly_t poly;
    uint16_t seed;
    uint8_t seeded,mode;
    s7_meter_t meter;
    s7_kit_t kit;
} s7_track_t;
static s7_track_t s7_track_default(void)
{
    s7_track_t t={0};t.seed=1;t.grids.x=t.grids.y=128;t.grids.dens=50;
    t.bounce.speed=12;t.bounce.angle=45;t.bounce.balls=1;
    t.awake.len_a=t.awake.len_b=8;t.awake.prob=100;
    for(uint32_t i=0;i<8;i++)t.awake.a[i]=0x21;
    t.poly.div[0]=3;t.poly.div[1]=4;t.poly.span=2;
    for(uint32_t i=0;i<4;i++)t.poly.pitch[i]=(uint8_t)(i*2u);
    t.meter=s7_meter_default();t.kit=s7_kit_default();
    return t;
}
static int s7_track_valid(const s7_track_t *t)
{
    if(!t->seed||t->seed>128||t->seeded>1||t->mode>9||t->grids.dens>100||t->grids.chaos>100||
       !t->bounce.speed||t->bounce.speed>64||t->bounce.angle>90||!t->bounce.balls||t->bounce.balls>4||
       !t->awake.len_a||t->awake.len_a>16||!t->awake.len_b||t->awake.len_b>16||t->awake.prob>100||
       !t->poly.span||t->poly.span>3||!s7_meter_valid(&t->meter)||!s7_kit_valid(&t->kit))return 0;
    for(uint32_t i=0;i<8;i++)if((t->awake.a[i]&15)>8||(t->awake.a[i]>>4)>8||
                                (t->awake.b[i]&15)>7||(t->awake.b[i]>>4)>7)return 0;
    for(uint32_t i=0;i<4;i++)if(t->poly.div[i]>16||t->poly.pitch[i]>20)return 0;
    return 1;
}
static void s7_u16_put(uint8_t **p,uint32_t v)
{*(*p)++=(uint8_t)v;*(*p)++=(uint8_t)(v>>8);}
static uint32_t s7_u16_get(const uint8_t **p)
{uint32_t v=(*p)[0]|(uint32_t)(*p)[1]<<8;*p+=2;
return v;}
static void s7_track_encode(uint8_t out[S7_TRACK_BYTES],const s7_track_t *t)
{
    uint8_t *p=out;memset(out,0,S7_TRACK_BYTES);
    s7_u16_put(&p,t->seed);*p++=t->seeded;
    *p++=t->grids.x;*p++=t->grids.y;*p++=t->grids.dens;*p++=t->grids.chaos;
    *p++=t->bounce.speed;*p++=t->bounce.angle;*p++=t->bounce.balls;
    *p++=t->awake.len_a;*p++=t->awake.len_b;*p++=t->awake.prob;
    memcpy(p,t->awake.a,8);p+=8;memcpy(p,t->awake.b,8);p+=8;
    for(uint32_t i=0;i<4;i++)*p++=t->poly.div[i];
    for(uint32_t i=0;i<4;i++)*p++=t->poly.pitch[i];
    *p++=t->poly.span;
    const s7_meter_t *m=&t->meter;
    *p++=m->enabled;*p++=m->numerator;*p++=m->denominator;*p++=m->group_count;
    memcpy(p,m->group,8);p+=8;
    *p++=m->ratio_n;*p++=m->ratio_d;*p++=m->freeze;*p++=m->reset;
    *p++=m->condition;*p++=m->reference;
    s7_u16_put(&p,m->phase);s7_u16_put(&p,(uint16_t)m->drift);
    const s7_kit_t *k=&t->kit;
    *p++=k->enabled;memcpy(p,k->density,8);p+=8;memcpy(p,k->role,8);p+=8;
    *p++=k->variation;*p++=k->fill;*p++=k->human;*p++=k->ghost;*p++=k->accent;
    memcpy(p,k->anchors,64);p+=64;*p++=t->mode;*p++=t->kit.source;
}
static int s7_track_decode(s7_track_t *out,const uint8_t in[S7_TRACK_BYTES])
{
    const uint8_t *p=in;s7_track_t t={0};
    t.seed=(uint16_t)s7_u16_get(&p);t.seeded=*p++;
    t.grids.x=*p++;t.grids.y=*p++;t.grids.dens=*p++;t.grids.chaos=*p++;
    t.bounce.speed=*p++;t.bounce.angle=*p++;t.bounce.balls=*p++;
    t.awake.len_a=*p++;t.awake.len_b=*p++;t.awake.prob=*p++;
    memcpy(t.awake.a,p,8);p+=8;memcpy(t.awake.b,p,8);p+=8;
    for(uint32_t i=0;i<4;i++)t.poly.div[i]=*p++;
    for(uint32_t i=0;i<4;i++)t.poly.pitch[i]=*p++;
    t.poly.span=*p++;
    s7_meter_t *m=&t.meter;
    m->enabled=*p++;m->numerator=*p++;m->denominator=*p++;m->group_count=*p++;
    memcpy(m->group,p,8);p+=8;
    m->ratio_n=*p++;m->ratio_d=*p++;m->freeze=*p++;m->reset=*p++;
    m->condition=*p++;m->reference=*p++;
    m->phase=(uint16_t)s7_u16_get(&p);m->drift=(int16_t)s7_u16_get(&p);
    s7_kit_t *k=&t.kit;
    k->enabled=*p++;memcpy(k->density,p,8);p+=8;memcpy(k->role,p,8);p+=8;
    k->variation=*p++;k->fill=*p++;k->human=*p++;k->ghost=*p++;k->accent=*p++;
    memcpy(k->anchors,p,64);p+=64;t.mode=*p++;t.kit.source=*p++;
    while(p<in+S7_TRACK_BYTES)if(*p++)return 0;
    if(!s7_track_valid(&t))return 0;
    if(out)*out=t;
    return 1;
}
static int s7_state_encode(uint8_t *out,uint32_t size,const s7_track_t track[4])
{
    if(!out||size!=S7_STATE_BYTES)return 0;
    for(uint32_t i=0;i<4;i++)if(!s7_track_valid(&track[i]))return 0;
    memcpy(out,"S7G\1",4);
    for(uint32_t i=0;i<4;i++)s7_track_encode(out+4u+i*S7_TRACK_BYTES,&track[i]);
    return 1;
}
static int s7_state_decode(s7_track_t *out,const uint8_t *in,uint32_t size)
{
    if(!in||size!=S7_STATE_BYTES||memcmp(in,"S7G\1",4))return 0;
    for(uint32_t i=0;i<4;i++)if(!s7_track_decode(0,in+4u+i*S7_TRACK_BYTES))return 0;
    if(out)for(uint32_t i=0;i<4;i++)s7_track_decode(out+i,in+4u+i*S7_TRACK_BYTES);
    return 1;
}
