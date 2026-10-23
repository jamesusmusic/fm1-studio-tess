/* SPDX-License-Identifier: GPL-3.0-only
 * Additional FM voices and per-hit kit ownership — By: Inputsignwave. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
int main(void)
{
 eng_state_reset();reset();assert(FM6_NFAC==49u&&ENG_FM6.npresets==50u);
 host_preset(&trk[0],ENGI_FM6,24);uint8_t init[128];fm6_pack(fm6_patch[0],init);assert(!memcmp(init,FM6_INIT,128));
 static const uint32_t expected[25]={3711027973u,3559231951u,1995084773u,3032055733u,3895144477u,3282965865u,2691192640u,279882475u,3149034948u,1584606910u,1569382572u,3143349974u,3371540635u,3022206486u,1985115831u,1802216406u,3333987819u,3568988717u,2221028112u,969330408u,607868996u,1363261121u,2015630535u,2441734417u,679610497u};
 for(uint32_t p=25;p<50;p++){
  host_preset(&trk[0],ENGI_FM6,p);fm6_pack(fm6_patch[0],init);assert(proj_hash(init,128)==expected[p-25]);
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
 /* Changing caps under held notes must never address the retired engine's
  * wider voice slots after the switch fade. */
 for(uint32_t round=0;round<20;round++){
  const uint8_t engine[]={ENGI_FM6,ENGI_WAVE_LAYERS,ENGI_FMKIT};
  for(uint32_t k=0;k<3;k++){
   set_engine_of(t,engine[k]);for(uint32_t b=0;b<16;b++)mix_block(audio,CTL);
   for(uint32_t key=0;key<16;key++)trk_note_on(t,36+key,100);
   for(uint32_t b=0;b<20;b++)mix_block(audio,CTL);
   for(uint32_t key=0;key<16;key++)trk_note_off(t,36+key);
  }
 }
 assert(!resource_failures);
 printf("FM6: 25 additive exact voices, INIT preserved; kits overlap with immutable native hit patches, release and recall; state=%zu\n",sizeof(fmk_part_t));
 printf("settings+template=%zu bank spare=%u\n",sizeof set_rec,(unsigned)(BANK_STORE_SIZE-4u-BANK_SFX_OFF-SFX_WIRE_BYTES));
 return 0;
}
