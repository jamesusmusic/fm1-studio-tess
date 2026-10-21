/* SPDX-License-Identifier: GPL-3.0-only
 * Wavetable integration, persistence and spectral fixtures — By: Inputsignwave. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
static void fresh(void)
{
    eng_state_reset();reset();memset(resource,0,sizeof resource);memset(fx_state,0,sizeof fx_state);
    memset(mix_cache,0,sizeof mix_cache);memset(&mod,0,sizeof mod);
    rng_state=0x12345678u;mod_seed=0x2545f491u;vage=voice_kills=0;track_solo=0;panic_req=0;
    for(uint32_t i=0;i<NPART;i++)solo_gain[i]=32768;
    lim_env=LIM_T;dc_l=dc_r=dce_l=dce_r=0;
    fx_bus_mix=fx_bus_target=fx_bus_pending=fx_bus_result=0;fx_epoch++;
    resource_peak=resource_failures=0;host_preset(&trk[0],ENGI_WAVETABLE,0);
}
static void bank_tests(void)
{
    assert(sizeof WT_FLASH==12384u && eng_ok(21) && !eng_ok(20));
    assert(ENGINE_ORDER[2]==21 && ENG_WAVETABLE.npresets==6);
    assert(wt_prepare());const void *shared=wt_bank;
    for(uint32_t p=0;p<4;p++){
        host_preset(&trk[p],ENGI_WAVETABLE,p);
        assert(eng_state_prepare(&trk[p]) && wt_bank==shared && !resource[RES_ENGINE0+p].size);
    }
    assert(resource_used()==sizeof WT_FLASH);
    for(uint32_t f=0;f<4;f++)for(uint32_t d=0;d<3;d++)for(uint32_t b=0;b<4;b++){
        const int16_t *row=WT_FLASH[f][d][b];assert(row[0]==row[WT_SAMPLES]);
        int sum=0;for(uint32_t i=0;i<WT_SAMPLES;i++){assert(abs(row[i])<=14000);sum+=row[i];}assert(abs(sum)<=128);
        for(uint32_t h=WT_HARMONICS[b]+1;h<=64;h++){
            double re=0,im=0;
            for(uint32_t i=0;i<WT_SAMPLES;i++){double a=6.283185307179586*h*i/WT_SAMPLES;re+=row[i]*cos(a);im+=row[i]*sin(a);}
            assert(hypot(re,im)/WT_SAMPLES<1.0); /* Only int16 rounding noise beyond the selected harmonic limit. */
        }
    }
    trk[3].v[0].active=1;wt_cache_sweep();assert(wt_bank==shared);
    trk[3].v[0].active=0;wt_cache_sweep();assert(!wt_bank && !resource_used());
    puts("wavetable: stable ID 21, periodic bounded mip tables, single 12384-byte shared bank, idle release: OK");
}
static void corners(void)
{
    fresh();assert(wt_prepare());
    for(uint32_t note=0;note<128;note++)for(uint32_t corner=0;corner<256;corner++){
        for(uint32_t k=0;k<8;k++){const param_desc_t *d=&ENG_WAVETABLE.edit[k];trk[0].p[P_E0+k]=(corner&(1u<<k))?d->max:d->min;}
        voice_t v={0};wt_note_on(&trk[0],&v);
        vmod_t m={0};m.pitch16=note*16;m.shape=64*256;
        for(uint32_t j=0;j<3;j++){
            int32_t out[CTL+2];memset(out,0,sizeof out);out[0]=out[CTL+1]=0x123456;
            wt_render(&trk[0],&v,out+1,j==0?0:j==1?1:CTL,&m);
            assert(out[0]==0x123456 && out[CTL+1]==0x123456);
            for(uint32_t i=1;i<=CTL;i++)assert(abs(out[i])<=28000);
        }
    }
    eng_state_reset();memset(resource,0,sizeof resource);
    /* Occupy every bank with live data. Cache is disabled in this profile. */
    uint32_t handle=0;
    for(uint32_t b=0;b<RESOURCE_BANKS;b++)if(b!=RESOURCE_CACHE_BANK){
        resource[handle].ptr=resource_banks[b].start;resource[handle].size=(uint32_t)(resource_banks[b].end-resource_banks[b].start);
        memset(resource[handle].ptr,0xa5,resource[handle].size);handle++;
    }
    assert(!wt_prepare() && !wt_bank && resource_failures);
    int32_t out[CTL]={0};voice_t v={0};vmod_t m={0};wt_render(&trk[0],&v,out,CTL,&m);
    for(uint32_t i=0;i<CTL;i++)assert(!out[i]);
    for(uint32_t h=0;h<handle;h++)for(uint32_t i=0;i<resource[h].size;i++)assert(((uint8_t *)resource[h].ptr)[i]==0xa5);
    memset(resource,0,sizeof resource);
    puts("wavetable: 32768 pitch/control corners, short/zero blocks, output guards, allocation-failure silence: OK");
}
static void storage(void)
{
    fresh();int16_t vals[8]={90,110,29,57,100,65,-19,-37};
    memcpy(trk[0].p+P_E0,vals,sizeof vals);assert(!project_save(0));
    host_preset(&trk[0],ENGI_FM6,0);project_load(0);
    assert(trk[0].eng_req==ENGI_WAVETABLE && !memcmp(trk[0].p+P_E0,vals,sizeof vals));
    assert(!up_store_to(0,0,"WAVE TEST") && up_valid(up_rec(0)));
    up_cache_reset();host_preset(&trk[0],ENGI_FM6,0);assert(!up_load_to(0,0));
    assert(trk[0].eng_req==ENGI_WAVETABLE && !memcmp(trk[0].p+P_E0,vals,sizeof vals));
    memset(&favorites,0xa5,sizeof favorites);uint8_t before[sizeof favorites];memcpy(before,&favorites,sizeof favorites);
    for(uint32_t i=0;i<6;i++){assert(favorite_set(ENGI_WAVETABLE,i,1));assert(favorite_has(ENGI_WAVETABLE,i));}
    assert(!favorite_set(ENGI_WAVETABLE,6,1));
    memcpy(before+8*32+12,favorites.factory[8]+12,4);assert(!memcmp(before,&favorites,sizeof favorites));
    song.playing=0;transport_req=0;settings_save();
    assert(!persist_pending);memset(&favorites,0,sizeof favorites);settings_import(&persist_saved,sizeof persist_saved);
    for(uint32_t i=0;i<6;i++)assert(favorite_has(ENGI_WAVETABLE,i));
    puts("wavetable: project and user-preset signed controls round-trip, six stars persist, legacy bytes preserved: OK");
}
static uint64_t render_preset(uint32_t p)
{
    fresh();host_preset(&trk[0],ENGI_WAVETABLE,p);trk[0].p[P_VOICE]=V_POLY;
    for(uint32_t k=0;k<4;k++)trk_note_on(&trk[0],48+k*7,100);
    uint64_t h=1469598103934665603ull,energy=0;int32_t out[CTL*2];
    for(uint32_t b=0;b<FS*4/CTL;b++){
        if(b==FS/CTL)for(uint32_t k=0;k<4;k++)trk_note_off(&trk[0],48+k*7);
        mix_block(out,CTL);for(uint32_t i=0;i<CTL*2;i++){h=(h^(uint32_t)out[i])*1099511628211ull;energy+=abs(out[i]);}
    }
    assert(energy && !resource_failures);
    for(uint32_t i=0;i<NVOICE;i++)assert(!trk[0].v[i].active);
    return h;
}
static void spectral(const char *dir)
{
    fresh();assert(wt_prepare());
    const uint32_t bins[]={409,1857};
    for(uint32_t c=0;c<2;c++){
        char name[512];snprintf(name,sizeof name,"%s/spectral-%u.txt",dir,bins[c]);FILE *f=fopen(name,"w");assert(f);
        uint32_t inc=bins[c]*(0x100000000ull/16384u),pos=wt_band_position(inc),b=pos>>16,bf=pos&65535;
        if(b>=3){b=2;bf=65536;}
        uint32_t ph=0;
        for(uint32_t i=0;i<16384;i++){
            int x=wt_lerp(wt_face(2,1,b,ph,65536,65536),wt_face(2,1,b+1,ph,65536,65536),bf);
            fprintf(f,"%d\n",x);ph+=inc;
        }
        fclose(f);
    }
}
int main(int argc,char **argv)
{
    fresh();bank_tests();corners();storage();
    for(uint32_t p=0;p<6;p++){uint64_t a=render_preset(p),b=render_preset(p);assert(a==b);}
    puts("wavetable: all six chord presets repeat bit-for-bit and release every voice: OK");
    if(argc>1)spectral(argv[1]);
    return 0;
}
