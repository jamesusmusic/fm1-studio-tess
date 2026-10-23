/* SPDX-License-Identifier: GPL-3.0-only
 * Browse identity and mixed UI/audio stress — By: Inputsignwave. */
#define PERFORMANCE10_FX_UI_MAIN fx_ui_main
#include "performance10_fx_ui_test.c"
#undef PERFORMANCE10_FX_UI_MAIN
int main(void){
 fx_ui_main();ui_power_on();list_set(LM_ALL);
 uint8_t seen[213]={0};uint32_t total,as,ak;
 for(uint32_t n=0;n<213;n++){
  list_scan(LM_ALL,~0u,0,n,&as,&ak,&total);
  assert(as==ENGI_PROPHET&&ak<213&&!seen[ak]);seen[ak]=1;
  assert(ak==(n+201)%213);
  uint32_t back=list_scan(LM_ALL,as,ak,~0u,&as,&ak,&total);assert(back==n);
 }
 select_engine(ENGI_PROPHET);assert(TSEL->preset==201);
 sound_init_of(TSEL,ENGI_PROPHET);assert(TSEL->preset==0);
 assert(str_eq(N_FX[FX_PLATE],"REVERB"));
 char nm[16];list_words(nm,N_FX[FX_PLATE],sizeof nm);assert(ty_fit(TY_TX,nm,POP_PICK_W-18)==TY_TX);
 int32_t out[CTL*2];uint32_t random=0x19790831;
 for(uint32_t pass=0;pass<512;pass++){
  uint32_t e=pass%3==0?ENGI_FMKIT:pass%3==1?ENGI_WAVE_LAYERS:ENGI_WAVETABLE;
  engine_load_of(TSEL,e,pass%ENGINES[e]->npresets);
  for(uint32_t settle=0;settle<16;settle++)mix_block(out,CTL);
  for(uint32_t p=0;p<NPAGES;p++)if(PAGES[p].fam==FAM_FX&&PAGES[p].scope==SC_TRACK&&PAGES[p].graph==GR_NONE){ui.home=0;ui.page=p;break;}
  TSEL->p[P_FXTYPE]=FX_DRIVE;TSEL->p[P_FXMIX]=38;
  for(uint32_t k=0;k<16;k++)trk_note_on(TSEL,36+k,100);
  for(uint32_t b=0;b<16;b++){
   random=random*1664525u+1013904223u;
   TSEL->p[P_FXRATE]=(random>>16)&127;TSEL->p[P_FXCHAR]=(random>>8)&127;TSEL->p[P_FXTONE]=(int)(random%127)-63;
   mix_block(out,CTL);cv_begin(PV_PANEL_W,120,T_PANEL);pv_fx(120);
  }
  for(uint32_t k=0;k<16;k++)trk_note_off(TSEL,36+k);
 }
 puts("Hardening: Prophet additions first, 213 canonical IDs exactly once/inverse lookup, explicit INIT retained; REVERB normal font; 8192 audio+full-preview blocks under held-note engine switches and random Drive controls passed");
 return 0;
}
