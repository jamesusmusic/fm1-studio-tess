/* SPDX-License-Identifier: GPL-3.0-only
 * Transport-independent lower-zone controller prototype — By: Inputsignwave.
 * Not included in production firmware. Voice ownership/rendering are separate.
 * A caller owns one context per MIDI source; RECONFIGURE requires releasing
 * that source's voice bindings before applying further performance messages. */
#ifndef INPUTSIGNWAVE_MPE_CONTROLS_H
#define INPUTSIGNWAVE_MPE_CONTROLS_H
#include <stdint.h>
#include <string.h>
typedef struct {
    int16_t bend;
    uint8_t pressure, slide, semis, cents, rpn_hi, rpn_lo;
} isw_mpe_channel;
typedef struct {
    isw_mpe_channel channel[16];
    uint8_t members, pedal;
} isw_mpe_controls;
enum { ISW_MPE_IGNORED, ISW_MPE_EXPRESSION, ISW_MPE_RECONFIGURE, ISW_MPE_PEDAL };
static void isw_mpe_init(isw_mpe_controls *s)
{
    memset(s,0,sizeof *s);
    for(uint32_t ch=0;ch<16;ch++){
        s->channel[ch].slide=64;
        s->channel[ch].semis=ch?48:2;
        s->channel[ch].rpn_hi=s->channel[ch].rpn_lo=127;
    }
}
/* Complete channel messages, with status class separated from channel.
 * Notes are intentionally left to the future channel+voice ownership layer. */
static int isw_mpe_control(isw_mpe_controls *s,uint32_t status,uint32_t ch,uint32_t a,uint32_t b)
{
    if(ch>=16 || a>127 || b>127 || (status!=0xb0 && status!=0xd0 && status!=0xe0))return ISW_MPE_IGNORED;
    isw_mpe_channel *c=&s->channel[ch];
    if(status==0xb0){
        /* Master RPN selection must work before the zone has been enabled. */
        if(ch==0 || (s->members && ch<=s->members)){
            if(a==101){c->rpn_hi=(uint8_t)b;return ISW_MPE_IGNORED;}
            if(a==100){c->rpn_lo=(uint8_t)b;return ISW_MPE_IGNORED;}
            if(a==99 || a==98){c->rpn_hi=c->rpn_lo=127;return ISW_MPE_IGNORED;}
            if(ch==0 && a==6 && !c->rpn_hi && c->rpn_lo==6){
                if(b>15)return ISW_MPE_IGNORED;
                isw_mpe_init(s);s->members=(uint8_t)b;return ISW_MPE_RECONFIGURE;
            }
        }
    }
    if(!s->members || ch>s->members)return ISW_MPE_IGNORED;
    if(status==0xe0){c->bend=(int16_t)((int32_t)(a|(b<<7))-8192);return ISW_MPE_EXPRESSION;}
    if(status==0xd0){c->pressure=(uint8_t)a;return ISW_MPE_EXPRESSION;}
    if(a==74){c->slide=(uint8_t)b;return ISW_MPE_EXPRESSION;}
    if(a==64 && ch==0){s->pedal=(uint8_t)(b>=64);return ISW_MPE_PEDAL;}
    if(!c->rpn_hi && !c->rpn_lo && (a==6 || a==38)){
        if(a==6)c->semis=(uint8_t)(b>96?96:b);
        else c->cents=(uint8_t)(b>99?99:b);
        return ISW_MPE_EXPRESSION;
    }
    return ISW_MPE_IGNORED;
}
static int32_t isw_mpe_bend_q8(const isw_mpe_channel *c)
{
    int32_t range=((int32_t)c->semis*100+c->cents)*256/100;
    return c->bend*range/(c->bend<0?8192:8191);
}
static int32_t isw_mpe_pitch_q8(const isw_mpe_controls *s,uint32_t ch)
{
    if(!s->members || ch>s->members)return 0;
    return isw_mpe_bend_q8(&s->channel[0])+(ch?isw_mpe_bend_q8(&s->channel[ch]):0);
}
#endif
