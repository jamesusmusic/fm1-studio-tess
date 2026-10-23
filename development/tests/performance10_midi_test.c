/* SPDX-License-Identifier: GPL-3.0-only
 * Combined MIDI control, recording and note lifetime contracts — By: Inputsignwave. */
#define EDITOR_TEST_NO_MAIN
#include "editor_test.c"
#include "../firmware/src/fm6_store.c"
#include <assert.h>
int main(void)
{
 reset();tessera_reset();recording_reset();sound_fx=SFX_DEFAULT;
 assert(request(101,0,0)==73u&&host_wire[5]==1u&&host_wire[6]==0u);
 sound_fx_t wanted=SFX_DEFAULT;wanted.plate[9]=80;wanted.slot[0][0]=4|32;wanted.slot[0][1]=4;wanted.slot[0][2]=64;
 assert(request(101,(const uint8_t *)&wanted,sizeof wanted)==73u&&!host_wire[6]&&!memcmp(&wanted,&sound_fx,sizeof wanted));
 sound_fx_t malformed=wanted;malformed.plate[12]=2;
 assert(request(101,(const uint8_t *)&malformed,sizeof malformed)==73u&&host_wire[6]==1u&&!memcmp(&wanted,&sound_fx,sizeof wanted));
 assert(!request(101,(const uint8_t *)&wanted,sizeof wanted-1u)&&!memcmp(&wanted,&sound_fx,sizeof wanted));
 host_preset(&trk[0],ENGI_WAVE_LAYERS,0);
 const uint8_t ids[]={P_E2,P_FM1_ATK,P_FM3_REL},values[]={7,87,60};
 for(uint32_t i=0;i<3;i++){
  uint32_t v=8192u+values[i];uint8_t a[4]={0,ids[i],v&127u,v>>7};
  assert(request(ED_SET,a,4)>0&&trk[0].p[ids[i]]==values[i]);
 }
 assert(!project_save(0));trk[0].p[P_FM1_ATK]=0;project_load(0);assert(trk[0].p[P_FM1_ATK]==87&&!memcmp(&wanted,&sound_fx,sizeof wanted));
 host_preset(&trk[0],ENGI_FMKIT,0);trk[0].p[P_RECQ]=0;song.sel=0;settings_drumch=0;
 rec_set(&trk[0],1);seq_start();
 midi_in_event(9u|(0x90u<<8)|(36u<<16)|(100u<<24));events_block(CTL);
 assert(step_lanes(&trk[0].step[0])&(1u<<DV_KICK));
 midi_in_event(8u|(0x80u<<8)|(36u<<16));events_block(CTL);seq_stop();
 int32_t out[CTL*2];for(uint32_t n=0;n<FS*15u;n+=CTL)mix_block(out,CTL);
 for(uint32_t v=0;v<NVOICE;v++)assert(!trk[0].v[v].active);
 assert(!trk[0].seq_n&&!trk[0].nheld);
 puts("Performance 10: MIDI FX read/write and malformed atomicity; layered WT controls/project recall; USB MIDI kit recording/release with no stuck owners passed");
 return 0;
}
