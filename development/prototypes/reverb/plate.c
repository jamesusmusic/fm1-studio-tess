// Copyright 2014 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
/* SPDX-License-Identifier: MIT
 * Fixed-point C adaptation and additional controls — By: Inputsignwave.
 * Derived topology: Rings reverb.h at
 * f4cfa7697095a28d292bff758beb51deb49b5a86 (Emilie Gillet).
 * Four input allpasses; two branches with two allpasses + long delay each.
 * This is a new rate-scaled implementation, not the unmodified Rings DSP.
 */
#include "plate.h"
#include <string.h>
static int32_t bound(int32_t x,int32_t lo,int32_t hi)
{ return x<lo?lo:x>hi?hi:x; }
/* Truncate toward zero: avoids persistent -1 LSB decay limit cycles.
 * Widen products to cover worst-case allpass transients. Target cost pending. */
static int32_t mul(int32_t x,int32_t k)
{ int64_t p=(int64_t)x*k;return (int32_t)((p+(p<0?32767:0))>>15); }
static int32_t lp(int32_t x,int32_t *z,int32_t k)
{ *z+=mul(x*4096-*z,k);return *z/4096; }
static int32_t read_delay(plate_state *s,int n,int32_t delay_q8)
{
    int32_t delay=delay_q8>>8,frac=delay_q8&255;
    int32_t a=(int32_t)s->pos[n]-delay;
    if(a<0)a+=s->length[n];
    int32_t b=a?a-1:s->length[n]-1;
    int32_t x=s->memory[s->base[n]+a],y=s->memory[s->base[n]+b];
    return x+((y-x)*frac)/256;
}
static void write_delay(plate_state *s,int n,int32_t x)
{
    if(x < -32768 || x>32767)s->saturations++;
    s->memory[s->base[n]+s->pos[n]]=(int16_t)bound(x,-32768,32767);
    if(++s->pos[n]>=s->length[n])s->pos[n]=0;
}
static int32_t ap(plate_state *s,int n,int32_t x,int32_t gain)
{
    int32_t d=read_delay(s,n,s->length[n]*256);
    int32_t y=d-mul(x,gain);
    write_delay(s,n,x+mul(y,gain));return y;
}
int plate_init(plate_state *s,int16_t *memory,size_t count)
{
    static const uint16_t lens[11]={PLATE_SCALE(150),PLATE_SCALE(214),PLATE_SCALE(319),PLATE_SCALE(527),PLATE_SCALE(2182),PLATE_SCALE(2690),PLATE_SCALE(4501),PLATE_SCALE(2525),PLATE_SCALE(2197),PLATE_SCALE(6312),PLATE_PRE};
    memset(s,0,sizeof *s);
    if(!memory || count<PLATE_SAMPLES)return 0;
    s->memory=memory;memset(memory,0,PLATE_SAMPLES*sizeof *memory);
    unsigned at=0;
    for(int n=0;n<11;n++){s->base[n]=(uint16_t)at;s->length[n]=lens[n];at+=lens[n];}
    s->phase[1]=0x63400000u;s->ready=1;return 1;
}
void plate_set(plate_state *s,const plate_params *p)
{
    if(!p)return;
    s->target=*p;
    s->target.feedback=bound(p->feedback,0,31129); /* <0.95; no freeze in v1 */
    s->target.damping=bound(p->damping,128,30000);
    s->target.tone=bound(p->tone,-16384,16384);
    s->target.lowcut=bound(p->lowcut,1,14000);
    s->target.highcut=bound(p->highcut,256,32700);
    s->target.width=bound(p->width,0,32768);
    s->target.depth=bound(p->depth,0,PLATE_SCALE(24)*256);
    s->target.predelay=bound(p->predelay,0,PLATE_PRE-1);
    s->target.increment=p->increment>0x100000u?0x100000u:p->increment;
    /* Predelay is an integer: slewed by its crossfaded read below. */
}
static int32_t slew(int32_t *x,int32_t target)
{ int32_t d=target-*x;if(d)*x+=d/256+(d>0 && d<256)-(d<0 && d>-256);return *x; }
static int32_t tri(uint32_t p)
{ uint32_t v=p>>16;return v<32768?(int32_t)v*2-32768:98303-(int32_t)v*2; }
void plate_tick(plate_state *s,int32_t input,int32_t *left,int32_t *right)
{
    *left=*right=0;if(!s->ready)return;
    plate_params *c=&s->current,*t=&s->target;
    slew(&c->feedback,t->feedback);slew(&c->damping,t->damping);
    slew(&c->tone,t->tone);slew(&c->lowcut,t->lowcut);
    slew(&c->highcut,t->highcut);slew(&c->width,t->width);slew(&c->depth,t->depth);
    c->increment=t->increment;
    /* Use Q8 predelay position for continuous changes instead of tap jumps. */
    slew(&c->predelay,t->predelay*256);
    input=bound(input,-32768,32767)/4; /* explicit send headroom */
    int32_t x=c->predelay<256?input:read_delay(s,10,c->predelay);
    write_delay(s,10,input);
    for(int n=0;n<4;n++)x=ap(s,n,x,20480);
    int32_t wet[2];
    /* Read both branches before writing either: independent of evaluation order. */
    int32_t tail[2];
    for(int n=0;n<2;n++){
        int id=n?9:6;
        s->phase[n]+=n?c->increment*3u/5u:c->increment;
        int32_t mod=mul(tri(s->phase[n]),c->depth);
        int32_t tap=(s->length[id]-PLATE_SCALE(25))*256+mod;
        tail[n]=read_delay(s,id,tap);
    }
    for(int n=0;n<2;n++){
        int32_t y=x+lp(mul(tail[1-n],c->feedback),&s->damp[n],c->damping);
        /* Tiny fixed DC rejection in the feedback path; separate from wet cuts. */
        y-=lp(y,&s->dc[n],32);
        y=ap(s,n?7:4,y,n?20480:-20480);
        y=ap(s,n?8:5,y,n?-20480:20480);
        write_delay(s,n?9:6,y);
        /* Wet TONE is a bipolar shelf around a fixed pivot, not tail damping. */
        int32_t low=lp(y,&s->shelf[n],PLATE_RATE==22050?7200:12800);
        y+=mul(y-2*low,c->tone);
        y-=lp(y,&s->bass[n],c->lowcut);
        wet[n]=lp(y,&s->treble[n],c->highcut);
    }
    int32_t mid=(wet[0]+wet[1])/2,side=mul((wet[0]-wet[1])/2,c->width);
    *left=bound((mid+side)*2,-32768,32767);
    *right=bound((mid-side)*2,-32768,32767);
}
