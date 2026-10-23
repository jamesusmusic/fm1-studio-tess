/* SPDX-License-Identifier: GPL-3.0-only
 * Gesture separation and malformed-stream checks — By: Inputsignwave. */
#include "mpe_controls.h"
#include <assert.h>
#include <stdio.h>
static void rpn(isw_mpe_controls *s,uint32_t ch,uint32_t number)
{
    isw_mpe_control(s,0xb0,ch,101,0);isw_mpe_control(s,0xb0,ch,100,number);
}
int main(void)
{
    isw_mpe_controls usb,trs;isw_mpe_init(&usb);isw_mpe_init(&trs);
    assert(!isw_mpe_control(&usb,0xe0,1,127,127));
    rpn(&usb,0,6);assert(isw_mpe_control(&usb,0xb0,0,6,4)==ISW_MPE_RECONFIGURE);
    assert(usb.members==4 && !trs.members);
    isw_mpe_control(&usb,0xe0,1,127,127);isw_mpe_control(&usb,0xe0,2,0,0);
    isw_mpe_control(&usb,0xe0,0,127,127);
    assert(isw_mpe_pitch_q8(&usb,1)==50*256 && isw_mpe_pitch_q8(&usb,2)==-46*256);
    isw_mpe_control(&usb,0xd0,1,100,0);isw_mpe_control(&usb,0xd0,2,30,0);
    isw_mpe_control(&usb,0xb0,1,74,20);isw_mpe_control(&usb,0xb0,2,74,110);
    assert(usb.channel[1].pressure==100 && usb.channel[2].pressure==30);
    assert(usb.channel[1].slide==20 && usb.channel[2].slide==110);
    assert(trs.channel[1].pressure==0 && !trs.channel[1].bend);
    assert(!isw_mpe_control(&usb,0xb0,1,64,127));
    assert(isw_mpe_control(&usb,0xb0,0,64,127)==ISW_MPE_PEDAL && usb.pedal);
    rpn(&usb,1,0);isw_mpe_control(&usb,0xb0,1,6,96);assert(isw_mpe_pitch_q8(&usb,1)==98*256);
    isw_mpe_control(&usb,0xb0,1,99,0);isw_mpe_control(&usb,0xb0,1,6,12);
    assert(usb.channel[1].semis==96); /* NRPN cannot accidentally edit pitch range */
    isw_mpe_controls saved=usb;
    assert(!isw_mpe_control(&usb,0xe0,16,0,0) && !memcmp(&saved,&usb,sizeof usb));
    assert(!isw_mpe_control(&usb,0xb0,0,74,128) && !memcmp(&saved,&usb,sizeof usb));
    uint32_t seed=0x19780814;
    for(uint32_t i=0;i<100000;i++){
        seed=seed*1664525u+1013904223u;
        isw_mpe_control(&usb,(seed>>24)&0xf0,(seed>>16)&31,(seed>>8)&255,seed&255);
        assert(usb.members<=15 && usb.pedal<=1);
        for(uint32_t c=0;c<16;c++){
            assert(usb.channel[c].semis<=96 && usb.channel[c].cents<=99);
            assert(usb.channel[c].pressure<=127 && usb.channel[c].slide<=127);
            assert(usb.channel[c].bend>=-8192 && usb.channel[c].bend<=8191);
        }
    }
    rpn(&usb,0,6);assert(isw_mpe_control(&usb,0xb0,0,6,0)==ISW_MPE_RECONFIGURE);
    assert(!usb.members && !usb.pedal && !isw_mpe_pitch_q8(&usb,1));
    printf("MPE prototype: independent member bend/pressure/slide, master bend/pedal, RPN 0/6, source isolation and 100000 malformed/mixed messages passed; context=%zu B\n",sizeof usb);
    return 0;
}
