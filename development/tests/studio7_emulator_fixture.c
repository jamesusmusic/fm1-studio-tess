/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess synthetic emulator setup — By: Inputsignwave.
 * Builds real A/B records with the firmware encoder. Contains no user data.
 * Usage: build/host/studio7_emulator_fixture OUTPUT.nor */
#define STUDIO_STORAGE_TEST_INCLUDE 1
#include "backup_test.c"
#include <assert.h>
int main(int argc,char **argv)
{
    assert(argc==2);reset();song.ntrk=4;song.sel=0;song.g[G_BPM]=120;
    const uint8_t engines[4]={ENGI_FM6,ENGI_CZ,ENGI_PROPHET,ENGI_DRUM};
    const uint8_t modes[4]={TESS_ENG_AWAKE,TESS_ENG_POLY,TESS_ENG_BOUNCE,TESS_ENG_KIT};
    for(uint32_t tr=0;tr<4;tr++){
        host_preset(&trk[tr],engines[tr],0);trk[tr].p[P_LEVEL]=64;trk[tr].p[P_SLEN]=16;trk[tr].p[P_SDIV]=2;
        tessera.mode[tr]=modes[tr];studio7_track[tr].mode=modes[tr];studio7_track[tr].seeded=1;studio7_track[tr].seed=(uint16_t)(tr+1u);
    }
    s7_meter_t *a=&studio7_track[0].meter;a->enabled=1;a->numerator=5;a->denominator=8;a->group_count=2;memset(a->group,0,8);a->group[0]=3;a->group[1]=2;a->ratio_n=3;a->ratio_d=2;
    s7_meter_t *b=&studio7_track[1].meter;b->enabled=1;b->numerator=7;b->denominator=8;b->group_count=3;memset(b->group,0,8);b->group[0]=2;b->group[1]=2;b->group[2]=3;b->ratio_n=5;b->ratio_d=4;
    studio7_track[2].bounce.balls=3;studio7_track[2].bounce.speed=40;
    s7_kit_t *k=&studio7_track[3].kit;k->enabled=1;k->human=60;k->variation=35;k->fill=25;
    for(uint32_t i=0;i<16;i++){
        step_t *s=&trk[3].step[i];memset(s,0,sizeof *s);s->time=ST_NOTE;s->vel=96;
        s->hit=(uint8_t)((i%4==0?1u:0u)|(i%8==4?2u:0u)|(1u<<3));s->acc=(uint8_t)(s->hit&(i%4==0?255u:0u));
        if(i%4==0)k->anchors[i]=(uint8_t)(s->hit&3u);
    }
    assert(!project_save_as(0,"S7 EMU TEST"));settings_boot=1;settings_save();settings_poll();
    FILE *f=fopen(argv[1],"wb");assert(f&&fwrite(nor,1,sizeof nor,f)==sizeof nor);fclose(f);
    puts("studio7: synthetic NOR with FM6/AWAKE 5/8 3:2, CZ/POLY 7/8 5:4, Prophet/BOUNCE and eight-lane DRUM kit");
    return 0;
}
