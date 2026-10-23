/* SPDX-License-Identifier: MIT
 * Compact plate contract and stability tests — By: Inputsignwave. */
#include "../firmware/src/plate.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
static plate_params neutral(void)
{ plate_params p={.feedback=28000,.damping=16000,.tone=0,.lowcut=90,
 .highcut=28000,.width=32768,.depth=800,.predelay=PLATE_RATE/50,.increment=30000};return p; }
static uint64_t impulse(plate_params p,int mono)
{
    plate_state s;int16_t memory[PLATE_SAMPLES];assert(plate_init(&s,memory,PLATE_SAMPLES));plate_set(&s,&p);
    uint64_t hash=1469598103934665603ull,early=0,late=0;int distinct=0;
    for(int n=0;n<PLATE_RATE*30;n++){
        int32_t l,r;plate_tick(&s,n==PLATE_RATE/10?24000:0,&l,&r);
        if(n<PLATE_RATE/10)assert(!l&&!r);
        if(n<PLATE_RATE*4)early+=(uint64_t)abs(l)+abs(r);
        if(n>PLATE_RATE*26)late+=(uint64_t)abs(l)+abs(r);
        distinct|=l!=r;hash=(hash^(uint32_t)l)*1099511628211ull;
    }
    if(!(early>1000 && late<early/20))fprintf(stderr,"impulse energy early=%llu late=%llu\n",(unsigned long long)early,(unsigned long long)late);
    assert(early>1000 && late<early/20);assert(!s.saturations);
    assert(mono?!distinct:distinct);return hash;
}
static uint64_t tank_energy(const plate_state *s)
{
 uint64_t sum=0;for(uint32_t side=0;side<2;side++){
  uint32_t id=side?9:6;for(uint32_t i=0;i<s->length[id];i++){
   int32_t x=s->memory[s->base[id]+i];sum+=(uint64_t)(x*x);
  }
 }return sum;
}
static void performance(void)
{
 plate_state s;int16_t memory[PLATE_SAMPLES];int32_t l,r;
 assert(plate_init(&s,memory,PLATE_SAMPLES));plate_params p=neutral();p.size=20000;p.wobble=25000;plate_set(&s,&p);
 for(uint32_t n=0;n<PLATE_RATE*2u;n++)plate_tick(&s,n<PLATE_RATE/5u?((n&16u)?5000:-5000):0,&l,&r);
 p.hold=1;plate_set(&s,&p);
 for(uint32_t n=0;n<PLATE_RATE;n++)plate_tick(&s,0,&l,&r);
 assert(s.current.hold==32768);uint64_t energy=tank_energy(&s);assert(energy>1000);
 for(uint32_t n=0;n<PLATE_RATE*30u;n++){
  plate_tick(&s,n&1?30000:-30000,&l,&r); /* closed input under Hold */
  if(n%PLATE_RATE==0)assert(tank_energy(&s)==energy);
 }
 assert(tank_energy(&s)==energy);
 p.hold=0;p.wobble=0;p.feedback=18000;plate_set(&s,&p);
 for(uint32_t n=0;n<PLATE_RATE*20u;n++)plate_tick(&s,0,&l,&r);
 assert(s.current.hold==0&&tank_energy(&s)<energy/10u);
 p.swell_samples=PLATE_RATE*2;plate_set(&s,&p);plate_trigger(&s);
 for(uint32_t n=0;n<256;n++)plate_tick(&s,0,&l,&r);
 assert(s.swell_gain==0);
 for(uint32_t n=0;n<PLATE_RATE*2u;n++)plate_tick(&s,0,&l,&r);
 assert(s.swell_gain==32768);
 puts("plate: size/wobble, closed-input exact-energy 30s Hold, release decay and wet Swell passed");
}
int main(void)
{
    performance();
    plate_state s;int16_t memory[PLATE_SAMPLES];int32_t l,r;
    assert(!plate_init(&s,NULL,0));plate_tick(&s,12345,&l,&r);assert(!l&&!r);
    assert(!plate_init(&s,memory,PLATE_SAMPLES-1));
    assert(plate_init(&s,memory,PLATE_SAMPLES));plate_params p=neutral();plate_set(&s,&p);
    for(int i=0;i<PLATE_RATE;i++){plate_tick(&s,0,&l,&r);assert(!l&&!r);}
    uint64_t h=impulse(p,0);assert(h==impulse(p,0));
    p.tone=-16384;assert(h!=impulse(p,0));p=neutral();p.lowcut=7000;assert(h!=impulse(p,0));
    p=neutral();p.highcut=2000;assert(h!=impulse(p,0));p=neutral();p.width=0;impulse(p,1);
    assert(plate_init(&s,memory,PLATE_SAMPLES));
    uint32_t rng=1;int32_t peak=0;
    for(int n=0;n<PLATE_RATE*90;n++){
        if(n%257==0){p=neutral();p.feedback=n&1?INT_MAX:INT_MIN;p.depth=INT_MAX;
            p.tone=n&512?INT_MIN:INT_MAX;p.lowcut=n&1024?INT_MAX:INT_MIN;
            p.highcut=n&2048?INT_MAX:INT_MIN;p.predelay=n&4096?INT_MAX:INT_MIN;
            p.increment=UINT_MAX;plate_set(&s,&p);}
        rng=rng*1664525u+1013904223u;
        plate_tick(&s,(int16_t)(rng>>16),&l,&r);
        assert(l>=-32768&&l<=32767&&r>=-32768&&r<=32767);
        if(abs(l)>peak)peak=abs(l);
    }
    /* Reinitialisation must remove the old tail, LFO and filter histories. */
    assert(plate_init(&s,memory,PLATE_SAMPLES));plate_set(&s,&p);
    for(int n=0;n<PLATE_RATE;n++){plate_tick(&s,0,&l,&r);assert(!l&&!r);}
    printf("PASS rate=%d delay=%zu state=%zu bounded_stress_peak=%d\n",PLATE_RATE,sizeof memory,sizeof s,peak);
}
