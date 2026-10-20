/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess math verification — By: Inputsignwave. */
#include <assert.h>
#include <stdio.h>
#include "studio7_math.h"
int main(void)
{
    uint64_t random=0xA53F726198ABCDEFu;
    for(uint32_t i=0;i<1000000;i++){
        random^=random<<13;random^=random>>7;random^=random<<17;
        uint64_t n=random;uint32_t d=(uint32_t)(random>>23)|1u,r;
        assert(s7_div64(n,d,&r)==n/d&&r==n%d);
    }
    const uint32_t ds[]={1,2,3,8,10000,646400,0x80000000u,UINT32_MAX};
    for(uint32_t i=0;i<sizeof ds/sizeof *ds;i++)for(uint32_t j=0;j<64;j++){
        uint64_t n=UINT64_MAX>>j;uint32_t r;assert(s7_div64(n,ds[i],&r)==n/ds[i]&&r==n%ds[i]);
    }
    puts("studio7: million full-width division/remainder comparisons and boundary divisors passed, bounded 32 iterations");
}
