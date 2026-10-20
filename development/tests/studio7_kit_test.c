/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave. */
#include <assert.h>
#include <stdio.h>
#include "studio7_kit.h"
int main(void)
{
    s7_kit_t k=s7_kit_default(),saved;k.enabled=1;k.variation=80;k.human=100;k.fill=40;
    for(uint32_t s=0;s<64;s++)k.anchors[s]=(uint8_t)(s%3?1:8);
    saved=k;
    for(uint32_t seed=1;seed<=128;seed++)for(uint32_t bar=0;bar<16;bar++)for(uint32_t s=0;s<64;s++){
        s7_kit_out_t a,b;
        assert(s7_kit_generate(&a,&k,seed,s,bar,9,1,96,s%4==0,s==63));
        assert(s7_kit_generate(&b,&k,seed,s,bar,9,1,96,s%4==0,s==63));
        assert(!memcmp(&a,&b,sizeof a));assert((a.hit&k.anchors[s])==k.anchors[s]);
        assert((a.accent&k.anchors[s])==(1u&k.anchors[s]));
        assert(!(a.accent&~a.hit));assert((a.hit&12)!=12);
        for(uint32_t l=0;l<8;l++){
            if(k.anchors[s]&(1u<<l))assert(a.velocity[l]==96&&!a.offset[l]);
            if(a.hit&(1u<<l))assert(a.velocity[l]&&a.velocity[l]<=127);
        }
    }
    assert(!memcmp(&k,&saved,sizeof k));
    k.enabled=0;s7_kit_out_t a;
    assert(s7_kit_generate(&a,&k,1,0,10,255,85,70,2,1));
    assert(a.hit==255&&a.accent==85);
    for(uint32_t i=0;i<8;i++)assert(a.velocity[i]==70&&!a.offset[i]);
    k.enabled=1;k.variation=k.fill=0;k.human=0;
    for(uint32_t i=0;i<8;i++)k.density[i]=0;
    memset(k.anchors,0,sizeof k.anchors);
    assert(s7_kit_generate(&a,&k,1,1,0,0,0,90,0,0)&&!a.hit);
    assert(s7_kit_generate(&a,&k,1,1,0,0,0,90,2,1)&&!a.hit);
    k.anchors[1]=1;
    assert(s7_kit_generate(&a,&k,1,1,0,0,0,90,0,0)&&!a.hit); /* deleting a base hit never resurrects it */
    s7_kit_out_t old=a;k.role[0]=k.role[1];
    assert(!s7_kit_generate(&a,&k,1,0,0,0,0,90,0,0)&&!memcmp(&a,&old,sizeof a));
    puts("studio7: 131072 repeatable eight-lane patterns; anchors/velocities, hat exclusion, bypass identity, base purity and deletion safety passed");
}
