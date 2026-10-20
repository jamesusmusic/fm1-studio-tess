/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio) */
/* A rejected descriptor must never merge stale PCM or use unchecked bounds. */
#define main hostsim_main
#include "hostsim.c"
#undef main
#include <assert.h>

static uint32_t calls;
static void sample_job(const void *data,int32_t *pcm)
{
    (void)data;calls++;
    for(uint32_t i=0;i<CTL;i++)pcm[i]=7;
}
int main(void)
{
    int32_t out[CTL+2];
    uint32_t data=0;
    for(uint32_t bad=0;bad<5;bad++){
        for(uint32_t i=0;i<CTL+2;i++)out[i]=123;
        audio_batch_count=0;
        audio_batch_push(sample_job,&data,sizeof data,out,CTL,AP_FM6);
        if(!bad)audio_batch_jobs[0].guard^=1u;
        else if(bad==1)audio_batch_jobs[0].n=CTL+1u;
        else if(bad==2)audio_batch_jobs[0].profile_id=AP_COUNT;
        else if(bad==3)audio_batch_jobs[0].run=0;
        else audio_batch_jobs[0].out=out+1;
        for(uint32_t i=0;i<CTL;i++)audio_batch_jobs[0].pcm[i]=999;
        audio_batch_flush();
        assert(!calls);
        for(uint32_t i=0;i<CTL+2;i++)assert(out[i]==123);
    }
    audio_batch_push(sample_job,&data,sizeof data,out,CTL,AP_FM6);
    audio_batch_flush();assert(calls==1u);
    for(uint32_t i=0;i<CTL;i++)assert(out[i]==130);
    assert(out[CTL]==123 && out[CTL+1u]==123);
    audio_batch_push(sample_job,&data,AUDIO_BATCH_BYTES+1u,out,CTL,AP_FM6);
    assert(!audio_batch_count && calls==1u);
    float sum[CTL];int32_t pcm[CTL];vmod_t m={0};m.amp0=m.amp1=32767;
    for(uint32_t i=0;i<CTL;i++)sum[i]=i%3u==0u?NAN:i%3u==1u?INFINITY:-INFINITY;
    fm6_voice_t voice={0};
    fm6_output(&voice,sum,pcm,CTL,&m,VOICE_FS,0);
    for(uint32_t i=0;i<CTL;i++)assert(i%3u==0u?pcm[i]==0:i%3u==1u?pcm[i]>0:pcm[i]<0);
    for(int32_t x=-2000;x<=2000;x++){
        float v=(float)x/100.0f,expected=v < -16.0f?-16.0f:v > 16.0f?16.0f:v;
        assert(fm6_output_clip(v)==expected);
    }
    puts("Audio batch safety: rejected serial descriptors never merge stale PCM or unchecked lengths: OK");
    return 0;
}
