/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess bounded integer math — By: Inputsignwave. */
#pragma once
#include <stdint.h>
/* The target has 32-bit division and no __udivdi3 runtime. Long division of
 * the low word needs at most 32 iterations; common short values use hardware.
 * Kept outlined so the control-rate call graph does not duplicate this loop. */
static __attribute__((noinline)) uint64_t s7_div64(uint64_t n,uint32_t d,uint32_t *remainder)
{
    if(!d){
        if(remainder)*remainder=0;
        return UINT64_MAX;
    }
    uint32_t high=(uint32_t)(n>>32),low=(uint32_t)n;
    if(!high){
        if(remainder)*remainder=low%d;
        return low/d;
    }
    uint32_t qh=high/d,r=high%d,ql=0;
    for(uint32_t i=32;i;i--){
        uint32_t carry=r>>31;r=(r<<1)|((low>>(i-1u))&1u);ql<<=1;
        if(carry||r>=d){r-=d;ql|=1u;}
    }
    if(remainder)*remainder=r;
    return (uint64_t)qh<<32|ql;
}
static uint32_t s7_mod64(uint64_t n,uint32_t d)
{uint32_t r;s7_div64(n,d,&r);return r;}
