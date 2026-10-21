/* SPDX-License-Identifier: GPL-3.0-only
 * By: Inputsignwave — retired engine compatibility, no synthesis.
 * Patch format/descriptor portions Copyright (C) 2026 Kerem Kilic (Ellic Studio).
 * Preserve opaque legacy project payloads without linking SID or CZ DSP. */
#include "cz_patch.h"
#define CZ_FACTORY_N 0u
static cz_patch_t cz_patch[NTRK] __attribute__((section(".pool")));
static void cz_init(void) { for(uint32_t k=0;k<NTRK;k++)cz_patch_init(cz_patch[k].raw); }
static void cz_factory(uint32_t k,uint8_t *raw) { (void)k;cz_patch_init(raw); }
static void cz_factory_loaded(track_t *t) { (void)t; }
static void cz_track_accept(track_t *t) { (void)t; }
static int (*cz_user_bank_read)(uint32_t,uint32_t,uint8_t *);
static uint16_t cz_user_pick[NTRK];
static cz_patch_t cz_compare;
static uint8_t cz_compare_tr;
static void cz_compare_drop(uint32_t tr) { (void)tr; }
static void cz_compare_take(uint32_t tr) { (void)tr; }
static void cz_sx_byte(uint8_t b) { (void)b; }
static void cz_service(void) {}
static void retired_note_on(track_t *t,voice_t *v) { (void)t;v->active=v->gate=v->stage=0;v->env=0; }
static void retired_render(track_t *t,voice_t *v,int32_t *out,uint32_t n,const vmod_t *m)
{ (void)t;(void)v;(void)out;(void)n;(void)m; }
#define RETIRED_ENGINE(label) { .name=label,.page_title={"UNAVAILABLE","UNAVAILABLE"}, \
.edit={{"-",F_INT,0,127,0,0,0},{"-",F_INT,0,127,0,0,0},{"-",F_INT,0,127,0,0,0}, \
{"-",F_INT,0,127,0,0,0},{"-",F_INT,0,127,0,0,0},{"-",F_INT,0,127,0,0,0}, \
{"-",F_INT,0,127,0,0,0},{"-",F_INT,0,127,0,0,0}}, \
.note_on=retired_note_on,.render=retired_render,.knob={P_E0,P_E1,P_E2,P_E3} }
static const engine_t ENG_CZ=RETIRED_ENGINE("CZ GONE");
static const engine_t ENG_SID=RETIRED_ENGINE("SID GONE");
#undef RETIRED_ENGINE
static const param_desc_t CZ_ED[20] = {
#define CZ_ROW(n) {"A" n, F_TIME, 0,127,0,0,0}, {"D" n, F_TIME,0,127,0,0,0}, \
    {"S" n,F_PCT,0,127,127,0,0}, {"R" n,F_TIME,0,127,0,0,0}, {"LV" n,F_PCT,0,127,127,0,0}
    CZ_ROW("W1"), CZ_ROW("W2"), CZ_ROW("A2"), CZ_ROW("P"),
#undef CZ_ROW
};

