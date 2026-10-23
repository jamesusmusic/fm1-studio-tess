/* SPDX-License-Identifier: GPL-3.0-only
 * Source-derived page inventory — By: Inputsignwave.
 * Build like ui_test.c; run from development to append the inventory to the guide. */
#define UI_TEST_NO_MAIN 1
#include "../tests/ui_test.c"
static const char *family(uint32_t f)
{
 switch(f){case FAM_EDIT:return "EDIT";case FAM_SEQ:return "SEQ";case FAM_GLO:return "GLO";case FAM_TRK:return "GLO / MIXER";case FAM_ENV:return "ENV";case FAM_LFO:return "LFO";case FAM_FX:return "FX";case FAM_SCL:return "SCL";case FAM_ARP:return "ARP";case FAM_SAVE:return "SAVE";default:return "Other";}
}
int main(void)
{
 ui_power_on();tessera_reset();ui.home=0;
 puts("\n## Source-derived page inventory\n");
 puts("Generated from the actual firmware page table and knob drawing code. Each engine table is a union of its reachable pages across PATTERN modes; conditional pages are not all visible together. Native operator rows use the selected operator. Shared pages repeat deliberately so each engine has a complete reference. `—` means an unused knob or a page whose controls are rendered as a list/grid. Values are examples from a fresh host session, not replacements for saved project values.\n");
 for(uint32_t e=0;e<NENG_SHOWN;e++) {
  uint32_t engine=eng_vis(e);set_engine_of(TSEL,engine);
  printf("\n### %s\n\n| Family | Page | Scope | K1 | K2 | K3 | K4 |\n| --- | --- | --- | --- | --- | --- | --- |\n",ENGINES[engine]->name);
  for(uint32_t p=0;p<NPAGES;p++) {
   int visible=0;
   for(uint32_t m=0;m<TESS_ENG_COUNT;m++){tessera.mode[0]=m;if(page_visible(p)){visible=1;break;}}
   if(!visible)continue;
   ui.page=p;
   const char *scope=PAGES[p].scope==SC_GLOBAL?"Project/device (see scope notes)":PAGES[p].scope==SC_STEP?"Pattern / step":"Selected track/sound";
   printf("| %s | %s | %s |",family(PAGES[p].fam),PAGES[p].title,scope);
   for(uint32_t k=0;k<4;k++) {
    lrow_t row={.page=PAGES[p].title,.slot=k};char label[12],value[32];uint16_t colour;
    list_cap(&row,label,value,&colour);
    printf(" %s%s%s |",label[0]?label:"—",value[0]?": ":"",value);
   }
   puts("");
  }
 }
 puts("\n### PATTERN controls by mode\n\n| MODE | K1 | K2 | K3 | K4 |\n| --- | --- | --- | --- | --- |");
 const char *modes[]={"STEP","Turing","Euclid","Markov","MORPH","Grids","BOUNCE","AWAKE","POLY","KIT"};
 for(uint32_t m=0;m<TESS_ENG_COUNT;m++){
  tessera.mode[0]=m;ui.page=page_titled("PATTERN");printf("| %s |",modes[m]);
  for(uint32_t k=0;k<4;k++){lrow_t row={.page="PATTERN",.slot=k};char label[12],value[32];uint16_t colour;list_cap(&row,label,value,&colour);printf(" %s%s%s |",label,value[0]?": ":"",value);}puts("");
 }
 return 0;
}
