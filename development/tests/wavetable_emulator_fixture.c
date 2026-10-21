/* SPDX-License-Identifier: GPL-3.0-only
 * Synthetic saved boot project for emulator auditions — By: Inputsignwave.
 * No user data. Compile like wavetable_test.c; argument is output NOR path. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
int main(int argc,char **argv)
{
    assert(argc==2);reset();song.ntrk=4;song.sel=0;
    const uint32_t engine[4]={ENGI_WAVETABLE,ENGI_FM6,ENGI_PROPHET,ENGI_DRUM};
    for(uint32_t i=0;i<4;i++){host_preset(&trk[i],engine[i],i==0?2u:0u);trk[i].p[P_LEVEL]=64;}
    assert(!project_save_as(0,"WT EMULATOR"));settings_boot=1;settings_save();settings_poll();
    assert(!persist_pending && project_used(0));
    /* Verify through the same cold-boot storage path before exporting. */
    memset(trk,0,sizeof trk);settings_boot=0;persist_boot();project_boot();
    assert(settings_boot==1 && trk[0].eng_req==ENGI_WAVETABLE);
    FILE *f=fopen(argv[1],"wb");assert(f && fwrite(nor,1,sizeof nor,f)==sizeof nor);fclose(f);
    puts("Synthetic boot project: WAVETABLE/GLASS DRIFT, FM6, Prophet, 808. Keyboard selects WT track 1.");
}
