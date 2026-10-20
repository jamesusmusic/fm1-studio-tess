/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess controls — By: Inputsignwave.
 * Central graph/slot IDs, labels, ranges and help for project-owned controls. */
static uint8_t s7_ui_lane,s7_ui_step,s7_ui_group;
static const struct {const char *label[4],*help;} S7_CONTROLS[]={
    {{"LOCAL","BEATS","UNIT","GRPS"},"Local meter"},
    {{"COUNT","GROUP","BEATS","TOTAL"},"Accent grouping"},
    {{"NUM","DEN","COND","REF"},"NUM against DEN"},
    {{"PHASE","DRIFT","HOLD","ALIGN"},"NOW < ALIGN > BAR"},
    {{"RESET","LOCAL","METER","RATE"},"Clock reset event"},
    {{"RNG","SEED","ROLL","CHAOS"},"Repeat from PLAY"},
    {{"KIT","SRC","VARY","FILL"},"Develop source kit"},
    {{"LANE","DENS","ROLE","SRC"},"Eight kit roles"},
    {{"HUMAN","GHOST","ACC","VARY"},"Anchors keep time"},
    {{"STEP","MASK","LOCK","CLEAR"},"Protect source hits"},
    {{"STEP","A","B","PROB"},"A + B transposes"},
    {{"P1","P2","P3","P4"},"Four dividers"}
};
static const char *const S7_ROLE_NAME[]={"KICK","SNARE","CH","OH","PERC 1","PERC 2","PERC 3","PERC 4"};
static const char *const S7_RESET_NAME[]={"PLAY","BANK","FORM","KEEP"};
static const char *const S7_REF_NAME[]={"LOCAL","MASTER","SECTION","PATTERN"};
static const char *studio7_ui_label(uint32_t graph,uint32_t slot)
{return S7_CONTROLS[graph-GR_S7_METER].label[slot];}
static const char *studio7_ui_help(uint32_t graph)
{return S7_CONTROLS[graph-GR_S7_METER].help;}
static void studio7_ui_value(uint32_t graph,uint32_t slot,char out[16])
{
    s7_track_t *s=&studio7_track[song.sel];s7_meter_t *m=&s->meter;s7_kit_t *k=&s->kit;
    int32_t v=0;const char *text=0;
    switch(graph){
    case GR_S7_METER:v=slot==0?m->enabled:slot==1?m->numerator:slot==2?m->denominator:m->group_count;break;
    case GR_S7_GROUP:v=slot==0?m->group_count:slot==1?s7_ui_group+1:slot==2?m->group[s7_ui_group%m->group_count]:m->numerator;break;
    case GR_S7_RATIO:if(slot==2)text=STUDIO7_COND_NAME[m->condition];else if(slot==3)text=S7_REF_NAME[m->reference];else v=slot==0?m->ratio_n:m->ratio_d;break;
    case GR_S7_PHASE:if(slot==3)text="TURN";else v=slot==0?m->phase:slot==1?m->drift:m->freeze;break;
    case GR_S7_RESET:if(slot==0)text=S7_RESET_NAME[m->reset];else if(slot==1)text=m->enabled?"ON":"OFF";else if(slot==2){fmt_int(out,m->numerator);str_cpy(out+str_len(out),"/",2);fmt_int(out+str_len(out),m->denominator);return;}else{fmt_int(out,m->ratio_n);str_cpy(out+str_len(out),":",2);fmt_int(out+str_len(out),m->ratio_d);return;}break;
    case GR_S7_SEED:if(slot==2)text="ROLL";else v=slot==0?s->seeded:slot==1?s->seed:s->grids.chaos;break;
    case GR_S7_KIT:v=slot==0?k->enabled:slot==1?k->source+1:slot==2?k->variation:k->fill;break;
    case GR_S7_LANE:if(slot==2)text=S7_ROLE_NAME[k->role[s7_ui_lane]];else v=slot==0?s7_ui_lane+1:slot==1?k->density[s7_ui_lane]:k->source+1;break;
    case GR_S7_HUMAN:v=slot==0?k->human:slot==1?k->ghost:slot==2?k->accent:k->variation;break;
    case GR_S7_ANCHOR:if(slot>=2)text=slot==2?"HITS":"ALL";else v=slot==0?s7_ui_step+1:k->anchors[s7_ui_step];break;
    case GR_S7_AWAKE:v=slot==0?s7_ui_step%16+1:slot==1?gen_awake_a(&s->awake,s7_ui_step%16):slot==2?gen_awake_b(&s->awake,s7_ui_step%16):s->awake.prob;break;
    case GR_S7_POLY:v=s->poly.pitch[slot];break;
    }
    if(text)str_cpy(out,text,16);else fmt_int(out,v);
}
static void studio7_group_count(s7_meter_t *m,uint32_t count)
{
    m->group_count=(uint8_t)clamp((int32_t)count,1,m->numerator<8?m->numerator:8);
    memset(m->group,0,8);
    for(uint32_t i=0;i<m->group_count;i++)m->group[i]=(uint8_t)(m->numerator/m->group_count+(i<m->numerator%m->group_count));
    s7_ui_group%=m->group_count;
}
static void studio7_ui_knob(uint32_t graph,uint32_t slot,int32_t delta)
{
    uint32_t tr=song.sel;int32_t d=tess_knob_delta(delta),dir=d>0?1:-1;
    s7_track_t *s=&studio7_track[tr];s7_meter_t *m=&s->meter;s7_kit_t *k=&s->kit;
    fm1_irq_off();
    switch(graph){
    case GR_S7_METER:
        if(slot==0){m->enabled=(uint8_t)clamp((int32_t)m->enabled+dir,0,1);s7_clock_start(&studio7_run[tr].clock,m,studio7_run[tr].now);}
        if(slot==1){m->numerator=(uint8_t)clamp((int32_t)m->numerator+d,1,16);studio7_group_count(m,m->group_count);}
        if(slot==2){int32_t i=m->denominator==4?0:m->denominator==8?1:2;i=clamp(i+dir,0,2);m->denominator=(uint8_t)(4u<<i);}
        if(slot==3)studio7_group_count(m,clamp((int32_t)m->group_count+dir,1,8));break;
    case GR_S7_GROUP:
        if(slot==0)studio7_group_count(m,clamp((int32_t)m->group_count+dir,1,8));
        if(slot==1)s7_ui_group=(uint8_t)clamp((int32_t)s7_ui_group+dir,0,m->group_count-1);
        if(slot==2&&m->group_count>1){
            for(uint32_t turn=0;turn<(uint32_t)(d>0?d:-d);turn++){
                uint32_t other=(s7_ui_group+1)%m->group_count;
                if(dir>0){for(uint32_t j=1;j<m->group_count;j++){uint32_t o=(s7_ui_group+j)%m->group_count;if(m->group[o]>1){other=o;break;}}if(m->group[other]>1){m->group[s7_ui_group]++;m->group[other]--;}}
                else if(m->group[s7_ui_group]>1){m->group[s7_ui_group]--;m->group[other]++;}
            }
        }break;
    case GR_S7_RATIO:
        if(slot==0)m->ratio_n=(uint8_t)clamp((int32_t)m->ratio_n+d,1,8);
        if(slot==1)m->ratio_d=(uint8_t)clamp((int32_t)m->ratio_d+d,1,8);
        if(slot==2)m->condition=(uint8_t)clamp((int32_t)m->condition+dir,0,10);
        if(slot==3)m->reference=(uint8_t)clamp((int32_t)m->reference+dir,0,3);break;
    case GR_S7_PHASE:
        if(slot==0){m->phase=(uint16_t)clamp((int32_t)m->phase+d*16,0,4095);s7_clock_realign(&studio7_run[tr].clock,m,studio7_run[tr].now,0);}
        if(slot==1)m->drift=(int16_t)clamp((int32_t)m->drift+d,-100,100);
        if(slot==2)m->freeze=(uint8_t)clamp((int32_t)m->freeze+dir,0,1);
        if(slot==3){studio7_drum_stop(tr);s7_clock_realign(&studio7_run[tr].clock,m,studio7_run[tr].now,dir>0);}break;
    case GR_S7_RESET:if(slot==0)m->reset=(uint8_t)clamp((int32_t)m->reset+dir,0,3);break;
    case GR_S7_SEED:
        if(slot==0){s->seeded=(uint8_t)clamp((int32_t)s->seeded+dir,0,1);studio7_restart(tr);}
        if(slot==1){s->seed=(uint16_t)clamp((int32_t)s->seed+d,1,128);s->seeded=1;if(tessera.mode[tr]==TESS_ENG_AWAKE)gen_awake_fill(&s->awake,s->seed);if(tessera.mode[tr]==TESS_ENG_POLY)gen_poly_fill(&s->poly,s->seed);if(tessera.mode[tr]==TESS_ENG_EUCLID)tessera.euclid[tr].pitch_seed=(uint8_t)s->seed;studio7_restart(tr);}
        if(slot==2)studio7_roll(tr);
        if(slot==3)s->grids.chaos=(uint8_t)clamp((int32_t)s->grids.chaos+d,0,100);break;
    case GR_S7_KIT:
        if(slot==0){if(!drum_track(TSEL)){fm1_irq_on();ui_message("SELECT DRUM ENGINE");return;}tessera_kill(TSEL);k->enabled=(uint8_t)clamp((int32_t)k->enabled+dir,0,1);if(tessera.mode[tr]<TESS_ENG_GRIDS)s->seeded=1;tessera.mode[tr]=TESS_ENG_KIT;}
        if(slot==1){studio7_drum_stop(tr);k->source=(uint8_t)clamp((int32_t)k->source+d,0,7);}
        if(slot==2)k->variation=(uint8_t)clamp((int32_t)k->variation+d,0,100);
        if(slot==3)k->fill=(uint8_t)clamp((int32_t)k->fill+d,0,100);break;
    case GR_S7_LANE:
        if(slot==0)s7_ui_lane=(uint8_t)clamp((int32_t)s7_ui_lane+d,0,7);
        if(slot==1)k->density[s7_ui_lane]=(uint8_t)clamp((int32_t)k->density[s7_ui_lane]+d,0,100);
        if(slot==2){uint32_t role=(uint32_t)clamp((int32_t)k->role[s7_ui_lane]+dir,0,7),old=k->role[s7_ui_lane];for(uint32_t j=0;j<8;j++)if(k->role[j]==role)k->role[j]=(uint8_t)old;k->role[s7_ui_lane]=(uint8_t)role;}
        if(slot==3)k->source=(uint8_t)clamp((int32_t)k->source+d,0,7);break;
    case GR_S7_HUMAN:
        if(slot==0)k->human=(uint8_t)clamp((int32_t)k->human+d,0,100);
        if(slot==1)k->ghost=(uint8_t)clamp((int32_t)k->ghost+d,1,127);
        if(slot==2)k->accent=(uint8_t)clamp((int32_t)k->accent+d,1,127);
        if(slot==3)k->variation=(uint8_t)clamp((int32_t)k->variation+d,0,100);break;
    case GR_S7_ANCHOR:
        if(slot==0)s7_ui_step=(uint8_t)clamp((int32_t)s7_ui_step+d,0,63);
        if(slot==1)k->anchors[s7_ui_step]=(uint8_t)clamp((int32_t)k->anchors[s7_ui_step]+d,0,255);
        if(slot==2){const step_t *base=k->source==TSEL->pattern?TSEL->step:pattern_at(tr,k->source)->step;for(uint32_t i=0;i<64;i++)k->anchors[i]=base[i].hit;}
        if(slot==3)memset(k->anchors,0,64);break;
    case GR_S7_AWAKE:
        if(slot==0)s7_ui_step=(uint8_t)clamp((int32_t)(s7_ui_step%16)+d,0,15);
        if(slot==1)gen_awake_put(s->awake.a,s7_ui_step%16,(uint32_t)clamp((int32_t)gen_awake_a(&s->awake,s7_ui_step%16)+d,0,8));
        if(slot==2)gen_awake_put(s->awake.b,s7_ui_step%16,(uint32_t)clamp((int32_t)gen_awake_b(&s->awake,s7_ui_step%16)+d,0,7));
        if(slot==3)s->awake.prob=(uint8_t)clamp((int32_t)s->awake.prob+d,0,100);break;
    case GR_S7_POLY:s->poly.pitch[slot]=(uint8_t)clamp((int32_t)s->poly.pitch[slot]+d,0,20);break;
    }
    s->mode=tessera.mode[tr];fm1_irq_on();ui.force=1;
}
