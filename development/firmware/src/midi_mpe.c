/* SPDX-License-Identifier: GPL-3.0-only
 * Live USB lower-zone MPE, allocation and release ownership — By: Inputsignwave.
 * ROUT MPE T1..T4 pins the destination; TRS remains ordinary CH1-4.
 * Bindings live in voices: restarting/stealing invalidates their identity.
 * Audio-side dispatch prepares controls before worker snapshots are rendered.
 */
#include "mpe_controls.h"
static void mpe_legacy_flush(void);
static isw_mpe_controls mpe;
static uint8_t mpe_route;
static uint32_t mpe_resets;
static void mpe_release(voice_t *v)
{
    v->mpe_key=0;
    /* A late note-off must not revive a voice already fading for admission. */
    if(v->gate){v->gate=0;v->stage=v->active?3:0;}
}
static void mpe_forget(uint32_t track)
{
    for(uint32_t i=0;i<NVOICE;i++)if(trk[track].v[i].mpe_ch)mpe_release(&trk[track].v[i]);
}
static void mpe_refresh(track_t *t)
{
    for(uint32_t i=0;i<NVOICE;i++){
        voice_t *v=&t->v[i];
        if(v->active && v->gate && v->mpe_key==1u && v->mpe_ch){
            uint32_t ch=v->mpe_ch;
            v->mpe_bend=isw_mpe_pitch_q8(&mpe,ch);
            v->mpe_pressure=(uint8_t)clamp(mpe.channel[ch].pressure+mpe.channel[0].pressure,0,127);
            v->mpe_slide=(uint8_t)clamp((int32_t)mpe.channel[ch].slide+mpe.channel[0].slide-64,0,127);
        }
    }
}
static void mpe_pedal_up(track_t *t)
{
    for(uint32_t i=0;i<NVOICE;i++)if(t->v[i].mpe_key==2u)mpe_release(&t->v[i]);
}
static void mpe_poll(void)
{
    uint32_t route=(uint32_t)song.g[G_ROUTE];
    if(route<5u || route>8u)route=0;
    if(route!=mpe_route || usb.resets!=mpe_resets || usb.suspended || midi_in_overflow){
        if(mpe_route)mpe_forget(mpe_route-5u);
        if(route && route!=mpe_route)mpe_legacy_flush();
        if(route!=mpe_route || usb.resets!=mpe_resets){isw_mpe_init(&mpe);mpe.members=15;}
        mpe_route=(uint8_t)route;mpe_resets=usb.resets;
        if(usb.suspended || midi_in_overflow)mpe.pedal=0;
    }
    if(mpe_route){
        track_t *t=&trk[mpe_route-5u];
        if(t->engine!=ENGI_WAVE_LAYERS || t->eng_req!=t->engine || t->xf_on || t->p[P_MUTE])mpe_forget(mpe_route-5u);
    }
}
static int mpe_event(uint32_t source,uint32_t st,uint32_t ch,uint32_t a,uint32_t b)
{
    if(source!=1u || !mpe_route)return 0;
    /* USB is dedicated to the zone while selected, including RPN6 disable:
     * no stray member messages may trigger other tracks. */
    track_t *t=&trk[mpe_route-5u];
    if(ch>15u || a>127u || b>127u || usb.suspended)return 1;
    if(t->engine!=ENGI_WAVE_LAYERS || t->eng_req!=t->engine || t->xf_on || t->p[P_MUTE] || mpe_route-5u>=trk_n())return 1;
    if(st==0xb0 && (a==120u || a==123u)){
        for(uint32_t i=0;i<NVOICE;i++){
            voice_t *v=&t->v[i];
            if(!v->mpe_ch || (ch && v->mpe_ch!=ch))continue;
            if(a==120u){mpe_release(v);if(v->active)v->stage=4;}
            else if(v->mpe_key){if(mpe.pedal)v->mpe_key=2;else mpe_release(v);}
        }
        return 1;
    }
    if(st==0xb0 && a==121u){
        uint8_t members=mpe.members;
        if(!ch){isw_mpe_init(&mpe);mpe.members=members;mpe_pedal_up(t);}
        else {isw_mpe_controls defaults;isw_mpe_init(&defaults);mpe.channel[ch]=defaults.channel[ch];}
        mpe_refresh(t);return 1;
    }
    int action=isw_mpe_control(&mpe,st,ch,a,b);
    if(action==ISW_MPE_RECONFIGURE)mpe_forget(mpe_route-5u);
    else if(action==ISW_MPE_PEDAL && !mpe.pedal)mpe_pedal_up(t);
    else if(action==ISW_MPE_EXPRESSION)mpe_refresh(t);
    if((st!=0x90u && st!=0x80u) || !ch || ch>mpe.members)return 1;
    for(uint32_t i=0;i<NVOICE;i++){
        voice_t *v=&t->v[i];
        if(v->mpe_ch!=ch || !v->mpe_key || v->note!=a)continue;
        if(st==0x80u || !b){
            v->mpe_lift=(uint8_t)(st==0x80u?b:0);
            if(mpe.pedal)v->mpe_key=2;else mpe_release(v);
            return 1;
        }
        mpe_release(v);
    }
    if(st==0x90u && b){
        /* Raw independent live notes; chord/ARP and expressive recording are
         * separate work. The existing engine cap/admission budget still apply. */
        mod_note(t,a,b);
        voice_t *v=voice_alloc(t,a,1);v->fine=0;voice_start(t,v,a,b,0);
        if(v->active){v->mpe_ch=(uint8_t)ch;v->mpe_key=1;mpe_refresh(t);v->mpe_current=v->mpe_bend;}
    }
    return 1;
}
