/* SPDX-License-Identifier: GPL-3.0-only
 * Hardware-reported FX preview crash and retired picker regression.
 * By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
#ifndef PERFORMANCE10_FX_UI_MAIN
#define PERFORMANCE10_FX_UI_MAIN main
#endif
int PERFORMANCE10_FX_UI_MAIN(void)
{
 ui_power_on();host_preset(TSEL,ENGI_WAVE_LAYERS,5);
 /* Rendering the full preview, not just columns, reproduces the RC1 null hint. */
 for(uint32_t p=0;p<NPAGES;p++)if(PAGES[p].fam==FAM_FX && PAGES[p].graph==GR_NONE && PAGES[p].scope!=SC_SFX){
  ui.home=0;ui.page=(uint8_t)p;page_entered();
  uint32_t master=PAGES[p].scope==SC_GLOBAL;
  int16_t *fx=(master?song.g:TSEL->p)+PAGES[p].id[0]-1u;
  for(uint32_t type=0;type<FX_COUNT;type++){
   fx[0]=type;fx[1]=38;fx[2]=60;fx[3]=72;
   cv_begin(PV_PANEL_W,120,T_PANEL);pv_fx(120);
   trk_note_on(TSEL,60,100);int32_t audio[CTL*2];mix_block(audio,CTL);
   cv_begin(PV_PANEL_W,120,T_PANEL);pv_fx(120);trk_note_off(TSEL,60);
  }
 }
 const param_desc_t *d=&TP[P_FXTYPE];
 assert(enum_rows(d)==FX_COUNT-2);
 for(int32_t r=0;r<enum_rows(d);r++){
  int32_t v=enum_value(d,r);assert(v!=FX_ROOM&&v!=FX_REVERB&&N_FX[v][0]);
  assert(enum_rank(d,v)==r);
 }
 assert(enum_step(d,FX_ECHO,FX_ROOM)==FX_CHORUS);
 assert(enum_step(d,FX_CHORUS,FX_REVERB)==FX_ECHO);
 assert(enum_orig(d,FX_ROOM)==FX_OFF&&enum_orig(d,FX_REVERB)==FX_OFF);
 char formatted[16];const char *unit;param_format(d,FX_ROOM,formatted,&unit);assert(str_eq(formatted,"OFF"));
 char name[16];list_words(name,N_FX[FX_DRIVE],sizeof name);
 assert(ty_fit(TY_TX,name,POP_PICK_W-18)==TY_TX);
 puts("Performance10: retired FX omitted from compact picker, original IDs preserved, Drive fits normal text font");
 puts("Performance10: complete FX previews, all effects and insert/send pages, Phase Prism note playback at 30 percent mix passed");
 return 0;
}
