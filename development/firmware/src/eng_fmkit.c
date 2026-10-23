/* SPDX-License-Identifier: GPL-3.0-only
 * Native FM6 eight-lane kits and per-hit patch lifetime — By: Inputsignwave.
 * Uses Melodee/Dexed FM6 envelope and operator kernels; original notices remain
 * in eng_fm6.c and fm6_core.c. Each hit snapshots its complete native patch.
 * Editing another lane never retunes an already ringing percussion voice. */
#if !MELODEE_RETIRED_REFERENCE
#define FMK_POLY 8u
typedef struct { fm6_voice_t voice;uint8_t patch[FP_SIZE+1u];uint32_t age;uint8_t lane; } fmk_voice_t;
typedef struct { fmk_voice_t v[NVOICE]; } fmk_part_t;
static fmk_part_t *fmk_part(const track_t *t);
static void fmk_note_on(track_t *t,voice_t *v)
{
 fmk_part_t *p=fmk_part(t);fmk_voice_t *hit=&p->v[v-t->v];
 fm6_voice_t *s=&hit->voice;uint32_t lane=drum_lane(v->note);
 if(lane==DV_HATC)for(uint32_t i=0;i<NVOICE;i++)
  if(t->v[i].active && &t->v[i]!=v && p->v[i].lane==DV_HATO)t->v[i].stage=4;
 uint8_t packed[FM6_PACKED];fm6_factory((uint32_t)clamp(t->p[P_E0+lane],0,FM6_NFAC-1u),packed);
 fm6_unpack(packed,hit->patch);hit->lane=(uint8_t)lane;hit->age=0;
 uint8_t *ed=hit->patch;
 /* On a sounding retrigger preserve phase/feedback and the output correction
  * history. Cold voices start from a deterministic oscillator phase. */
 if(!v->env && !v->env_out)memset(s,0,sizeof *s);
 s->vel=v->mvel?v->mvel:v->vel;s->note=60;
 fm6_keyed(s,ed,(uint32_t)clamp(60+ed[FP_TRNSP]-24,0,127));
 for(uint32_t k=0;k<6;k++){
  s->eg[k].level=0;fm6_eg_go(&s->eg[k],ed+k*FP_OP,s->ol[k],s->rs[k],0);
  s->porta[k]=s->base[k];
 }
 fm6_peg_set(&s->pe,ed);s->down=1;s->played=1;s->quiet=s->sub=s->frozen=s->still=0;
 drum_flash[t-trk]|=(uint8_t)(1u<<lane);
}
static int fmk_done(track_t *t,voice_t *v)
{ return fmk_part(t)->v[v-t->v].voice.quiet>=2u; }
static void fmk_render(track_t *t,voice_t *v,int32_t *out,uint32_t n,const vmod_t *m)
{
 fmk_voice_t *hit=&fmk_part(t)->v[v-t->v];fm6_voice_t *s=&hit->voice;const uint8_t *ed=hit->patch;
 hit->age+=n;
 if(!s->sub){
  if(s->down && (hit->age>=FS/12u || !v->gate)){
   s->down=0;for(uint32_t k=0;k<6;k++)fm6_eg_go(&s->eg[k],ed+k*FP_OP,s->ol[k],s->rs[k],3);
   fm6_peg_go(&s->pe,3);
  }
  uint32_t alg=ed[FP_ALG]&31u,dead=1;int32_t levels[6];
  int32_t pitch=fm6_peg_step(&s->pe,s->down)+m->plog+song.g[G_TUNE]*13981+tuning_log();
  for(uint32_t k=0;k<6;k++){
   const uint8_t *op=ed+k*FP_OP;
   int32_t freq=s->base[k]+(op[FP_MODE]?0:pitch);
   s->fq[k]=fm6_freq(freq);
   levels[k]=fm6_eg_step(&s->eg[k],op,s->ol[k],s->rs[k],s->down);
   if(fm6_carrier(alg,6u-k) && (s->eg[k].level>=FM6_SILENT || s->eg[k].ix<3u || s->down))dead=0;
  }
  s->quiet=dead?(uint8_t)(s->quiet+(s->quiet<2u)):0;
  fm6_plan(s,levels,alg,FM6F(t)[FN_ENGINE],ed[FP_FB]?8u-ed[FP_FB]:16u);
 }
 s->sub^=1u;
 vmod_t gain=*m;int32_t lane=clamp(t->p[P_LN0+hit->lane],0,127)*258;
 gain.amp0=mulq15(gain.amp0,lane);gain.amp1=mulq15(gain.amp1,lane);
 if(audio_batch_active){const fm6_batch_t job={s,gain,n,VOICE_FS};audio_batch_push(fm6_batch_kernel,&job,sizeof job,out,n,AP_FM6);return;}
 int32_t pcm[CTL];const fm6_batch_t job={s,gain,n,VOICE_FS};
 fm6_batch_kernel(&job,pcm);for(uint32_t i=0;i<n;i++)out[i]+=pcm[i];
}
static const preset_t FMK_PRESETS[]={
 {"FM ELECTRONIC",{31,29,30,33,34,37,32,39},{0,45,127,35},0,0},
 {"FM METAL KIT",{31,44,36,40,35,37,32,39},{0,45,127,35},0,0},
 {"FM TUNED KIT",{1,11,6,12,19,21,23,5},{0,45,127,35},0,0},
};
static const engine_t ENG_FMKIT={
 .name="FM DRUM KIT",.page_title={"FM LANES 1-4","FM LANES 5-8"},
 .edit={{"KICK",F_INT,0,48,31,0,0},{"SNARE",F_INT,0,48,29,0,0},
 {"TOM",F_INT,0,48,30,0,0},{"PERC",F_INT,0,48,33,0,0},
 {"CLAP",F_INT,0,48,34,0,0},{"BELL",F_INT,0,48,37,0,0},
 {"C HAT",F_INT,0,48,32,0,0},{"O HAT",F_INT,0,48,39,0,0}},
 .presets=FMK_PRESETS,.npresets=NELEM(FMK_PRESETS),.poly=FMK_POLY,.oneshot=1,
 .note_on=fmk_note_on,.render=fmk_render,.keys=drum_keys,.ownenv=1,.done=fmk_done,
 .knob={P_LN0,P_LN1,P_LN6,P_LN7}
};
#endif
