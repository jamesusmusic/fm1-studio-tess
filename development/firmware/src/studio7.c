/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess integration — By: Inputsignwave.
 * CPU0 owns generators, local clocks, MIDI and capture. No flash/IO here. */
static struct {
    s7_clock_t clock;
    uint64_t now,fraction;
    uint32_t event,bar,grid_index;
    uint32_t section_bar;
    uint8_t bank,section,primed,initialized;
    struct {uint64_t at;uint8_t lane,vel,on;} pending[16];
    struct {uint32_t left;uint8_t note,on;} drum[8];
} studio7_run[4] __attribute__((section(".pool")));
static uint32_t studio7_rand(uint32_t tr)
{ return tr<4u&&studio7_track[tr].seeded?gen_next(&studio7_random[tr]):rng(); }
static void studio7_restart(uint32_t tr)
{
    s7_track_t *s=&studio7_track[tr];
    studio7_random[tr]=gen_stream(s->seeded||tessera.mode[tr]<TESS_ENG_GRIDS?s->seed:rng());
    s->grids.step_idx=0;memset(s->grids.perturb,0,3);
    gen_bounce_serve(&s->bounce,s->seed);
    s->awake.pos_a=s->awake.pos_b=0;s->poly.pos=3;s->poly.tick=0;
    if(s->seeded){tessera.turing[tr].reg=gen_turing_pick(gen_stream(s->seed));tessera.euclid[tr].step_idx=0;tessera.markov[tr].cur_state=0;tessera.morph[tr].step_idx=(tessera.morph[tr].step_idx&0xF0u)|15u;}
}
static void studio7_start(void)
{
    for(uint32_t tr=0;tr<4;tr++){
        if(studio7_track[tr].meter.reset==3&&studio7_run[tr].initialized){
            s7_clock_t *c=&studio7_run[tr].clock;
            c->next=c->next>studio7_run[tr].now?c->next-studio7_run[tr].now:0;c->last=0;c->started=0;
            studio7_run[tr].now=0;studio7_run[tr].fraction=0;
        }else{
            studio7_restart(tr);memset(&studio7_run[tr],0,sizeof studio7_run[tr]);
            s7_meter_t m=studio7_track[tr].meter.enabled?studio7_track[tr].meter:s7_meter_default();
            s7_clock_start(&studio7_run[tr].clock,&m,0);
        }
        studio7_run[tr].initialized=1;studio7_run[tr].bank=trk[tr].pattern;
    }
}
static int studio7_local(const track_t *t)
{return tessera.mode[trk_index(t)] && (studio7_track[trk_index(t)].meter.enabled||tessera.mode[trk_index(t)]==TESS_ENG_KIT);}
static void studio7_roll(uint32_t tr)
{
    s7_track_t *s=&studio7_track[tr];
    uint32_t r=studio7_random[tr];
    s->seed=(uint16_t)(1u+(s->seed-1u+1u+gen_rnd(&r,127u))%128u);s->seeded=1;
    if(tessera.mode[tr]==TESS_ENG_AWAKE)gen_awake_fill(&s->awake,s->seed);
    if(tessera.mode[tr]==TESS_ENG_POLY)gen_poly_fill(&s->poly,s->seed);
    if(tessera.mode[tr]==TESS_ENG_EUCLID)tessera.euclid[tr].pitch_seed=(uint8_t)s->seed;
    if(tessera.mode[tr]==TESS_ENG_TURING)tessera.turing[tr].reg=gen_turing_pick(gen_stream(s->seed));
    studio7_restart(tr);tessera.evolve[tr].flash=4;
}
static int studio7_knob(uint32_t tr,uint32_t k,int32_t delta)
{
    s7_track_t *s=&studio7_track[tr];uint8_t *p=0;int32_t lo=0,hi=0;
    switch(tessera.mode[tr]){
    case TESS_ENG_GRIDS:
        p=k==1?&s->grids.x:k==2?&s->grids.y:&s->grids.dens;hi=k==3?100:255;break;
    case TESS_ENG_BOUNCE:
        p=k==1?&s->bounce.speed:k==2?&s->bounce.angle:&s->bounce.balls;
        lo=k==2?0:1;hi=k==1?64:k==2?90:4;break;
    case TESS_ENG_AWAKE:
        p=k==1?&s->awake.len_a:k==2?&s->awake.len_b:&s->awake.prob;
        lo=k==3?0:1;hi=k==3?100:16;break;
    case TESS_ENG_KIT:
        p=k==1?&s->kit.source:k==2?&s->kit.variation:&s->kit.fill;hi=k==1?7:100;break;
    case TESS_ENG_POLY:
        if(k==1){s->poly.sel=(uint8_t)clamp((int32_t)s->poly.sel+delta,0,3);return 1;}
        p=k==2?&s->poly.div[s->poly.sel%4]:&s->poly.span;lo=k==2?0:1;hi=k==2?16:3;break;
    default:return 0;
    }
    *p=(uint8_t)clamp((int32_t)*p+delta,lo,hi);return 1;
}
static void studio7_note(track_t *t,uint32_t note,uint32_t vel,int drum)
{
    uint32_t tr=trk_index(t);
    if(!drum){
        const tessera_evolve_t *tw=&tessera.evolve[tr];
        int32_t n=(int32_t)note;
        if(tw->follow && tw->follow-1u!=tr && tessera.mode[tw->follow-1u])n=tessera.last_note[tw->follow-1u];
        n=tessera_snap_note(t,clamp(n+tess_flourish(t,tr),0,127));
        n=features_pitch(t,clamp(n+t->p[P_TRANS]+tw->shift,0,127));note=(uint32_t)n;
    }
    if(drum){
        uint32_t lane=0;for(uint32_t i=0;i<8;i++)if(DRUM_LANE_NOTE[i]==note){lane=i;break;}
        if(studio7_run[tr].drum[lane].on){trk_note_off(t,note);features_midi_off(t,note);}
        uint32_t gate=seq_div_samples(t->p[P_SDIV])*(uint32_t)(t->p[P_SGATE]?t->p[P_SGATE]:64)/128u;
        studio7_run[tr].drum[lane].on=1;studio7_run[tr].drum[lane].note=(uint8_t)note;studio7_run[tr].drum[lane].left=gate?gate:1u;
        trk_note_on(t,note,vel);features_midi_on(t,note,vel);features_capture_note(t,note,vel,gate);
    }else if(performance[tr].mode&&!drum){features_duration_on(t,note,vel);studio7_batch=1;}
    else {
        trk_note_on(t,note,vel);features_midi_on(t,note,vel);
        if(t->seq_n<sizeof t->seq_notes)t->seq_notes[t->seq_n++]=(uint8_t)note;
        uint32_t gate=seq_div_samples(t->p[P_SDIV])*(uint32_t)(t->p[P_SGATE]?t->p[P_SGATE]:64)/128u;
        t->seq_off=gate?gate:1u;
        features_capture_note(t,note,vel,t->seq_off);
    }
    tessera.last_note[tr]=(uint8_t)note;
}
static void studio7_emit(track_t *t)
{
    uint32_t tr=trk_index(t);s7_track_t *s=&studio7_track[tr];
    if(t->p[P_MUTE]){tessera_kill(t);return;}
    if(!performance[tr].mode||drum_track(t))seq_release(t);
    tessera_evolve_tick(t,tr);
    gen_clock_t clk={(uint8_t)tessera.evolve[tr].pos_now,(uint8_t)tess_steps_per_bar(t),(uint8_t)(t->p[P_ROOT]%12)};
    gen_out_t out={0};
    switch(tessera.mode[tr]){
    case TESS_ENG_GRIDS:gen_grids_tick(&s->grids,&clk,&studio7_random[tr],&out);break;
    case TESS_ENG_BOUNCE:gen_bounce_tick(&s->bounce,&clk,&studio7_random[tr],&out);break;
    case TESS_ENG_AWAKE:gen_awake_tick(&s->awake,&clk,&studio7_random[tr],&out);break;
    case TESS_ENG_POLY:gen_poly_tick(&s->poly,&clk,&studio7_random[tr],&out);break;
    }
    studio7_batch=0;
    out.vel=studio7_accent(t,out.vel);
    int allowed=studio7_allow(t);
    if(out.gate&&allowed){
        const tessera_evolve_t *tw=&tessera.evolve[tr];
        if(tw->drop&&gen_rnd(&studio7_random[tr],100)<tw->drop)return;
        if(tw->human)out.vel=(uint8_t)clamp((int32_t)out.vel+(int32_t)gen_rnd(&studio7_random[tr],tw->human/2u+1u)-(int32_t)tw->human/4,1,127);
        if(out.parts){
            /* Map roles to the kit's actual lanes; use its existing choke groups. */
            for(uint32_t role=0;role<3;role++)if(out.parts>>role&1u){
                uint32_t lane=role==2?2u:role;
                for(uint32_t j=0;j<8;j++)if(s->kit.role[j]==role){lane=j;break;}
                studio7_note(t,DRUM_LANE_NOTE[lane],out.accent>>role&1u?127:96,drum_track(t));
            }
        }else{
            studio7_note(t,out.note,out.vel,0);
            for(uint32_t i=0;i<out.more_n&&i<2;i++)studio7_note(t,out.more[i],out.vel,0);
        }
    }
    studio7_batch=0;
}
static void studio7_drum_stop(uint32_t tr)
{
    for(uint32_t l=0;l<8;l++)if(studio7_run[tr].drum[l].on){
        uint32_t note=studio7_run[tr].drum[l].note;trk_note_off(&trk[tr],note);features_midi_off(&trk[tr],note);studio7_run[tr].drum[l].on=0;
    }
    memset(studio7_run[tr].pending,0,sizeof studio7_run[tr].pending);studio7_run[tr].primed=0;
}
static void studio7_drum_tick(track_t *t,uint32_t n)
{
    uint32_t tr=trk_index(t);
    for(uint32_t l=0;l<8;l++)if(studio7_run[tr].drum[l].on){
        if(studio7_run[tr].drum[l].left<=n){uint32_t note=studio7_run[tr].drum[l].note;trk_note_off(t,note);features_midi_off(t,note);studio7_run[tr].drum[l].on=0;}
        else studio7_run[tr].drum[l].left-=n;
    }
}
static const char *const STUDIO7_COND_NAME[]={"OFF","1:2","2:2","1:4","2:4","3:4","4:4","1:8","8:8","FIRST","!FIRST"};
static int studio7_condition(uint32_t condition,uint64_t bar)
{
    static const uint8_t ab[9][2]={{0,1},{1,2},{2,2},{1,4},{2,4},{3,4},{4,4},{1,8},{8,8}};
    if(!condition)return 1;if(condition==9)return bar==0;if(condition==10)return bar!=0;
    return condition<9&&s7_mod64(bar,ab[condition][1])==ab[condition][0]-1u;
}
static uint64_t studio7_bar_at(const track_t *t,uint32_t tr)
{
    const s7_meter_t *m=&studio7_track[tr].meter;
    uint32_t div=(uint32_t)t->p[P_SDIV],bn=div<6u?1u:1u<<(div-5u),bd=div<6u?DIV_DEN[div]:1u;
    uint32_t index=studio7_local(t)?studio7_run[tr].clock.index-1u:studio7_run[tr].grid_index;
    if(m->reference==1&&studio7_local(t))return studio7_run[tr].clock.last/(4u*S7_QUARTER);
    uint64_t local=s7_div64((uint64_t)index*S7_QUARTER*bn,bd,0);
    if(m->reference==3)return index/(uint32_t)(t->p[P_SLEN]?t->p[P_SLEN]:1);
    uint64_t bar=s7_div64(local,m->enabled?s7_bar_units(m):4u*S7_QUARTER,0);
    return m->reference==2&&bar>=studio7_run[tr].section_bar?bar-studio7_run[tr].section_bar:bar;
}
static uint32_t studio7_bars(const track_t *t,uint32_t tr)
{
    const s7_meter_t *m=&studio7_track[tr].meter;uint32_t div=(uint32_t)t->p[P_SDIV];
    uint32_t bn=div<6?1u:1u<<(div-5u),bd=div<6?DIV_DEN[div]:1u;
    uint32_t index=studio7_run[tr].clock.index?studio7_run[tr].clock.index-1u:0u;
    uint32_t bar=(uint32_t)(s7_div64(s7_div64((uint64_t)index*S7_QUARTER*bn,bd,0),s7_bar_units(m),0)),old=studio7_run[tr].bar;
    studio7_run[tr].bar=bar;
    return bar>=old?bar-old:0;
}
static int studio7_allow(const track_t *t)
{
    uint32_t tr=trk_index(t);int pass=studio7_condition(studio7_track[tr].meter.condition,studio7_bar_at(t,tr));
    if(!studio7_local(t))studio7_run[tr].grid_index++;
    return pass;
}
static uint8_t studio7_accent(const track_t *t,uint8_t vel)
{
    uint32_t tr=trk_index(t);const s7_meter_t *m=&studio7_track[tr].meter;
    if(!m->enabled)return vel;
    uint32_t div=(uint32_t)t->p[P_SDIV],bn=div<6?1u:1u<<(div-5u),bd=div<6?DIV_DEN[div]:1u;
    uint32_t index=studio7_run[tr].clock.index?studio7_run[tr].clock.index-1u:0u;
    int accent=s7_group_accent(m,s7_div64((uint64_t)index*S7_QUARTER*bn,bd,0));
    return (uint8_t)clamp((int32_t)vel+accent*6,1,127);
}
static void studio7_mutate(uint32_t tr,uint32_t amount)
{
    s7_track_t *s=&studio7_track[tr];uint32_t *r=&studio7_random[tr];
    switch(tessera.mode[tr]){
    case TESS_ENG_GRIDS:gen_grids_mutate(&s->grids,amount,r);break;
    case TESS_ENG_BOUNCE:gen_bounce_mutate(&s->bounce,amount,r);break;
    case TESS_ENG_AWAKE:gen_awake_mutate(&s->awake,amount,r);break;
    case TESS_ENG_POLY:gen_poly_mutate(&s->poly,amount,r);break;
    case TESS_ENG_KIT:s->kit.variation=(uint8_t)clamp((int32_t)s->kit.variation+(int32_t)gen_rnd(r,amount+1u)-(int32_t)amount/2,0,100);break;
    }
}
static void studio7_sec(uint32_t tr,tessera_sec_t *sc,int load)
{
    s7_track_t *s=&studio7_track[tr];gen_sec_t sec={sc->reg,sc->p0,sc->p1};
    switch(tessera.mode[tr]){
    case TESS_ENG_GRIDS:if(load)gen_grids_load(&s->grids,&sec);else gen_grids_save(&s->grids,&sec);break;
    case TESS_ENG_BOUNCE:if(load)gen_bounce_load(&s->bounce,&sec);else gen_bounce_save(&s->bounce,&sec);break;
    case TESS_ENG_AWAKE:if(load)gen_awake_load(&s->awake,&sec);else gen_awake_save(&s->awake,&sec);break;
    case TESS_ENG_POLY:if(load)gen_poly_load(&s->poly,&sec);else gen_poly_save(&s->poly,&sec);break;
    case TESS_ENG_KIT:if(load){s->kit.variation=(uint8_t)clamp(sec.p0,0,100);s->kit.fill=(uint8_t)clamp(sec.p1,0,100);}else{sec.p0=s->kit.variation;sec.p1=s->kit.fill;}break;
    }
    if(!load){sc->reg=sec.reg;sc->p0=sec.p0;sc->p1=sec.p1;}
}
static void studio7_reset_event(track_t *t,const s7_meter_t *m)
{
    uint32_t tr=trk_index(t);uint8_t section=tessera.evolve[tr].sec;
    int bank=studio7_run[tr].bank!=t->pattern,form=studio7_run[tr].section!=section;
    if(form)studio7_run[tr].section_bar=studio7_run[tr].bar;
    studio7_run[tr].bank=t->pattern;studio7_run[tr].section=section;
    if((m->reset==1&&bank)||(m->reset==2&&form)){
        studio7_drum_stop(tr);studio7_restart(tr);
        s7_clock_start(&studio7_run[tr].clock,m,studio7_run[tr].now);
        studio7_run[tr].bar=studio7_run[tr].section_bar=0;
    }
}
static void studio7_kit_plan(track_t *t,const s7_meter_t *m,uint32_t bn,uint32_t bd)
{
    uint32_t tr=trk_index(t);s7_track_t *s=&studio7_track[tr];
    if(!drum_track(t)||t->p[P_MUTE]||((seq_hush>>tr)&1u))return;
    uint32_t index=studio7_run[tr].clock.index,bank=s->kit.source;
    const pattern_t *p=pattern_at(tr,bank);
    uint32_t len=bank==t->pattern?(uint32_t)t->p[P_SLEN]:(uint32_t)p->timing[0];
    uint32_t step=index%(len?len:1u);const step_t *base=bank==t->pattern?&t->step[step]:&p->step[step];
    uint64_t local=s7_div64((uint64_t)index*S7_QUARTER*bn,bd,0),bar=s7_div64(local,s7_bar_units(m),0);
    uint64_t condbar=m->reference==1?studio7_run[tr].clock.next/(4u*S7_QUARTER):m->reference==2?(bar>=studio7_run[tr].section_bar?bar-studio7_run[tr].section_bar:0u):m->reference==3?index/(len?len:1u):bar;
    if(!studio7_condition(m->condition,condbar))return;
    uint32_t hit=base->hit,acc=base->acc&hit;
    s7_kit_out_t out;
    if(!s7_kit_generate(&out,&s->kit,s->seed,step,(uint32_t)bar,hit,acc,base->vel,s7_group_accent(m,local),step+1u==len))return;
    uint64_t interval=s7_div64((uint64_t)S7_QUARTER*bn*m->ratio_d*10000u,bd*m->ratio_n*(uint32_t)(10000+(m->freeze?0:m->drift)),0);
    for(uint32_t l=0;l<8;l++)if(out.hit>>l&1u){
        int64_t at=(int64_t)studio7_run[tr].clock.next+(int64_t)interval*out.offset[l]/256;
        if(at<(int64_t)studio7_run[tr].now)at=(int64_t)studio7_run[tr].now;
        for(uint32_t j=0;j<16;j++)if(!studio7_run[tr].pending[j].on){
            studio7_run[tr].pending[j].at=(uint64_t)at;studio7_run[tr].pending[j].lane=(uint8_t)l;
            studio7_run[tr].pending[j].vel=out.velocity[l];studio7_run[tr].pending[j].on=1;break;
        }
    }
}
/* Bound event work and split grid/capture time at performed onsets, including
 * negative kit offsets scheduled a step ahead. No wall-clock or second tempo. */
static void studio7_tick(track_t *t,uint32_t n)
{
    uint32_t tr=trk_index(t),quarter=seq_beat_samples(),budget=256;
    s7_meter_t fallback=s7_meter_default();
    s7_meter_t *m=studio7_track[tr].meter.enabled?&studio7_track[tr].meter:&fallback;
    if(!quarter||!s7_meter_valid(m)){seq_grid_tick(t,n);return;}
    uint32_t div=(uint32_t)t->p[P_SDIV],bn=div<6u?1u:1u<<(div-5u),bd=div<6u?DIV_DEN[div]:1u;
    if(t->seq_pos>=0x7FFFFFFFu)seq_grid_tick(t,0);
    int kit=tessera.mode[tr]==TESS_ENG_KIT;
    if(t->p[P_MUTE])tessera_kill(t);
    if(kit&&!studio7_run[tr].primed&&!t->p[P_MUTE]){studio7_kit_plan(t,m,bn,bd);studio7_run[tr].primed=1;}
    while(budget--){
        uint64_t next=studio7_run[tr].clock.next,now=studio7_run[tr].now;
        for(uint32_t j=0;j<16;j++)if(studio7_run[tr].pending[j].on&&studio7_run[tr].pending[j].at<next)next=studio7_run[tr].pending[j].at;
        uint64_t need=next>now?(next-now)*quarter:0;
        need=need>studio7_run[tr].fraction?need-studio7_run[tr].fraction:0;
        uint64_t wait=(need+S7_QUARTER-1u)/S7_QUARTER;
        uint32_t chunk=wait<n?(uint32_t)wait:n;
        if(chunk){
            studio7_drum_tick(t,chunk);features_duration_tick(t,chunk);seq_grid_tick(t,chunk);n-=chunk;
            uint64_t delta=(uint64_t)chunk*S7_QUARTER+studio7_run[tr].fraction;
            uint32_t remainder;studio7_run[tr].now+=s7_div64(delta,quarter,&remainder);studio7_run[tr].fraction=remainder;
            studio7_reset_event(t,m);
        }
        if(kit&&!studio7_run[tr].primed&&!t->p[P_MUTE]){studio7_kit_plan(t,m,bn,bd);studio7_run[tr].primed=1;}
        int fired=0;studio7_fractional=1;
        features_capture_guard(t);
        for(uint32_t j=0;j<16;j++)if(studio7_run[tr].pending[j].on&&studio7_run[tr].pending[j].at<=studio7_run[tr].now){
            if(!t->p[P_MUTE])studio7_note(t,DRUM_LANE_NOTE[studio7_run[tr].pending[j].lane],studio7_run[tr].pending[j].vel,1);
            studio7_run[tr].pending[j].on=0;fired=1;
        }
        uint64_t onset;
        if(s7_clock_take(&studio7_run[tr].clock,m,studio7_run[tr].now,bn,bd,&onset)){
            studio7_run[tr].event++;fired=1;
            if(kit){tessera_evolve_tick(t,tr);studio7_reset_event(t,m);studio7_kit_plan(t,m,bn,bd);}
            else if(!((seq_hush>>tr)&1u))tessera_clock_step(t);
        }
        studio7_fractional=0;
        if(!n&&!fired)break;
    }
    if(n){studio7_drum_stop(tr);features_duration_tick(t,n);seq_grid_tick(t,n);uint64_t delta=(uint64_t)n*S7_QUARTER+studio7_run[tr].fraction;uint32_t remainder;studio7_run[tr].now+=s7_div64(delta,quarter,&remainder);studio7_run[tr].fraction=remainder;s7_clock_realign(&studio7_run[tr].clock,m,studio7_run[tr].now,1);}
}
