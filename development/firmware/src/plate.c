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
#ifndef MELODEE_FREESTANDING
#include <string.h>
#endif
static int32_t plate_bound(int32_t x,int32_t lo,int32_t hi)
{ return x<lo?lo:x>hi?hi:x; }
/* Truncate toward zero: avoids persistent -1 LSB decay limit cycles.
 * Widen products to cover worst-case allpass transients. Target cost pending. */
static int32_t plate_mul(int32_t x,int32_t k)
{ int64_t p=(int64_t)x*k;return (int32_t)((p+(p<0?32767:0))>>15); }
static int32_t plate_lp(int32_t x,int32_t *z,int32_t k)
{ *z+=plate_mul(x*4096-*z,k);return *z/4096; }
static int32_t plate_read_delay(plate_state *s,int n,int32_t delay_q8)
{
    int32_t delay=delay_q8>>8,frac=delay_q8&255;
    int32_t a=(int32_t)s->pos[n]-delay;
    if(a<0)a+=s->length[n];
    int32_t b=a?a-1:s->length[n]-1;
    int32_t x=s->memory[s->base[n]+a],y=s->memory[s->base[n]+b];
    return x+((y-x)*frac)/256;
}
static void plate_write_delay(plate_state *s,int n,int32_t x)
{
    if(x < -32768 || x>32767)s->saturations++;
    s->memory[s->base[n]+s->pos[n]]=(int16_t)plate_bound(x,-32768,32767);
    if(++s->pos[n]>=s->length[n])s->pos[n]=0;
}
static int32_t plate_ap(plate_state *s,int n,int32_t x,int32_t gain)
{
    int32_t span=(s->length[n]*s->current.size)>>7;
    int32_t d=plate_read_delay(s,n,span<256?256:span);
    int32_t y=d-plate_mul(x,gain);
    plate_write_delay(s,n,x+plate_mul(y,gain));return y;
}
int plate_init(plate_state *s,int16_t *memory,size_t count)
{
    static const uint16_t lens[11]={PLATE_SCALE(150),PLATE_SCALE(214),PLATE_SCALE(319),PLATE_SCALE(527),PLATE_SCALE(2182),PLATE_SCALE(2690),PLATE_SCALE(4501),PLATE_SCALE(2525),PLATE_SCALE(2197),PLATE_SCALE(6312),PLATE_PRE};
    memset(s,0,sizeof *s);
    if(!memory || count<PLATE_SAMPLES)return 0;
    s->memory=memory;memset(memory,0,PLATE_SAMPLES*sizeof *memory);
    unsigned at=0;
    for(int n=0;n<11;n++){s->base[n]=(uint16_t)at;s->length[n]=lens[n];at+=lens[n];}
    s->phase[1]=0x63400000u;s->current.size=32768;s->swell_gain=32768;s->silent=PLATE_RATE;s->ready=1;return 1;
}
void plate_set(plate_state *s,const plate_params *p)
{
    if(!p)return;
    s->target=*p;
    s->target.feedback=plate_bound(p->feedback,0,31129); /* Normal decay; Hold uses a separate lossless closed-input loop. */
    s->target.damping=plate_bound(p->damping,128,30000);
    s->target.tone=plate_bound(p->tone,-16384,16384);
    s->target.lowcut=plate_bound(p->lowcut,1,14000);
    s->target.highcut=plate_bound(p->highcut,256,32700);
    s->target.width=plate_bound(p->width,0,32768);
    s->target.depth=plate_bound(p->depth,0,PLATE_SCALE(24)*256);
    s->target.predelay=plate_bound(p->predelay,0,PLATE_PRE-1);
    s->target.increment=p->increment>0x100000u?0x100000u:p->increment;
    s->target.size=p->size?plate_bound(p->size,8192,32768):32768;
    s->target.wobble=plate_bound(p->wobble,0,32768);
    s->target.hold=p->hold?32768:0;
    s->target.swell_samples=plate_bound(p->swell_samples,0,PLATE_RATE*20);
    /* Predelay is an integer: slewed by its crossfaded read below. */
}
static int32_t plate_slew(int32_t *x,int32_t target)
{ int32_t d=target-*x;if(d)*x+=d/256+(d>0 && d<256)-(d<0 && d>-256);return *x; }
static int32_t plate_tri(uint32_t p)
{ uint32_t v=p>>16;return v<32768?(int32_t)v*2-32768:98303-(int32_t)v*2; }
void plate_trigger(plate_state *s)
{ if(s->target.swell_samples)s->swell_stage=1; }
void plate_tick(plate_state *s,int32_t input,int32_t *left,int32_t *right)
{
    *left=*right=0;if(!s->ready)return;
    plate_params *c=&s->current,*t=&s->target;
    plate_slew(&c->feedback,t->feedback);plate_slew(&c->damping,t->damping);
    plate_slew(&c->tone,t->tone);plate_slew(&c->lowcut,t->lowcut);
    plate_slew(&c->highcut,t->highcut);plate_slew(&c->width,t->width);plate_slew(&c->depth,t->depth);
    plate_slew(&c->size,t->size);plate_slew(&c->wobble,t->wobble);plate_slew(&c->hold,t->hold);
    c->swell_samples=t->swell_samples;
    c->increment=t->increment;
    if(input>128 || input< -128){if(s->silent>PLATE_RATE/50u)plate_trigger(s);s->silent=0;}
    else if(s->silent<PLATE_RATE)s->silent++;
    if(!c->swell_samples){s->swell_gain=32768;s->swell_stage=0;}
    else if(s->swell_stage==1){s->swell_gain-=128;if(s->swell_gain<=0){s->swell_gain=0;s->swell_stage=2;}}
    else if(s->swell_stage==2){s->swell_gain+=32768/c->swell_samples+1;if(s->swell_gain>=32768){s->swell_gain=32768;s->swell_stage=0;}}
    s->wow+=116870u*(22050u/PLATE_RATE);s->flutter+=10130777u*(22050u/PLATE_RATE);
    /* Use Q8 predelay position for continuous changes instead of tap jumps. */
    plate_slew(&c->predelay,t->predelay*256);
    input=plate_mul(plate_bound(input,-32768,32767)/4,32768-c->hold); /* explicit send headroom */
    int32_t x=c->predelay<256?input:plate_read_delay(s,10,c->predelay);
    plate_write_delay(s,10,input);
    for(int n=0;n<4;n++)x=plate_ap(s,n,x,20480);
    int32_t wet[2];
    /* Read both branches before writing either: independent of evaluation order. */
    int32_t tail[2];
    for(int n=0;n<2;n++){
        int id=n?9:6;
        s->phase[n]+=n?c->increment*3u/5u:c->increment;
        int32_t mod=plate_mul(plate_tri(s->phase[n]),c->depth);
        int32_t wobble=plate_mul(plate_tri(s->wow+n*0x41000000u)+plate_tri(s->flutter+n*0x12300000u)/5,c->wobble);
        mod+=wobble*PLATE_SCALE(12)/128;
        int32_t span=((s->length[id]-PLATE_SCALE(25))*c->size)>>7;
        int32_t tap=plate_bound(span+mod,256,(s->length[id]-1)*256);
        tail[n]=plate_read_delay(s,id,tap);
    }
    int32_t frozen[2]={plate_read_delay(s,9,s->length[9]*256),plate_read_delay(s,6,s->length[6]*256)};
    for(int n=0;n<2;n++){
        int32_t feedback=32768-plate_mul(32768-c->feedback,c->size);
        int32_t y=x+plate_lp(plate_mul(tail[1-n],feedback),&s->damp[n],c->damping);
        /* Tiny fixed DC rejection in the feedback path; separate from wet cuts. */
        y-=plate_lp(y,&s->dc[n],32);
        y=plate_ap(s,n?7:4,y,n?20480:-20480);
        y=plate_ap(s,n?8:5,y,n?-20480:20480);
        /* At full Hold this is an exact integer permutation of the two
         * delay rings: input is closed and no lossy interpolation/filter
         * operates inside the loop. Tail energy cannot decay or grow.
         * Normal and frozen paths crossfade while entering/leaving Hold. */
        y+=plate_mul(frozen[n]-y,c->hold);
        plate_write_delay(s,n?9:6,y);
        /* Wet TONE is a bipolar shelf around a fixed pivot, not tail damping. */
        int32_t low=plate_lp(y,&s->shelf[n],PLATE_RATE==22050?7200:12800);
        y+=plate_mul(y-2*low,c->tone);
        y-=plate_lp(y,&s->bass[n],c->lowcut);
        wet[n]=plate_lp(y,&s->treble[n],c->highcut);
    }
    int32_t mid=(wet[0]+wet[1])/2,side=plate_mul((wet[0]-wet[1])/2,c->width);
    *left=plate_bound(plate_mul((mid+side)*2,s->swell_gain),-32768,32767);
    *right=plate_bound(plate_mul((mid-side)*2,s->swell_gain),-32768,32767);
}
