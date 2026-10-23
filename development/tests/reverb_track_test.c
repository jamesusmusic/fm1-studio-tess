/* SPDX-License-Identifier: GPL-3.0-only
 * Track-facing selection of one shared reverb — By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
static void select_track_reverb(uint32_t track,uint32_t slot)
{
 song.sel=track;go_title("FX");fx_rack_row[0]=slot;
 TSEL->p[P_FXTYPE+slot*FX_PARAMS]=FX_CHORUS;
 turn(EN_K1,1);assert(TSEL->p[P_FXTYPE+slot*FX_PARAMS]==FX_PLATE);
}
static void controls(void)
{
 ui_power_on();song.ntrk=NTRK;
 for(uint32_t t=0;t<NTRK;t++)for(uint32_t s=0;s<FX_SLOTS;s++){
  select_track_reverb(t,s);assert(reverb_slot()==0);
  for(uint32_t a=0;a<20+t;a++)turn(EN_K4,1);assert(TSEL->p[P_FXMIX+s*FX_PARAMS]==20+(int)t);
  assert(str_eq(track_desc(TSEL,P_FXMIX+s*FX_PARAMS)->label,"SEND"));
  press(B_OCTUP);assert(cur_page()->scope==SC_TRACK && cur_page()->graph==GR_NONE);
  int16_t *vp;page_desc(cur_page(),0,&vp);assert(vp==&TSEL->p[P_FXMIX+s*FX_PARAMS]);
  page_desc(cur_page(),1,&vp);assert(vp==&song.g[G_MFXRATE]);
  page_desc(cur_page(),2,&vp);assert(vp==&song.g[G_MFXCHAR]);
  int before=*vp;turn(EN_K3,1);assert(song.g[G_MFXCHAR]==before+1);
  char b[16];head_chip_text(b);assert(str_eq(b,"REV SHARED"));
  assert(!resource[RES_FX0+t*FX_SLOTS+s].size);
 }
 assert(song.g[G_MFXTYPE]==FX_PLATE && !song.g[G_MFX2TYPE] && !song.g[G_MFX3TYPE]);
 /* The full picker goes through the same binding path as the rack knob. */
 ui_power_on();go_title("FX");fx_rack_row[0]=0;
 vlist_open(ui.page,0);vl.sel=FX_PLATE;vlist_input(0,2);
 assert(TSEL->p[P_FXTYPE]==FX_PLATE && reverb_slot()==0);
 /* Existing shared effects are retained, including CHAIN/SPLIT topology. */
 ui_power_on();song.g[G_FXMODE]=1;song.g[G_MFXTYPE]=FX_DRIVE;song.g[G_MFX2TYPE]=FX_ECHO;
 select_track_reverb(0,0);assert(reverb_slot()==2 && song.g[G_FXMODE]==1);
 assert(song.g[G_MFXTYPE]==FX_DRIVE && song.g[G_MFX2TYPE]==FX_ECHO);
 song.g[G_MFXRATE+2*FX_PARAMS]=87;select_track_reverb(1,1);assert(song.g[G_MFXRATE+2*FX_PARAMS]==87);
 ui_power_on();song.g[G_MFXTYPE]=FX_DRIVE;song.g[G_MFX2TYPE]=FX_ECHO;song.g[G_MFX3TYPE]=FX_PHASER;
 go_title("FX");fx_rack_row[0]=0;TSEL->p[P_FXTYPE]=FX_CHORUS;turn(EN_K1,1);
 assert(TSEL->p[P_FXTYPE]==FX_CHORUS && str_eq(ui.msg,"SHARED FX FULL"));
 assert(song.g[G_MFXTYPE]==FX_DRIVE && song.g[G_MFX2TYPE]==FX_ECHO && song.g[G_MFX3TYPE]==FX_PHASER);
 TSEL->p[P_FXTYPE]=FX_PLATE;fx_bus_active();assert(reverb_slot()==FX_SLOTS);
 go_title("FX 1");char b[16];head_chip_text(b);assert(str_eq(b,"REV NO BUS"));
 int16_t *vp;assert(!page_desc(cur_page(),1,&vp)&&!vp);
 puts("Track REVERB: four tracks/three slots, knob+picker, independent SEND, common SIZE/DECAY, visible shared context and occupied-bus protection passed");
}
static void routing(void)
{
 ui_power_on();song.ntrk=NTRK;
 for(uint32_t t=0;t<NTRK;t++){select_track_reverb(t,0);trk[t].p[P_FXMIX]=20+20*t;}
 for(uint32_t mode=0;mode<2;mode++)for(uint32_t bus=0;bus<FX_SLOTS;bus++){
  song.g[G_FXMODE]=mode;fx_bus_mix=0;fx_bus_select();
  song.g[G_MFXTYPE]=song.g[G_MFX2TYPE]=song.g[G_MFX3TYPE]=0;song.g[G_MFXTYPE+bus*FX_PARAMS]=FX_PLATE;
  for(uint32_t t=0;t<NTRK;t++)for(uint32_t dest=0;dest<FX_SLOTS;dest++){
   uint32_t amount=fx_send_amount(&trk[t],dest);
   assert(amount==((!mode || bus==dest)?20+20*t:0));
  }
  trk[0].p[P_FX2TYPE]=FX_PLATE;trk[0].p[P_FX2MIX]=10;
  assert(fx_send_amount(&trk[0],mode?bus:0)==20); /* duplicate markers do not add */
  trk[0].p[P_FXSEND+(mode?bus:0)]=40;
  assert(fx_send_amount(&trk[0],mode?bus:0)==40); /* explicit and marker do not add */
  trk[0].p[P_FXSEND+(mode?bus:0)]=0;
 }
 /* Actual mixer contribution remains post-level/pan/mute/solo, and dry
  * output is not changed by the virtual insert. Test serial and split buses. */
 for(uint32_t mode=0;mode<2;mode++){
  ui_power_on();song.ntrk=2;host_preset(&trk[0],ENGI_WAVE_LAYERS,5);host_preset(&trk[1],ENGI_PROPHET,201);
  song.g[G_FXMODE]=mode;song.g[G_MFXTYPE]=FX_DRIVE; /* reverb uses bus B */
  select_track_reverb(0,0);trk[0].p[P_FXMIX]=64;
  select_track_reverb(1,1);trk[1].p[P_FX2MIX]=100;
  assert(reverb_slot()==1);
  int32_t out[CTL*2];trk_note_on(&trk[0],60,100);trk_note_on(&trk[1],67,100);
  uint64_t wet=0;
  for(uint32_t b=0;b<160;b++){
   mix_block(out,CTL);
   for(uint32_t i=0;i<CTL;i++)wet+=abs(mode?fx_send_extra_l[0][i]:fx_send_l[i]);
   assert(!resource[RES_FX0].size && !resource[RES_FX0+FX_SLOTS+1].size);
  }
  assert(wet && fx_plate_owner==NPART*FX_SLOTS+1 && fx_plate.ready);
  uint32_t tanks=0;for(uint32_t k=0;k<FX_STATE_COUNT;k++)tanks+=resource[RES_FX0+k].size==PLATE_SAMPLES*2u;
  assert(tanks==1);
  track_solo=1u<<3; /* a different track is soloed: both sources fade out */
  for(uint32_t b=0;b<16;b++)mix_block(out,CTL);
  for(uint32_t i=0;i<CTL;i++)assert(!(mode?fx_send_extra_l[0][i]:fx_send_l[i]));
 }
 /* A saved track marker can recover its shared slot after a sound/preset
  * load without overwriting existing shared effects or changing format. */
 ui_power_on();TSEL->p[P_FXTYPE]=FX_PLATE;TSEL->p[P_FXMIX]=61;assert(fx_bus_active());assert(reverb_slot()==0);
 project_t q;project_capture(&q);project_store_t wire;
 assert(proj_pack(&wire,&q)&&proj_import(&proj_scratch,&wire,sizeof wire));
 assert(!project_restore_runtime(&proj_scratch));
 assert(TSEL->p[P_FXTYPE]==FX_PLATE && TSEL->p[P_FXMIX]==61 && reverb_slot()==0);
 template_save();TSEL->p[P_FXTYPE]=FX_OFF;song.g[G_MFXTYPE]=FX_OFF;template_load();
 assert(TSEL->p[P_FXTYPE]==FX_PLATE && TSEL->p[P_FXMIX]==61 && reverb_slot()==0);
 puts("Track REVERB: CHAIN/SPLIT, all bus positions, max-not-sum sends, actual audio/solo, one tank, project wire and template recall passed");
}
int main(void){controls();routing();return 0;}
