/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess additions — By: Inputsignwave. */
#include <assert.h>
#include <stdio.h>
#include "studio7_state.h"
int main(void)
{
    s7_track_t a[4],b[4],saved[4];uint8_t wire[S7_STATE_BYTES],again[S7_STATE_BYTES];
    for(uint32_t i=0;i<4;i++){
        a[i]=s7_track_default();a[i].seed=i+1;a[i].awake.a[7]=0x88;
        a[i].awake.b[6]=0x77;a[i].poly.pitch[3]=20;a[i].meter.drift=(int16_t)(i*50-100);
        a[i].kit.anchors[63]=(uint8_t)(1u<<i);a[i].meter.phase=(uint16_t)(i*1024);
    }
    assert(s7_state_encode(wire,sizeof wire,a));
    assert(s7_state_decode(b,wire,sizeof wire));assert(s7_state_encode(again,sizeof again,b));
    assert(!memcmp(wire,again,sizeof wire));
    memset(b,0xA5,sizeof b);memcpy(saved,b,sizeof b);
    for(uint32_t n=0;n<S7_STATE_BYTES;n++){
        assert(!s7_state_decode(b,wire,n)&&!memcmp(b,saved,sizeof b));
    }
    uint32_t rejected=0;
    for(uint32_t i=0;i<sizeof wire;i++)for(uint32_t bit=0;bit<8;bit++){
        wire[i]^=(uint8_t)(1u<<bit);
        if(!s7_state_decode(b,wire,sizeof wire)){assert(!memcmp(b,saved,sizeof b));rejected++;}
        else {assert(s7_state_encode(again,sizeof again,b));assert(!memcmp(wire,again,sizeof wire));memcpy(b,saved,sizeof b);}
        wire[i]^=(uint8_t)(1u<<bit);
    }
    assert(rejected>1000);a[3].seed=0;memset(again,0xAA,sizeof again);uint8_t pristine[sizeof again];memcpy(pristine,again,sizeof again);
    assert(!s7_state_encode(again,sizeof again,a)&&!memcmp(again,pristine,sizeof again));
    printf("studio7: %u-byte complete extension, all truncations and %u rejected bit mutations atomic; AWAKE, inactive modes, anchors and signed drift round-trip\n",S7_STATE_BYTES,rejected);
}
