/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio) */
/* One-track FM6, actual transport STOP, inserts/shared tails and CPU1 parity. */
#define MELODEE_DUAL_CORE_TEST_NO_MAIN 1
#include "dual_core_test.c"
#define TAIL_BLOCKS (FS*8u/CTL)
static int32_t tail_reference[TAIL_BLOCKS*CTL*2u];

static void tail_case(uint32_t mode,uint32_t fx,uint32_t voices,int parallel)
{
    seq_stop();setup_case(7,mode,0);tessera_reset();recording_reset();
    song.ntrk=0;audio_worker_online=1;serial_render=!parallel;
    memset(fx_state,0,sizeof fx_state);fx_epoch++;
    fx_bus_mix=fx_bus_target=fx_bus_pending=fx_bus_result=0;
    for(uint32_t i=G_MFXTYPE;i<G_COUNT;i++)song.g[i]=GP[i].def;
    track_t *t=&trk[0];
    t->p[P_LEVEL]=voices==1u?80:55;t->p[P_SLEN]=16;t->p[P_SDIV]=2;
    t->p[P_M1AMT]=t->p[P_M2AMT]=0;
    t->p[P_FXTYPE]=fx;t->p[P_FXMIX]=fx?80:0;t->p[P_FXRATE]=60;t->p[P_FXCHAR]=85;
    if(fx){song.g[G_MFXTYPE]=FX_REVERB;song.g[G_MFXMIX]=127;song.g[G_MFXRATE]=90;
        song.g[G_MFXCHAR]=90;song.g[G_FXRETURN]=80;t->p[P_FXSEND]=70;}
    for(uint32_t i=0;i<16u;i+=4u){
        t->step[i].time=ST_NOTE;
        t->step[i].n=(uint8_t)(voices>4u?4u:voices);
        for(uint32_t k=0;k<t->step[i].n;k++)t->step[i].note[k]=(uint8_t)(48u+4u*k);
        t->step[i].vel=110;
    }
    seq_start();uint64_t tail_energy=0;uint32_t peak=0;
    for(uint32_t b=0;b<TAIL_BLOCKS;b++){
        if(b==FS/CTL){
            seq_stop();
            for(uint32_t k=0;k<NVOICE;k++)assert(!t->v[k].gate);
        }
        if(b<FS/CTL && voices>4u && b%128u==0u){
            for(uint32_t k=0;k<voices;k++)trk_note_on(t,60u+k,100u);
            for(uint32_t k=0;k<voices;k++)trk_note_off(t,60u+k);
        }
        int32_t pcm[CTL*2];mix_block(pcm,CTL);
        assert(!audio_batch_count && !fx_bus_pending && !audio_worker.timeouts && !resource_failures);
        for(uint32_t i=0;i<CTL*2u;i++){
            int32_t a=pcm[i]<0?-pcm[i]:pcm[i];assert(a<=32767);
            if((uint32_t)a>peak)peak=(uint32_t)a;
            if(b>FS/CTL && b<FS*2u/CTL)tail_energy+=(uint32_t)a;
            if(b>FS*7u/CTL && !fx)assert(a<=1);
        }
        if(!parallel)memcpy(tail_reference+b*CTL*2u,pcm,sizeof pcm);
        else assert(!memcmp(tail_reference+b*CTL*2u,pcm,sizeof pcm));
    }
    assert(peak>100u && (!fx || tail_energy>0));
    for(uint32_t k=0;k<NVOICE;k++)assert(!t->v[k].active);
    printf("FM6 tail: mode %u FX %u density %u %s, peak %u tail energy %llu: OK\n",
        mode,fx,voices,parallel?"dual":"serial reference",peak,(unsigned long long)tail_energy);
}
int main(void)
{
    pthread_t thread;assert(!pthread_create(&thread,0,worker_thread,0));
    while(!AW_LOAD(&audio_worker.ready))sched_yield();
    const uint32_t fx[3]={FX_OFF,FX_ECHO,FX_REVERB};
    for(uint32_t mode=0;mode<3u;mode++)for(uint32_t f=0;f<3u;f++)for(uint32_t v=1;v<=8;v+=7u){
        tail_case(mode,fx[f],v,0);tail_case(mode,fx[f],v,1);
    }
    assert(!pthread_cancel(thread));assert(!pthread_join(thread,0));
    puts("FM6: 18 one-track scenarios, 8 seconds each, STOP releases, dry/insert/shared tails, serial/dual identical: OK");
    return 0;
}
