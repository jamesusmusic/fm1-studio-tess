/* SPDX-License-Identifier: GPL-3.0-only
 * Layered engine signal/persistence contracts — By: Inputsignwave. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
static void setup(uint32_t preset)
{
 eng_state_reset();reset();host_preset(&trk[0],ENGI_WAVE_LAYERS,preset);
 trk[0].p[P_VOICE]=V_POLY;
}
static uint64_t sound(uint32_t preset,int layer,int attack,int cut)
{
 setup(preset);track_t *t=&trk[0];
 t->p[P_E4]=layer?127:0;t->p[P_E5]=0;
 t->p[layer?P_FM2_LEVEL:P_FM1_LEVEL]=127;
 t->p[layer?P_FM1_ATK:P_ATK]=attack;
 t->p[layer?P_FM2_REL:P_FM2_ATK]=cut;
 trk_note_on(t,60,110);int32_t audio[CTL*2];uint64_t energy=0;
 for(uint32_t n=0;n<FS/4;n+=CTL){mix_block(audio,CTL);for(uint32_t j=0;j<CTL*2;j++)energy+=(uint32_t)abs(audio[j]);}
 trk_note_off(t,60);
 for(uint32_t n=0;n<FS*12;n+=CTL)mix_block(audio,CTL);
 for(uint32_t j=0;j<NVOICE;j++)assert(!t->v[j].active);
 return energy;
}
int main(void)
{
 assert(eng_ok(ENGI_WAVE_LAYERS)&&sizeof WL_FLASH==5952);
 uint64_t previous[8];
 for(uint32_t p=0;p<8;p++){
  previous[p]=sound(p,0,0,127);assert(previous[p]>10000);
  for(uint32_t q=0;q<p;q++)assert(previous[p]!=previous[q]);
  assert(previous[p]==sound(p,0,0,127));
 }
 assert(sound(0,0,127,127)<sound(0,0,0,127)/2);
 assert(sound(0,1,127,127)<sound(0,1,0,127)/2);
 assert(sound(0,0,0,10)<sound(0,0,0,127));
 assert(sound(0,1,0,10)<sound(0,1,0,127));
 setup(3);track_t *t=&trk[0];t->p[P_FM1_ATK]=73;t->p[P_FM3_REL]=60;t->p[P_LN7]=8;
 assert(!project_save(0));t->p[P_FM1_ATK]=0;project_load(0);
 assert(t->p[P_FM1_ATK]==73&&t->p[P_FM3_REL]==60&&t->p[P_LN7]==8);
 int rc=up_store(4,"DUAL TEST");assert(rc==0||rc==3);t->p[P_FM1_ATK]=0;assert(!up_load(4));
 assert(t->p[P_FM1_ATK]==73&&t->p[P_LN7]==8);
 printf("layers: eight distinct deterministic banks, independent envelopes/filters, complete release, project recall; state=%zu assets=%zu\n",sizeof(wl_part_t),sizeof WL_FLASH);
 return 0;
}
