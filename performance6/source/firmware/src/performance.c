/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio) */
static void performance_start(track_t *t, voice_t *v)
{
    performance_voice_t *s=&performance_voice[t-trk][v-t->v];
    *s=(performance_voice_t){0,32767,32767,0,0,1};
}
static int32_t performance_move(int32_t from,int32_t to,uint32_t ms)
{
    if(!ms)return to;
    int32_t step=(int32_t)((32767u*CTL*1000u/FS)/ms);
    if(step<1)step=1;
    return from<to ? (to-from<step?to:from+step) : (from-to<step?to:from-step);
}
/* Control-rate gain; existing engine ramps interpolate it sample by sample. */
static int32_t performance_gain(track_t *t,voice_t *v,int fm)
{
    const performance_t *p=&performance[t-trk];
    performance_voice_t *s=&performance_voice[t-trk][v-t->v];
    if(!p->fade && !(fm && p->amp))return 32767;
    if(!v->gate && s->down){
        s->down=0;s->stage=3;
        uint32_t ms=performance_ms(p->fade);
        s->fade_step=ms ? (32767u*CTL*1000u/FS)/ms+1u : 0u;
    }
    if(fm && p->amp){
        if(s->stage==0){
            s->amp=performance_move(s->amp,32767,performance_ms(p->attack));
            if(s->amp==32767)s->stage=1;
        }else if(s->stage==1 || s->stage==2){
            s->amp=performance_move(s->amp,p->sustain*258,performance_ms(p->decay));
            if(s->amp==p->sustain*258)s->stage=2;
        }else s->amp=performance_move(s->amp,0,performance_ms(p->release));
    }else s->amp=32767;
    if(!v->gate && s->fade_step){
        s->fade=s->fade>(int32_t)s->fade_step?s->fade-(int32_t)s->fade_step:0;
    }
    int32_t gain=(s->amp*s->fade)/32767;
    if(!v->gate && !gain){v->active=0;v->stage=0;v->env=0;}
    return gain;
}

