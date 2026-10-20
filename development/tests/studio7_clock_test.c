/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave. */
#include <assert.h>
#include <stdio.h>
#include "studio7_clock.h"
static void rational(uint32_t n,uint32_t d,int32_t drift)
{
    s7_meter_t m=s7_meter_default();m.ratio_n=n;m.ratio_d=d;m.drift=(int16_t)drift;
    s7_clock_t c;uint64_t at;s7_clock_start(&c,&m,0);
    uint64_t den=(uint64_t)4*n*(uint32_t)(10000+drift),num=(uint64_t)S7_QUARTER*d*10000u;
    for(uint32_t i=0;i<1000000;i++){
        uint64_t ideal=(uint64_t)i*num/den;
        assert(c.next==ideal);assert(s7_clock_take(&c,&m,c.next,1,4,&at)&&at==ideal);
    }
}
int main(void)
{
    rational(3,2,0);rational(5,4,0);rational(1,1,100);rational(1,1,-100);
    s7_meter_t a=s7_meter_default(),b=a;a.numerator=a.group_count=3;a.group[3]=0;
    assert(s7_meter_valid(&a));assert(s7_bar_units(&a)*4u==s7_bar_units(&b)*3u);
    a.numerator=7;a.denominator=8;a.group_count=3;a.group[0]=2;a.group[1]=2;a.group[2]=3;
    assert(s7_meter_valid(&a));assert(s7_group_accent(&a,0)==2);
    assert(s7_group_accent(&a,S7_QUARTER)==1);
    assert(s7_group_accent(&a,2*S7_QUARTER)==1);
    assert(!s7_group_accent(&a,S7_QUARTER/2));
    s7_clock_t c;uint64_t at;s7_clock_start(&c,&a,0);
    assert(s7_clock_take(&c,&a,0,1,4,&at));
    assert(s7_clock_realign(&c,&a,0,0)&&c.next==s7_bar_units(&a));
    assert(s7_clock_realign(&c,&a,123,1)&&c.next==s7_bar_units(&a));
    a.phase=2048;s7_clock_start(&c,&a,0);assert(c.next==s7_bar_units(&a)/2);
    a.freeze=1;a.drift=100;s7_clock_start(&c,&a,0);
    assert(s7_clock_take(&c,&a,c.next,1,4,&at));assert(c.next-at==S7_QUARTER/4);
    s7_clock_t old=c;a.ratio_n=0;
    assert(!s7_clock_take(&c,&a,UINT64_MAX,1,4,&at)&&!memcmp(&c,&old,sizeof c));
    puts("studio7: four million exact rational events; polymeter, odd grouping, phase, drift freeze, atomic rejection and duplicate-free realignment passed");
}
