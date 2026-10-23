/* SPDX-License-Identifier: GPL-3.0-only
 * Production queue/voice/render USB MPE contracts — By: Inputsignwave. */
#define MIDI_CONTROL_NO_MAIN
#include "midi_control_test.c"
#include <assert.h>
static voice_t *member(uint32_t ch,uint32_t note,uint32_t key)
{
 for(uint32_t i=0;i<NVOICE;i++){voice_t *v=&trk[0].v[i];if(v->active&&v->mpe_ch==ch&&v->note==note&&v->mpe_key==key)return v;}
 return 0;
}
static void setup(void)
{
 midi_test_reset();host_preset(&trk[0],ENGI_WAVE_LAYERS,0);
 song.master_q12=4096;trk[0].p[P_VOICE]=V_POLY;usb.suspended=0;
 song.g[G_ROUTE]=5;events_block(CTL);
 assert(mpe.members==15 && mpe_route==5);
}
static void render(uint32_t blocks)
{int32_t out[CTL*2];for(uint32_t b=0;b<blocks;b++)mix_block(out,CTL);}
static void identity(void)
{
 setup();queued(0xE1,127,127,1);queued(0xD1,90,0,1);queued(0xB1,74,100,1);
 queued(0x91,60,100,1);queued(0x92,60,100,1);
 voice_t *a=member(1,60,1),*b=member(2,60,1);assert(a&&b&&a!=b);
 assert(a->mpe_bend==48*256&&b->mpe_bend==0&&a->mpe_pressure==90&&a->mpe_slide==100);
 queued(0xE2,0,0,1);queued(0xE0,127,127,1);
 assert(a->mpe_bend==50*256&&b->mpe_bend==-46*256);
 queued(0xD2,10,0,1);queued(0xB2,74,5,1);
 assert(a->mpe_pressure==90&&b->mpe_pressure==10&&a->mpe_slide==100&&b->mpe_slide==5);
 song.sel=1;queued(0x81,60,73,1);
 assert(!a->gate&&a->mpe_key==0&&a->mpe_lift==73&&b->gate);
 int32_t frozen=a->mpe_bend;
 queued(0xE1,0,0,1);queued(0x91,65,100,1);assert(a->mpe_bend==frozen);
 assert(member(1,65,1));queued(0x82,60,0,1);assert(!b->gate);
 render(10);
 puts("MPE: equal pitches, opposite/member+master bends, pre-note controls, pressure/slide, Lift, pinned track and frozen tails passed");
}
static void lifecycle(void)
{
 setup();queued(0x91,60,100,1);voice_t *a=member(1,60,1);assert(a);
 queued(0xB0,64,127,1);queued(0x81,60,60,1);assert(a->gate&&a->mpe_key==2);
 queued(0xE1,127,127,1);assert(!a->mpe_bend);
 queued(0x91,60,110,1);voice_t *b=member(1,60,1);assert(b&&b!=a&&!a->gate);
 queued(0xB0,64,0,1);assert(b->gate);
 queued(0xB1,123,0,1);assert(!b->gate);
 queued(0x91,61,100,1);queued(0xB0,101,0,1);queued(0xB0,100,6,1);queued(0xB0,6,3,1);
 assert(mpe.members==3&&!member(1,61,1));queued(0x94,62,100,1);assert(!member(4,62,1));
 queued(0xB1,101,0,1);queued(0xB1,100,0,1);queued(0xB1,6,12,1);queued(0xB1,38,50,1);
 queued(0xE1,127,127,1);queued(0x91,64,100,1);assert(member(1,64,1)->mpe_bend==3200);
 queued(0xB0,121,0,1);assert(!member(1,64,1)->mpe_bend);
 usb.suspended=1;events_block(CTL);assert(!member(1,64,1));usb.suspended=0;
 queued(0x91,66,100,1);usb.resets++;events_block(CTL);assert(!member(1,66,1));
 queued(0x91,67,100,1);song.g[G_ROUTE]=0;events_block(CTL);assert(!member(1,67,1));
 setup();queued(0x91,60,100,1);panic_req|=1;events_block(CTL);assert(!member(1,60,1));
 queued(0x91,61,100,1);voice_t *victim=member(1,61,1);assert(victim);
 voice_capacity_kill(victim);assert(victim->stage==4);queued(0x81,61,0,1);
 assert(victim->stage==4&&!victim->mpe_key);
 queued(0x91,62,100,1);midi_in_overflow=1;events_block(CTL);assert(!member(1,62,1));
 puts("MPE: sustain, repeated note, RPN zone/range, controllers reset, USB suspend/reset, route change, panic and overflow passed");
}
static void isolation(void)
{
 midi_test_reset();song.g[G_ROUTE]=0;queued(0x90,60,100,1);assert(gate_note(&trk[0],60));
 song.g[G_ROUTE]=5;events_block(CTL);assert(!gate_note(&trk[0],60));
 setup();queued(0x91,60,100,1);voice_t *a=member(1,60,1);assert(a);
 queued(0x90,60,100,2);voice_t *local=0;
 for(uint32_t i=0;i<NVOICE;i++)if(trk[0].v[i].gate&&!trk[0].v[i].mpe_ch)local=&trk[0].v[i];
 assert(local&&local!=a);queued(0x80,60,0,2);assert(a->gate&&!local->gate);
 queued(0xE1,127,127,2);assert(!a->mpe_bend);
 queued(0xB0,120,0,1);assert(!a->gate);
 setup();for(uint32_t ch=1;ch<16;ch++)queued(0x90|ch,59+ch,110,1);
 uint32_t n=0;for(uint32_t i=0;i<NVOICE;i++)n+=trk[0].v[i].gate;assert(n==4);
 queued(0x81,60,0,1);n=0;for(uint32_t i=0;i<NVOICE;i++)n+=trk[0].v[i].gate;assert(n>=3);
 set_engine_of(&trk[0],ENGI_PROPHET);events_block(CTL);queued(0x91,72,100,1);assert(!member(1,72,1));
 puts("MPE: USB/TRS equal-pitch isolation, no controller leak, bounded four-voice stealing, stale note-off and unsupported engine passed");
}
static uint64_t pcm(uint32_t mode)
{
 setup();trk[0].p[P_ATK]=0;trk[0].p[P_FM1_ATK]=0;
 queued(0x91,60,110,1);queued(0x92,67,110,1);
 if(mode==1)queued(0xE1,0,80,1);
 if(mode==2)queued(0xD1,127,0,1);
 if(mode==3)queued(0xB1,74,127,1);
 int32_t out[CTL*2];uint64_t hash=1469598103934665603ull,energy=0;
 for(uint32_t b=0;b<150;b++){
  mix_block(out,CTL);for(uint32_t i=0;i<CTL*2;i++){hash^=(uint32_t)out[i];hash*=1099511628211ull;energy+=(uint32_t)abs(out[i]);}
 }
 assert(energy>10000);return hash;
}
static void stress(void)
{
 setup();uint32_t rng=0x12345678;int32_t out[CTL*2];
 for(uint32_t i=0;i<12000;i++){
  rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;uint32_t ch=1+(rng%15);
  static const uint8_t st[]={0x90,0x80,0xe0,0xd0,0xb0};
  uint32_t status=st[(rng>>8)%5]|ch,a=(rng>>16)&127,b=(rng>>24)&127;
  midi_enqueue((status>>4)|status<<8|a<<16|b<<24,1);
  if(!(i%32)){events_block(CTL);mix_block(out,CTL);}
 }
 events_block(CTL);queued(0xB0,120,0,1);render(20);
 for(uint32_t i=0;i<NVOICE;i++)assert(!trk[0].v[i].mpe_key);
 puts("MPE: 12,000 queued mixed expression/note events with real rendering passed");
}
int main(void)
{
 identity();lifecycle();isolation();uint64_t h[4];for(uint32_t m=0;m<4;m++){h[m]=pcm(m);for(uint32_t j=0;j<m;j++)assert(h[j]!=h[m]);}
 puts("MPE: actual stereo PCM responds separately to bend, pressure and slide");stress();
 assert(!controls_test()&&!sustain_test()&&!ownership_test());
 puts("Live USB MPE integration passed; legacy controls/ownership remain intact");return 0;
}
