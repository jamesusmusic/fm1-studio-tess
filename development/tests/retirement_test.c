/* SPDX-License-Identifier: GPL-3.0-only
 * Removal-profile compatibility and resource tests — By: Inputsignwave. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
static uint64_t pcm_hash(uint32_t e,uint32_t pi,uint32_t fx)
{
    reset();memset(resource,0,sizeof resource);memset(fx_state,0,sizeof fx_state);
    resource_peak=resource_failures=0;
    host_preset(&trk[0],e,pi);trk[0].p[P_FXTYPE]=(int16_t)fx;
    trk[0].p[P_FXMIX]=fx?64:0;trk[0].p[P_FXRATE]=60;trk[0].p[P_FXCHAR]=64;
    for(uint32_t i=0;i<4;i++)trk_note_on(&trk[0],48+i*7,100);
    uint64_t h=1469598103934665603ull;int32_t b[CTL*2];
    for(uint32_t j=0;j<FS/CTL;j++){
        if(j==FS/CTL/2)for(uint32_t i=0;i<4;i++)trk_note_off(&trk[0],48+i*7);
        mix_block(b,CTL);
        for(uint32_t k=0;k<CTL*2;k++)h=(h^(uint32_t)b[k])*1099511628211ull;
    }
    return h;
}
static void parity(void)
{
    const uint32_t engines[]={ENGI_PROPHET,ENGI_FM6,ENGI_DRUM};
    for(uint32_t k=0;k<3;k++){
        uint32_t e=engines[k],np=e==ENGI_DRUM?2u:ENGINES[e]->npresets;
        for(uint32_t pi=0;pi<np;pi++)printf("preset %u %u %016llx\n",e,pi,(unsigned long long)pcm_hash(e,pi,0));
    }
    for(uint32_t fx=0;fx<FX_COUNT;fx++)if(fx!=FX_ROOM && fx!=FX_REVERB)
        printf("fx %u %016llx\n",fx,(unsigned long long)pcm_hash(ENGI_FM6,4,fx));
}
static void legacy(void)
{
    reset();cz_init();
    for(uint32_t k=0;k<NPART;k++){
        trk[k].eng_req=trk[k].engine=k==0?3u:k==1?ENGI_CZ:ENGI_DRUM;
        trk[k].p[P_E0]=k>=2?DK_SID:3;
        trk[k].p[P_E7]=k==1?CZ_NATIVE:91;
        trk[k].p[P_FXTYPE]=FX_ROOM;trk[k].p[P_FX2TYPE]=FX_REVERB;
        trk[k].p[P_FXMIX]=100;trk[k].p[P_FX2MIX]=127;
        cz_patch[k].raw[128]=(uint8_t)('A'+k);
    }
    uint8_t raw[sizeof cz_patch];memcpy(raw,cz_patch,sizeof raw);
    assert(!project_save(0));
    /* Keep simulated NOR intact across reload. */
    memset(trk,0,sizeof trk);cz_init();project_load(0);
    assert(trk[0].eng_req==3u && trk[1].eng_req==ENGI_CZ);
    assert(trk[0].p[P_E7]==91 && trk[1].p[P_E7]==CZ_NATIVE);
    assert(trk[2].p[P_E0]==DK_SID && trk[3].p[P_E0]==DK_SID);
    assert(!memcmp(raw,cz_patch,sizeof raw));
    for(uint32_t k=0;k<NPART;k++)assert(trk[k].p[P_FXTYPE]==FX_ROOM && trk[k].p[P_FX2TYPE]==FX_REVERB);
    assert(!project_save(1));project_load(1);assert(!memcmp(raw,cz_patch,sizeof raw));
    memset(resource,0,sizeof resource);resource_peak=resource_failures=0;
    for(uint32_t k=0;k<NPART;k++){
        trk[k].engine=trk[k].eng_req;
        trk_note_on(&trk[k],60,100);
    }
    int32_t b[CTL*2];
    for(uint32_t j=0;j<100;j++){mix_block(b,CTL);for(uint32_t i=0;i<CTL*2;i++)assert(!b[i]);}
    assert(!resource_used() && !resource_failures);
    puts("production: retired SID/CZ/kits silent, no engine/FX allocations, saved IDs and raw CZ data round-trip");
}
static void bypass(void)
{
    for(uint32_t type=FX_ROOM;type<=FX_REVERB;type++){
        reset();memset(resource,0,sizeof resource);memset(fx_state,0,sizeof fx_state);
        track_t *t=&trk[0];t->p[P_FXTYPE]=type;t->p[P_FXMIX]=127;
        int32_t b[CTL],before[CTL];for(uint32_t i=0;i<CTL;i++)b[i]=before[i]=(int32_t)i*123-1700;
        for(uint32_t j=0;j<4;j++){
            assert(!fx_process(t,b,CTL));assert(!memcmp(b,before,sizeof b));
            fx_plan_t plan;fx_slot_prepare(&plan,NPART*FX_SLOTS,t->p+P_FXTYPE);
            assert(!plan.active && !resource[RES_FX0+NPART*FX_SLOTS].size);
        }
    }
    puts("production: retired ROOM/REVERB dry bypass and shared-slot no-allocation passed");
}
static void stress(void)
{
    const uint32_t e[]={ENGI_PROPHET,ENGI_FM6,ENGI_DRUM,ENGI_DRUM};
    uint32_t peak=0,failed=0;
    for(uint32_t scenario=0;scenario<256;scenario++){
        reset();memset(resource,0,sizeof resource);memset(fx_state,0,sizeof fx_state);
        resource_peak=resource_failures=0;
        for(uint32_t k=0;k<4;k++){
            uint32_t choice=(scenario>>(k*2))&3;
            host_preset(&trk[k],e[choice],choice==3?1u:0u);
            trk_note_on(&trk[k],e[choice]==ENGI_DRUM?36:60,100);
        }
        int32_t b[CTL*2];for(uint32_t j=0;j<32;j++)mix_block(b,CTL);
        if(resource_peak>peak)peak=resource_peak;
        failed+=resource_failures;

    }
    printf("production: 256 four-track engine/kit combinations, host allocation peak %u bytes, failures %u\n",peak,failed);
    assert(!failed);
    printf("production: host sizes Prophet=%zu FM6=%zu 808=%zu 909=%zu FXstate=%zu; target sizes reported separately\n",sizeof(p5_part_t),sizeof(fm6_part_t),sizeof(drum_lane_t)*DV_NLANE,sizeof(drum909_part_t),sizeof(fx_state));
}
int main(int argc,char **argv)
{
    if(argc>1){parity();return 0;}
    assert(!MELODEE_RETIRED_REFERENCE);
    assert(NENG_SHOWN==3 && !eng_ok(3) && !eng_ok(ENGI_CZ));
    for(uint32_t k=0;k<NENG_SHOWN;k++)assert(eng_vis(k)==(k==0?ENGI_PROPHET:k==1?ENGI_FM6:ENGI_DRUM));
    assert(!eng_state_size(3) && !eng_state_size(ENGI_CZ));
    assert(ENG_DRUM.npresets==2 && TRK_DEF[2][0]==ENGI_FM6);
    legacy();bypass();stress();return 0;
}
