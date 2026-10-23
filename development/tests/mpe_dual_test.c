/* SPDX-License-Identifier: GPL-3.0-only
 * Concurrent live MPE + three other engines/FX — By: Inputsignwave.
 * Reuses upstream dual-core test harness with its original notices intact. */
#define MELODEE_DUAL_CORE_TEST_NO_MAIN
#include "dual_core_test.c"
static void queue(uint32_t st,uint32_t a,uint32_t b)
{ assert(midi_enqueue((st>>4)|st<<8|a<<16|b<<24,1)); }
static void run(int parallel)
{
 fx_clear_all();
 memset(trk,0,sizeof trk);memset(&song,0,sizeof song);eng_state_reset();
 memset(fm6_eff,0,sizeof fm6_eff);memset(p5_ready,0,sizeof p5_ready);cz_init();
 memset(mix_cache,0,sizeof mix_cache);memset(&mod,0,sizeof mod);memset(fx_state,0,sizeof fx_state);
 memset(midi_bend_q8,0,sizeof midi_bend_q8);memset(midi_bend_target,0,sizeof midi_bend_target);
 fx_bus_mix=fx_bus_target=fx_bus_pending=fx_bus_result=0;fx_epoch++;
 rng_state=0x12345678u;mod_seed=0x2545f491u;vage=voice_kills=0;track_solo=0;
 lim_env=LIM_T;dc_l=dc_r=dce_l=dce_r=0;host_tracks_init();fm6_fn_reset();
 serial_render=!parallel;audio_worker_online=1;
 memset(midi_sel_on,0,sizeof midi_sel_on);memset(midi_usb_owned,0,sizeof midi_usb_owned);
 memset(midi_ch,0,sizeof midi_ch);memset(midi_owners,0,sizeof midi_owners);
 memset(&mpe,0,sizeof mpe);mpe_route=0;mi_r=mi_w=midi_in_overflow=0;usb.suspended=0;
 const uint8_t es[4]={ENGI_WAVE_LAYERS,ENGI_FMKIT,ENGI_PROPHET,ENGI_FM6};
 for(uint32_t k=0;k<4;k++){host_preset(&trk[k],es[k],k==2?201:0);trk[k].p[P_LEVEL]=45;trk[k].p[P_FXSEND]=24;}
 song.g[G_ROUTE]=5;song.g[G_MFXTYPE]=FX_PLATE;song.g[G_MFXMIX]=127;song.g[G_MFXRATE]=80;song.g[G_MFXCHAR]=90;
 trk[0].p[P_FXTYPE]=FX_DRIVE;trk[0].p[P_FXMIX]=38;trk[0].p[P_FXRATE]=35;
 events_block(CTL);
 for(uint32_t k=1;k<4;k++)trk_note_on(&trk[k],k==1?36:48+k*7,100);
 for(uint32_t ch=1;ch<5;ch++)queue(0x90|ch,60+ch*3,100);
 for(uint32_t block=0;block<256;block++){
  for(uint32_t ch=1;ch<5;ch++){
   queue(0xe0|ch,(block*ch)&127,48+((block+ch)%32));
   queue(0xd0|ch,(block*3+ch*11)&127,0);
   queue(0xb0|ch,74,(block*5+ch*7)&127);
  }
  if(block==48){queue(0xb0,64,127);queue(0x81,63,75);}
  if(block==64)queue(0x91,76,110);
  if(block==96)queue(0xb0,64,0);
  if(block==128){queue(0x82,66,50);queue(0x92,76,90);}
  if(block==160)for(uint32_t ch=5;ch<9;ch++)queue(0x90|ch,60+ch,100);
  if(block==200)queue(0xb0,123,0);
  events_block(CTL);int32_t out[CTL*2];mix_block(out,CTL);
  assert(AW_LOAD(&audio_worker.request)==AW_LOAD(&audio_worker.complete));
  if(!parallel)memcpy(reference+block*CTL*2,out,sizeof out);
  else {if(memcmp(reference+block*CTL*2,out,sizeof out)){fprintf(stderr,"PCM mismatch at block %u\n",block);abort();}}
 }
}
int main(void)
{
 pthread_t thread;assert(!pthread_create(&thread,0,worker_thread,0));while(!AW_LOAD(&audio_worker.ready))sched_yield();
 run(0);run(1);assert(audio_worker.jobs);assert(!pthread_cancel(thread));assert(!pthread_join(thread,0));
 puts("MPE dual-core: 256 blocks bit-identical serial/concurrent stereo PCM with 4 live tracks, plate, warm drive, dense independent expression, sustain/channel reuse/steals; all jobs joined");return 0;
}
