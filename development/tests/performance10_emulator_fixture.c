/* SPDX-License-Identifier: GPL-3.0-only
 * Synthetic combined boot song for emulator validation — By: Inputsignwave.
 * No user saved data. Package application must overlay this synthetic NOR. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
int main(int argc,char **argv)
{
 assert(argc==2);reset();song.ntrk=4;song.sel=0;sound_fx=SFX_DEFAULT;
 const uint8_t engine[4]={ENGI_WAVE_LAYERS,ENGI_FMKIT,ENGI_PROPHET,ENGI_FM6};
 const uint8_t preset[4]={0,0,205,25};
 for(uint32_t k=0;k<4;k++){
  host_preset(&trk[k],engine[k],preset[k]);trk[k].p[P_LEVEL]=45;trk[k].p[P_SLEN]=16;trk[k].p[P_SDIV]=2;trk[k].p[P_FXSEND]=24;
  for(uint32_t s=0;s<16;s++)trk[k].step[s]=(step_t){{0},0,ST_REST};
  for(uint32_t s=0;s<16;s+=4)trk[k].step[s]=(step_t){{(uint8_t)(k==0?60:k==2?48:67)},1,ST_NOTE,0,100};
 }
 for(uint32_t s=0;s<16;s+=2)trk[1].step[s]=(step_t){.time=ST_NOTE,.vel=100,.hit=(uint8_t)((s%8u?0u:1u)|(1u<<DV_HATC)),.acc=(uint8_t)(s%8u?0u:1u)};
 song.g[G_MFXTYPE]=FX_PLATE;song.g[G_MFXMIX]=127;song.g[G_MFXRATE]=90;song.g[G_MFXCHAR]=100;song.g[G_FXRETURN]=70;
 trk[1].p[P_FXTYPE]=FX_DRIVE;trk[1].p[P_FXMIX]=24;trk[1].p[P_FXRATE]=30;trk[1].p[P_FXCHAR]=80;
 sound_fx.slot[12][0]=16u|32u;sound_fx.slot[12][1]=2;sound_fx.slot[12][2]=18;
 assert(!project_save_as(0,"ISW COMBINED"));settings_boot=1;settings_save();settings_poll();assert(!persist_pending);
 memset(trk,0,sizeof trk);settings_boot=0;persist_boot();project_boot();
 assert(settings_boot==1&&trk[0].eng_req==ENGI_WAVE_LAYERS&&trk[1].eng_req==ENGI_FMKIT&&song.g[G_MFXTYPE]==FX_PLATE);
 FILE *f=fopen(argv[1],"wb");assert(f&&fwrite(nor,1,sizeof nor,f)==sizeof nor);fclose(f);
 puts("Synthetic full song: dual WT, FM kit, new Prophet, Eno FM6, stereo plate, warm drive and clocked FX LFO");return 0;
}
