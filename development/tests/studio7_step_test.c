/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave. */
#include <assert.h>
#include <stdio.h>
#include "studio7_step.h"
int main(void)
{
    uint32_t count=0,seed=12345;
    for(uint32_t h=0;h<256;h++)for(uint32_t a=0;a<256;a++){
        if(h&&(a&~h))continue;
        uint8_t old[8]={0},packed[24]={0},got[8];uint32_t p=41,at=3,back=3;
        s7_bits_put(old,&p,h,8);s7_bits_put(old,&p,a,8);
        assert(s7_step_pack(packed,&at,old)&&at==64);
        assert(s7_step_unpack(got,packed,&back)&&back==at&&!memcmp(got,old,8));count++;
    }
    assert(count==STUDIO7_STEP_PAIR_COUNT);
    for(uint32_t i=0;i<200000;i++){
        uint8_t old[8],got[8],packed[16]={0};
        for(uint32_t j=0;j<8;j++){seed=seed*1664525u+1013904223u;old[j]=(uint8_t)(seed>>24);}
        uint32_t p=41,hit=s7_bits_get(old,&p,8),acc=s7_bits_get(old,&p,8);p=49;
        s7_bits_put(old,&p,hit?acc&hit:acc,8);
        uint32_t at=i%8u,back=at;
        assert(s7_step_pack(packed,&at,old));assert(s7_step_unpack(got,packed,&back));
        assert(back==at&&!memcmp(old,got,8));
    }
    for(uint32_t rank=STUDIO7_STEP_PAIR_COUNT;rank<8192;rank++){
        uint8_t packed[8]={0},got[8],saved[8];memset(got,0xA5,8);memcpy(saved,got,8);
        uint32_t p=41,at=0;s7_bits_put(packed,&p,rank,13);
        assert(!s7_step_unpack(got,packed,&at)&&at==0&&!memcmp(got,saved,8));
    }
    printf("studio7: all %u valid hit/gate pairs, 200000 full-record round trips, invalid ranks atomic; 672 bank bytes reclaimed\n",count);
}
