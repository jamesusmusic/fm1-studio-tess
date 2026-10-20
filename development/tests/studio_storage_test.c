/* SPDX-License-Identifier: GPL-3.0-only */
/* Exercise the bounded flash allocation and the real boot/track paths. */
#define STUDIO_STORAGE_TEST_INCLUDE
#include "backup_test.c"
#include <assert.h>
static uint8_t fixture[0x100000],expected[TESS_FULL_BYTES];
static void verify_migrated(void)
{
    uint8_t want[TESS_FULL_BYTES],got[TESS_FULL_BYTES];
    for(uint32_t slot=0;slot<4u;slot++){
        tessera_reset();project_load(slot);tess_full_pack(got);memcpy(want,expected,sizeof want);want[TESSERA_PK_BYTES+7u]=(uint8_t)(20u+slot);
        uint8_t loaded[TESS_FULL_BYTES];memcpy(loaded,got,sizeof loaded);
        assert(tess_full_unpack(want,sizeof want));tess_full_pack(want);
        assert(proj_cur==slot && !memcmp(want,loaded,sizeof loaded));
        assert(song.g[G_BPM]==97 && trk[0].step[0].note[0]==62u);
    }
}
static void migration(const char *nor_path,const char *state_path)
{
    FILE *f=fopen(nor_path,"rb");assert(f&&fread(fixture,1,sizeof fixture,f)==sizeof fixture);fclose(f);
    f=fopen(state_path,"rb");assert(f&&fread(expected,1,sizeof expected,f)==sizeof expected);fclose(f);
    const uint32_t cuts[]={1,2,5,50,109,110,111,220,330,440,441,448,450};
    for(uint32_t trial=0;trial<NELEM(cuts)+1u;trial++){
        reset();flash_ok=1;memcpy(nor,fixture,sizeof nor);
        uint8_t old_sidecar[TESS_OBJ_BYTES];int old_n=st_load(OBJ_SHARED_EXT,old_sidecar,sizeof old_sidecar);assert(old_n==TESS_OBJ_BYTES);
        fail_prog_call=trial?cuts[trial-1u]:0;prog_calls=0;
        int failed=tess_migrate_legacy();
        if(failed){uint8_t kept[TESS_OBJ_BYTES];assert(st_load(OBJ_SHARED_EXT,kept,sizeof kept)==old_n && !memcmp(kept,old_sidecar,sizeof kept));}
        fail_prog_call=0;assert(!tess_migrate_legacy());verify_migrated();
        uint32_t before=erases;assert(!tess_migrate_legacy()&&erases==before);
    }
    reset();flash_ok=1;memcpy(nor,fixture,sizeof nor);erase_error=1;
    assert(tess_migrate_legacy() && !memcmp(nor+0x9D000u,fixture+0x9D000u,0x2000u));
    erase_error=0;assert(!tess_migrate_legacy());verify_migrated();
    puts("studio: real .3 NOR migrates all four full feature blocks; interrupted page/header/erase writes preserve the sidecar and retry; completed boot is read-only");
}
static void compact_and_tracks(void)
{
    reset();flash_ok=1;song.ntrk=4;
    for(uint32_t k=0;k<NTRK;k++){
        song.sel=(uint8_t)k;tessera_knob(0,(int32_t)k+1);
        harm_track[k]=(harm_track_t){HARM_KEY,20+k,100+k,5+k,60,70,1,9+k};
        for(uint32_t b=0;b<NPAT;b++){
            pattern_t *p=pattern_at(k,b);p->timing[0]=1u+(k*17u+b*7u)%64u;p->timing[1]=(int16_t)((k+b)%NELEM(N_DIV));p->timing[2]=(k*23u+b)%101u;p->timing[3]=1u+(k*29u+b)%100u;
            p->step[0]=(step_t){{(uint8_t)(36u+k*12u+b)},1,ST_NOTE,0,100};
        }
        memcpy(trk[k].p+P_SLEN,pattern_at(k,0)->timing,4u*sizeof(int16_t));memcpy(trk[k].step,pattern_at(k,0)->step,sizeof trk[k].step);
    }
    for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<NTRK;k++)chain_patterns[r][k]=(uint8_t)((r+k)%NPAT);
    for(uint32_t k=0;k<NTRK;k++){performance[k].attack=20+k;performance[k].amp=1;performance[k].mode=DUR_FIXED;}
    song.sel=0;uint8_t state[TESS_FULL_BYTES];tess_full_pack(state);
    assert(!project_save_as(0,"COMPACT"));project_load(0);
    uint8_t got[TESS_FULL_BYTES];tess_full_pack(got);assert(!memcmp(state,got,sizeof got));
    for(uint32_t k=0;k<NTRK;k++)for(uint32_t b=0;b<NPAT;b++){
        pattern_t *p=pattern_at(k,b);assert(p->timing[0]==1u+(k*17u+b*7u)%64u&&p->timing[1]==(k+b)%NELEM(N_DIV)&&p->timing[2]==(k*23u+b)%101u&&p->timing[3]==1u+(k*29u+b)%100u);
        assert(p->step[0].note[0]==36u+k*12u+b);
    }
    for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<NTRK;k++)assert(chain_patterns[r][k]==(r+k)%NPAT);
    assert(!project_rename(0,"RENAMED"));project_load(0);tess_full_pack(got);assert(!memcmp(state,got,sizeof got));
    assert(!track_remove(1));assert(trk_n()==3&&tessera.mode[1]==TESS_ENG_MARKOV&&tessera.mode[2]==TESS_ENG_MORPH&&harm_track[1].channel==11&&harm_track[2].channel==12);
    assert(performance[1].attack==22 && performance[2].attack==23 && !performance[3].amp);
    assert(!tessera.mode[3]&&!harm_track[3].midi);
    track_add();assert(trk_n()==4&&!tessera.mode[3]&&!harm_track[3].midi);
    puts("studio: all 32 banks retain distinct notes/timings and SONG assignments; rename retains features; track removal shifts generator/routing ownership and add resets only its new track");
}
int main(int argc,char **argv)
{
    compact_and_tracks();
    if(argc==3)migration(argv[1],argv[2]);
    return 0;
}
