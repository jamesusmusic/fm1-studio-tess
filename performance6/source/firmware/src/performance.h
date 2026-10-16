/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio) */
/* Studio performance controls. Defaults are an exact bypass. */
#pragma once
enum { DUR_STEP, DUR_FIXED, DUR_HIT };
typedef struct {
    uint8_t mode, length, fade, overlap, amp, attack, decay, sustain, release;
    uint8_t brightness, contour, amount, tone_decay;
} performance_t;
#define PERFORMANCE_DEFAULT {0,2,0,1,0,0,40,127,40,64,64,64,40}
static performance_t performance[NTRK]={PERFORMANCE_DEFAULT,PERFORMANCE_DEFAULT,PERFORMANCE_DEFAULT,PERFORMANCE_DEFAULT};
static performance_t performance_default(void)
{
    return (performance_t)PERFORMANCE_DEFAULT;
}
/* One byte is a time curve, not a DX7 rate: larger = longer. */
static uint32_t performance_ms(uint32_t value)
{ return value ? 2u + value * value : 0u; }
typedef struct {
    int32_t amp, fade, tone;
    uint32_t fade_step;
    uint8_t stage, down;
} performance_voice_t;
static performance_voice_t performance_voice[NTRK][NVOICE] __attribute__((section(".pool")));
static void performance_start(track_t *t,voice_t *v);
static int32_t performance_move(int32_t from,int32_t to,uint32_t ms);
static int32_t performance_gain(track_t *t,voice_t *v,int fm);
static void performance_sound_reset(uint32_t tr)
{
    performance_t d=performance_default();
    performance[tr].fade=d.fade;
    memcpy(&performance[tr].amp,&d.amp,9u);
}
static void performance_sound_pack(uint8_t *b,uint32_t tr)
{
    b[0]='S';b[1]='P';b[2]=1;b[3]=performance[tr].fade;
    memcpy(b+4,&performance[tr].amp,9u);
}
static int performance_sound_valid(const uint8_t *b)
{
    if(b[0]!='S'||b[1]!='P'||b[2]!=1)return 1; /* old native payload */
    if(b[3]>127||b[4]>1)return 0;
    for(uint32_t k=5;k<13;k++)if(b[k]>127)return 0;
    return 1;
}
static void performance_sound_apply(const uint8_t *b,uint32_t tr)
{
    performance_sound_reset(tr);
    if(b[0]=='S'&&b[1]=='P'&&b[2]==1){performance[tr].fade=b[3];memcpy(&performance[tr].amp,b+4,9u);}
}
