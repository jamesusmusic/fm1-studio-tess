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
    assert(!memcmp(wt_bank,&WT_FLASH,sizeof WT_FLASH)); /* every reconstructed sample */
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
        vmod_t m={0};m.pitch16=note*16;m.shape=64*256;m.amp0=m.amp1=32767;
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
/* Bound host allocation conservatively below Wave1's disjoint target banks:
 * 60 KiB main, and a smaller host data tail (2048 vs 3120), without cache. */
static void target_capacity(void)
{
    const uint32_t caps[5]={3120,784,4260,0,61440};
    const uint32_t engines[5]={ENGI_PROPHET,ENGI_FM6,ENGI_DRUM,ENGI_DRUM,ENGI_WAVETABLE};
    uint32_t peak=0;
    for(uint32_t scenario=0;scenario<625;scenario++){
        fresh();uint32_t held=0,handle=RES_FX0,choices=scenario;
        for(uint32_t b=0;b<RESOURCE_BANKS;b++)if(b!=RESOURCE_CACHE_BANK){
            uint32_t len=(uint32_t)(resource_banks[b].end-resource_banks[b].start);
            uint32_t cap=caps[b]&~7u;if(cap>len)cap=len;
            resource[handle].ptr=resource_banks[b].start+cap;resource[handle].size=len-cap;
            held+=len-cap;handle++;
        }
        for(uint32_t p=0;p<4;p++){
            uint32_t c=choices%5u;choices/=5u;host_preset(&trk[p],engines[c],c==3?1u:0u);
            assert(eng_state_prepare(&trk[p]));
        }
        uint32_t used=resource_used()-held;if(used>peak)peak=used;
        assert(!resource_failures);
    }
    printf("wavetable: 625 four-track allocations fit target banks without cache; peak live host state %u B: OK\n",peak);
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
/* Musical contracts are checked against measured output, not oscillator internals. */
static uint32_t frequency(int semi,int fine,int a4)
{
    fresh();tuning_a4=a4;trk[0].p[P_E6]=semi;trk[0].p[P_E7]=fine;assert(wt_prepare());
    voice_t v={0};wt_note_on(&trk[0],&v);vmod_t m={0};m.pitch16=69*16;m.shape=64*256;m.amp0=m.amp1=32767;
    uint32_t crossings=0;int32_t previous=0;
    for(uint32_t at=0;at<FS;at+=CTL){
        uint32_t n=FS-at<CTL?FS-at:CTL;int32_t out[CTL]={0};wt_render(&trk[0],&v,out,n,&m);
        for(uint32_t k=0;k<n;k++){crossings+=previous<=0 && out[k]>0;previous=out[k];}
    }
    tuning_a4=A4_DEFAULT;return crossings;
}
static void wt_performance_contract(void)
{
    assert(abs((int)frequency(0,0,440)-440)<=1);
    assert(abs((int)frequency(12,0,440)-880)<=1);
    assert(abs((int)frequency(-12,0,440)-220)<=1);
    assert(abs((int)frequency(0,50,440)-453)<=1);
    assert(abs((int)frequency(0,-50,440)-427)<=1);
    assert(abs((int)frequency(0,0,432)-432)<=1);
    fresh();midi_parameter(&trk[0],74,39);midi_parameter(&trk[0],71,91);
    assert(trk[0].p[P_E4]==39 && trk[0].p[P_E1]==91);
    trk[0].p[P_E6]=-17;trk[0].p[P_E7]=-31;template_save();assert(!tmpl_dirty);
    host_preset(&trk[0],ENGI_FM6,0);template_load();
    assert(trk[0].eng_req==ENGI_WAVETABLE && trk[0].p[P_E6]==-17 && trk[0].p[P_E7]==-31);
    for(uint32_t mode=V_POLY;mode<=V_UNISON;mode++){
        fresh();trk[0].p[P_VOICE]=mode;trk[0].p[P_ATK]=trk[0].p[P_DEC]=trk[0].p[P_REL]=0;
        trk[0].p[P_SUS]=127;trk_note_on(&trk[0],60,100);trk_note_on(&trk[0],64,100);
        int32_t out[CTL*2];uint64_t energy=0;
        for(uint32_t b=0;b<16;b++){mix_block(out,CTL);for(uint32_t k=0;k<CTL*2;k++)energy+=abs(out[k]);}
        assert(energy && wt_bank);trk_note_off(&trk[0],60);trk_note_off(&trk[0],64);
        for(uint32_t b=0;b<100;b++)mix_block(out,CTL);
        for(uint32_t v=0;v<NVOICE;v++)assert(!trk[0].v[v].active);
        assert(!wt_bank && !resource[RES_WAVETABLE].size && !resource_failures);
    }
    fresh();trk_note_on(&trk[0],60,100);int32_t out[CTL*2];mix_block(out,CTL);assert(wt_bank);
    for(uint32_t round=0;round<20;round++){
        set_engine_of(&trk[0],ENGI_FM6);
        for(uint32_t b=0;b<16;b++)mix_block(out,CTL);
        assert(trk[0].engine==ENGI_FM6 && !wt_bank);
        trk_note_on(&trk[0],60,100);mix_block(out,CTL);assert(resource[RES_ENGINE0].size);
        set_engine_of(&trk[0],ENGI_WAVETABLE);
        for(uint32_t b=0;b<16;b++)mix_block(out,CTL);
        assert(trk[0].engine==ENGI_WAVETABLE && !resource[RES_ENGINE0].size);
        trk_note_on(&trk[0],64,100);mix_block(out,CTL);assert(wt_bank && !resource_failures);
    }
    /* Scan at zero depth must remain inaudible at every rate. */
    fresh();assert(wt_prepare());voice_t a={0},b={0};wt_note_on(&trk[0],&a);wt_note_on(&trk[0],&b);
    vmod_t m={0};m.pitch16=60*16;m.shape=64*256;m.amp0=m.amp1=32767;
    for(uint32_t j=0;j<100;j++){
        int32_t x[CTL]={0},y[CTL]={0};trk[0].p[P_E2]=0;wt_render(&trk[0],&a,x,CTL,&m);
        trk[0].p[P_E2]=127;wt_render(&trk[0],&b,y,CTL,&m);assert(!memcmp(x,y,sizeof x));
    }
    puts("wavetable: measured pitch/A4/fine tune, MIDI tone/fold, template recall, all voice modes, 40 held-note engine switches and scan OFF neutrality: OK");
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
/* Audible contracts: zero gain really is silent, and note edges follow the
 * shared sample ramp even when the oscillator is at its peak. */
static void amplitude_edges(void)
{
    fresh();assert(wt_prepare());
    for(uint32_t edge=0;edge<3;edge++){
        voice_t v={0};wt_note_on(&trk[0],&v);v.ph[0]=0x40000000u;
        vmod_t m={0};m.pitch16=0;m.shape=64*256;
        m.amp0=edge==2?32767:0;m.amp1=edge==1?32767:0;
        int32_t out[CTL]={0};wt_render(&trk[0],&v,out,CTL,&m);
        if(!edge)for(uint32_t i=0;i<CTL;i++)assert(!out[i]);
        if(edge==1){assert(!out[0]);assert(out[CTL-1]>1000);}
        if(edge==2){assert(out[0]>1000);assert(abs(out[CTL-1])<1000);}
    }
    puts("wavetable: silent gain, zero-start attack and near-zero release edge at oscillator peak: OK");
}
static uint64_t envelope_energy(uint32_t preset,uint32_t vel,int attack,int release,uint32_t start_ms,uint32_t end_ms,uint32_t off_ms)
{
    fresh();host_preset(&trk[0],ENGI_WAVETABLE,preset);
    if(attack>=0)trk[0].p[P_ATK]=(int16_t)attack;
    if(release>=0)trk[0].p[P_REL]=(int16_t)release;
    trk[0].p[P_VOICE]=V_POLY;trk_note_on(&trk[0],60,vel);
    uint64_t energy=0;uint32_t off=0;int32_t out[CTL*2];
    for(uint32_t at=0;at<FS*end_ms/1000;at+=CTL){
        if(!off && at>=FS*off_ms/1000){trk_note_off(&trk[0],60);off=1;}
        mix_block(out,CTL);
        for(uint32_t i=0;i<CTL && at+i<FS*end_ms/1000;i++)
            if(at+i>=FS*start_ms/1000)energy+=(uint32_t)abs(out[i*2]);
    }
    return energy;
}
static void musical_envelopes(void)
{
    uint64_t fast=envelope_energy(0,127,0,-1,0,40,1000);
    uint64_t slow=envelope_energy(0,127,65,-1,0,40,1000);
    uint64_t quiet=envelope_energy(0,25,0,-1,100,200,1000);
    uint64_t loud=envelope_energy(0,127,0,-1,100,200,1000);
    uint64_t short_rel=envelope_energy(0,127,0,0,300,400,100);
    uint64_t long_rel=envelope_energy(0,127,0,100,300,400,100);
    uint64_t pluck_early=envelope_energy(5,127,-1,-1,0,100,1000);
    uint64_t pluck_late=envelope_energy(5,127,-1,-1,700,800,1000);
    printf("wavetable envelopes: attack fast/slow %llu/%llu; velocity loud/quiet %llu/%llu; release long/short %llu/%llu; pluck early/late %llu/%llu\n",
        (unsigned long long)fast,(unsigned long long)slow,(unsigned long long)loud,(unsigned long long)quiet,
        (unsigned long long)long_rel,(unsigned long long)short_rel,(unsigned long long)pluck_early,(unsigned long long)pluck_late);
    assert(fast>1000 && slow<fast/3 && quiet<loud/3);
    assert(long_rel>1000 && short_rel<long_rel/20);
    assert(pluck_early>1000 && pluck_late<pluck_early/4);
    /* Same-note retrigger must continue the sounding oscillator/filter output,
     * compared with a copy left running (not merely compare state members). */
    fresh();trk_note_on(&trk[0],60,100);int32_t mixed[CTL*2];
    for(uint32_t k=0;k<16;k++)mix_block(mixed,CTL);
    voice_t *live=0;for(uint32_t k=0;k<NVOICE;k++)if(trk[0].v[k].active)live=&trk[0].v[k];
    assert(live);voice_t continued=*live;
    trk_note_on(&trk[0],60,100);
    vmod_t m={0};m.pitch16=60*16;m.shape=64*256;m.amp0=m.amp1=32767;
    int32_t a[CTL]={0},b[CTL]={0};
    wt_render(&trk[0],&continued,a,CTL,&m);wt_render(&trk[0],live,b,CTL,&m);
    assert(!memcmp(a,b,sizeof a));
    puts("wavetable: audible attack/release/velocity and pluck decay, sounding retrigger continuity: OK");
}
int main(int argc,char **argv)
{
    amplitude_edges();musical_envelopes();
    fresh();bank_tests();corners();storage();target_capacity();wt_performance_contract();
    for(uint32_t p=0;p<6;p++){uint64_t a=render_preset(p),b=render_preset(p);assert(a==b);}
    puts("wavetable: all six chord presets repeat bit-for-bit and release every voice: OK");
    if(argc>1)spectral(argv[1]);
    return 0;
}
