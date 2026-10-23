/* SPDX-License-Identifier: GPL-3.0-only
 * Retired editor removal must preserve archive bytes — By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
int main(void)
{
 ui_power_on();cz_init();
 for(uint32_t tr=0;tr<NTRK;tr++){
  uint8_t before[CZ_BYTES],out[CZ_BYTES],untouched[CZ_BYTES];
  memcpy(before,cz_patch[tr].raw,CZ_BYTES);memset(out,0xa5,sizeof out);memcpy(untouched,out,sizeof out);
  for(uint32_t id=0;id<LCZ_NP;id++){
   int16_t *value=(int16_t *)1;
   assert(!cz_ed_desc(tr,id,0,&value)&&!value);
   assert(!cz_ed_put(tr,id,99,out));
   assert(!memcmp(out,untouched,sizeof out)&&!memcmp(cz_patch[tr].raw,before,CZ_BYTES));
  }
  trk[tr].eng_req=ENGI_CZ;cz_patch[tr].raw[128]=(uint8_t)('A'+tr);
 }
 for(uint32_t p=0;p<NPAGES;p++)if(PAGES[p].scope==SC_CZ1){
  assert(!page_visible(p));
  for(uint32_t c=0;c<4;c++){int16_t *value=(int16_t *)1;assert(!page_desc(&PAGES[p],c,&value)&&!value);}
 }
 uint8_t archived[sizeof cz_patch];memcpy(archived,cz_patch,sizeof archived);
 assert(!project_save(0));memset(cz_patch,0,sizeof cz_patch);project_load(0);
 assert(!memcmp(archived,cz_patch,sizeof archived));
 template_save();memset(cz_patch,0,sizeof cz_patch);template_load();
 assert(!memcmp(archived,cz_patch,sizeof archived));
 puts("Space reclaim: retired CZ editor refuses every control/page without writes; raw archive bytes survive project and template recall");
 return 0;
}
