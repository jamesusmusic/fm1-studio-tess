/* SPDX-License-Identifier: GPL-3.0-only
 * Additional FM voices and per-hit kit ownership — By: Inputsignwave. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
int main(void)
{
 eng_state_reset();reset();assert(FM6_NFAC==49u&&ENG_FM6.npresets==50u);
 host_preset(&trk[0],ENGI_FM6,24);uint8_t init[128];fm6_pack(fm6_patch[0],init);assert(!memcmp(init,FM6_INIT,128));
 for(uint32_t p=25;p<50;p++){
  host_preset(&trk[0],ENGI_FM6,p);fm6_pack(fm6_patch[0],init);assert(!memcmp(init,FM6_ADDED[p-25],128));
 }
 host_preset(&trk[0],ENGI_FMKIT,0);assert(drum_track(&trk[0]));
 track_t *t=&trk[0];int32_t audio[CTL*2];
 trk_note_on(t,36,100);mix_block(audio,CTL);
 uint32_t first=0;while(first<NVOICE&&!t->v[first].active)first++;assert(first<NVOICE);
 uint8_t original[FP_SIZE+1u];memcpy(original,fmk_part(t)->v[first].patch,sizeof original);
 trk_note_on(t,38,110);mix_block(audio,CTL);
 assert(!memcmp(original,fmk_part(t)->v[first].patch,sizeof original));
 uint32_t active=0;for(uint32_t v=0;v<NVOICE;v++)active+=t->v[v].active;assert(active>=2);
 t->p[P_E0]=36;assert(!memcmp(original,fmk_part(t)->v[first].patch,sizeof original));
 for(uint32_t n=0;n<FS*15;n+=CTL)mix_block(audio,CTL);
 for(uint32_t v=0;v<NVOICE;v++)assert(!t->v[v].active);
 assert(!project_save(0));t->p[P_E0]=1;project_load(0);assert(t->p[P_E0]==36&&t->eng_req==ENGI_FMKIT);
 printf("FM6: 25 additive exact voices, INIT preserved; kits overlap with immutable native hit patches, release and recall; state=%zu\n",sizeof(fmk_part_t));
 printf("settings+template=%zu bank spare=%u\n",sizeof set_rec,(unsigned)(BANK_STORE_SIZE-4u-BANK_SFX_OFF-SFX_WIRE_BYTES));
 return 0;
}
