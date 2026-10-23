/* SPDX-License-Identifier: GPL-3.0-only
 * Combined saved FX state, stereo routing and zero-depth contracts.
 * By: Inputsignwave. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
static void state_contract(void)
{
 reset();sound_fx=SFX_DEFAULT;sound_fx.slot[0][0]=4u|32u;sound_fx.slot[0][1]=4;sound_fx.slot[0][2]=90;
 sound_fx.slot[13][0]=12u;sound_fx.slot[13][1]=32;sound_fx.slot[13][2]=70;sound_fx.plate[9]=70;sound_fx.plate[12]=1;
 uint8_t wire[SFX_WIRE_BYTES];sound_fx_t got;
 assert(sfx_encode(wire,&sound_fx)&&sfx_decode(&got,wire)&&!memcmp(&got,&sound_fx,sizeof got));
 wire[60]|=128;memset(&got,42,sizeof got);sound_fx_t sentinel=got;
 assert(!sfx_decode(&got,wire)&&!memcmp(&got,&sentinel,sizeof got));
 sound_fx_t saved=sound_fx;assert(!project_save(0));sound_fx=SFX_DEFAULT;project_load(0);assert(!memcmp(&saved,&sound_fx,sizeof saved));
 template_save();sound_fx=SFX_DEFAULT;template_load();assert(!memcmp(&saved,&sound_fx,sizeof saved));
 /* Reference TPLL retains its actual size/magic and acquires neutral FX. */
 uint8_t old[sizeof(tmpl_t)];uint32_t n=(uint32_t)__builtin_offsetof(tmpl_t,sound_fx),size=n+8u,magic=0x4C4C5054u;
 memcpy(old,&tmpl,n);memcpy(old+n,&size,4);memcpy(old+n+4,&magic,4);
 assert(tmpl_take(old,size));sound_fx=saved;template_load();assert(!memcmp(&sound_fx,&SFX_DEFAULT,sizeof sound_fx));
}
static void audio_contract(void)
{
 eng_state_reset();reset();sound_fx=SFX_DEFAULT;fx_clear_all();memset(fx_state,0,sizeof fx_state);fx_plate_owner=255;
 int16_t p[4]={FX_PLATE,127,90,100};fx_plan_t plan;
 fx_slot_prepare(&plan,NPART*FX_SLOTS,p,CTL);assert(plan.active&&plan.mem&&fx_plate.ready);
 assert(resource[RES_FX0+NPART*FX_SLOTS].size==PLATE_SAMPLES*2u);
 fx_plan_t duplicate;fx_slot_prepare(&duplicate,NPART*FX_SLOTS+1u,p,CTL);assert(!duplicate.active);
 fx_slot_prepare(&duplicate,0,p,CTL);assert(!duplicate.active);
 uint64_t energy=0;
 for(uint32_t b=0;b<FS/CTL*3u;b++){
  for(uint32_t i=0;i<CTL;i++)fx_left[i]=fx_right[i]=b==0u&&i==0u?20000:0;
  fx_slot_run(&plan,CTL);for(uint32_t i=0;i<CTL;i++)energy+=(uint32_t)abs(fx_left[i])+abs(fx_right[i]);
 }
 assert(energy>1000);p[0]=FX_DRIVE;p[1]=0;
 for(uint32_t i=0;i<400;i++){fx_slot_prepare(&plan,NPART*FX_SLOTS,p,CTL);if(plan.active)fx_slot_run(&plan,CTL);}
 assert(!resource[RES_FX0+NPART*FX_SLOTS].size);
 p[1]=100;fx_slot_prepare(&plan,0,p,CTL);assert(plan.active&&!resource[RES_FX0].size);
 for(uint32_t b=0;b<300;b++){
  for(uint32_t i=0;i<CTL;i++){fx_left[i]=(i&4)?12000:-12000;fx_right[i]=fx_left[i]/2;}
  fx_slot_run(&plan,CTL);for(uint32_t i=0;i<CTL;i++)assert(abs(fx_left[i])<=65536&&abs(fx_right[i])<=65536);
 }
 sound_fx.slot[0][0]=8;sound_fx.slot[0][1]=10;sound_fx.slot[0][2]=0;
 uint32_t phase=fx_state[0].mod_phase;fx_slot_prepare(&plan,0,p,CTL);assert(fx_state[0].mod_phase==phase);
 sound_fx.slot[0][2]=127;fx_slot_prepare(&plan,0,p,CTL);assert(fx_state[0].mod_phase!=phase);
 puts("FX: versioned state/project/template and old TPLL migration; one shared stereo plate, duplicate/insert refusal, allocation release, bounded drive and neutral/active independent LFO passed");
}
int main(void){state_contract();audio_contract();return 0;}
