/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio) */
/* FBKT retains the existing 27,904-byte A/B allocation and every step/event.
 * Timing triples and 3-bit bank tags reclaim 112 bytes. Tessera's first 180
 * bytes use the FUN22 reserved gap; HMF1 uses the reclaimed bank tail. FBKK
 * imports remain read-only until SAVE or the protected TSR migration. */
#define BANK_LEGACY_MAGIC 0x4B4B4246u
#define BANK_MAGIC 0x544B4246u /* FBKT */
#define BANK_STORE_SIZE PROJ_BANK_BYTES
#define BANK_LEGACY_SIZE BANK_STORE_SIZE
#define BANK_PROJ_SIZE PROJ_STORE_SIZE
#define BANK_STEP_SIZE 8u
#define BANK_ACTIVE_OFF (8u + BANK_PROJ_SIZE)
#define BANK_EXTRA_OFF (BANK_ACTIVE_OFF + NTRK)
#define BANK_TIMING_OFF (BANK_EXTRA_OFF + NTRK * (NPAT - 1u) * NSTEP * BANK_STEP_SIZE)
#define BANK_TIME_SIZE 3u
#define BANK_CHAIN_OFF (BANK_TIMING_OFF + NTRK * NPAT * BANK_TIME_SIZE)
#define BANK_CHAIN_BYTES ((CHAIN_ROWS*NTRK*3u+7u)/8u)
#define BANK_MOTION_OFF (BANK_CHAIN_OFF + BANK_CHAIN_BYTES)
#define BANK_MOTION_BYTES ((MOTION_MAX*3u+7u)/8u)
#define BANK_FN_OFF (BANK_MOTION_OFF + BANK_MOTION_BYTES)
#define BANK_FN_MAGIC 0x31364E46u
#define BANK_OLD_CHAIN_OFF (BANK_TIMING_OFF + NTRK * NPAT * 4u)
#define BANK_OLD_MOTION_OFF (BANK_OLD_CHAIN_OFF + CHAIN_ROWS*NTRK)
#define BANK_OLD_FN_OFF (BANK_OLD_MOTION_OFF + MOTION_MAX)
#define BANK_TESS_OFF (BANK_FN_OFF + 4u + NTRK*FM6_NFN)
#define BANK_TESS_MAGIC 0x34435354u
#define PROJ_TESS_OFF (PROJ_TONE_WIRE - TESSERA_PK_BYTES)
_Static_assert(NELEM(N_DIV) <= 16u,"packed bank division field");
_Static_assert(BANK_TESS_OFF + 4u + TESS_FEATURE_BYTES <= BANK_STORE_SIZE-4u,"Tess state fits the existing project allocation");
_Static_assert(68u+NTRK*(P_COUNT+2u+NSTEP*8u)+sizeof(chain_config_t)+sizeof(motion_store_t)+2u+2u*NTRK <= PROJ_TESS_OFF,"Tessera uses only the reserved FUN22 gap");
static int bank_full(uint32_t n) { return n == BANK_STORE_SIZE; }
static int bank_custom(const uint8_t *raw) { uint32_t m;memcpy(&m,raw,4u);return m==BANK_MAGIC; }
static uint32_t bank_fn_off(const uint8_t *raw) { return bank_custom(raw)?BANK_FN_OFF:BANK_OLD_FN_OFF; }
static uint8_t bank_tag_get(const uint8_t *raw,uint32_t off,uint32_t index)
{ uint32_t pos=index*3u;return (uint8_t)wire_bits_get(raw+off,&pos,3u); }
static void bank_tag_put(uint8_t *raw,uint32_t off,uint32_t index,uint32_t value)
{
    uint32_t pos=index*3u;
    for(uint32_t i=0;i<3u;i++,pos++){
        uint8_t mask=(uint8_t)(1u<<(pos&7u));
        raw[off+(pos>>3)]=(uint8_t)((raw[off+(pos>>3)]&~mask)|(((value>>i)&1u)?mask:0u));
    }
}
static uint8_t bank_chain_get(const uint8_t *raw,uint32_t row,uint32_t track)
{ return bank_custom(raw)?bank_tag_get(raw,BANK_CHAIN_OFF,row*NTRK+track):raw[BANK_OLD_CHAIN_OFF+row*NTRK+track]; }
static uint8_t bank_motion_get(const uint8_t *raw,uint32_t i)
{ return bank_custom(raw)?bank_tag_get(raw,BANK_MOTION_OFF,i):raw[BANK_OLD_MOTION_OFF+i]; }
static uint32_t bank_timing_get(const uint8_t *raw,uint32_t track,uint32_t bank,uint32_t field)
{
    uint32_t index=track*NPAT+bank;
    if(!bank_custom(raw))return raw[BANK_TIMING_OFF+index*4u+field];
    uint32_t pos=index*24u+ (field==0u?0u:field==1u?6u:field==2u?10u:17u);
    return wire_bits_get(raw+BANK_TIMING_OFF,&pos,field==0u?6u:field==1u?4u:7u)+(field==0u);
}
static void bank_timing_put(uint8_t *raw,uint32_t track,uint32_t bank,const int16_t *timing)
{
    uint32_t pos=0;uint8_t *v=raw+BANK_TIMING_OFF+(track*NPAT+bank)*3u;memset(v,0,3u);
    wire_bits_put(v,&pos,(uint32_t)timing[0]-1u,6u);wire_bits_put(v,&pos,(uint32_t)timing[1],4u);
    wire_bits_put(v,&pos,(uint32_t)timing[2],7u);wire_bits_put(v,&pos,(uint32_t)timing[3],7u);
}
static void bank_checksum(uint8_t *raw)
{ uint32_t sum=proj_hash(raw,BANK_STORE_SIZE-4u);memcpy(raw+BANK_STORE_SIZE-4u,&sum,4u); }
static void bank_upgrade_layout(uint8_t *raw)
{
    if(bank_custom(raw))return;
    for(uint32_t k=0;k<NTRK;k++)for(uint32_t b=0;b<NPAT;b++){
        int16_t time[4];for(uint32_t i=0;i<4u;i++)time[i]=(int16_t)raw[BANK_TIMING_OFF+(k*NPAT+b)*4u+i];
        bank_timing_put(raw,k,b,time);
    }
    uint8_t tags[MOTION_MAX];
    memcpy(tags,raw+BANK_OLD_CHAIN_OFF,CHAIN_ROWS*NTRK);memset(raw+BANK_CHAIN_OFF,0,BANK_CHAIN_BYTES);
    for(uint32_t i=0;i<CHAIN_ROWS*NTRK;i++)bank_tag_put(raw,BANK_CHAIN_OFF,i,tags[i]);
    memcpy(tags,raw+BANK_OLD_MOTION_OFF,MOTION_MAX);memset(raw+BANK_MOTION_OFF,0,BANK_MOTION_BYTES);
    for(uint32_t i=0;i<MOTION_MAX;i++)bank_tag_put(raw,BANK_MOTION_OFF,i,tags[i]);
    uint8_t fn[4u+NTRK*FM6_NFN];memcpy(fn,raw+BANK_OLD_FN_OFF,sizeof fn);memcpy(raw+BANK_FN_OFF,fn,sizeof fn);
    memset(raw+BANK_TESS_OFF,0,BANK_STORE_SIZE-4u-BANK_TESS_OFF);
    uint32_t m=BANK_MAGIC;memcpy(raw,&m,4u);
}
static void bank_tess_set(uint8_t *raw,const uint8_t *state)
{
    bank_upgrade_layout(raw);
    uint32_t magic=state?BANK_TESS_MAGIC:0u;
    memcpy(raw+BANK_TESS_OFF,&magic,4u);
    if(state){memcpy(raw+8u+PROJ_TESS_OFF,state,TESSERA_PK_BYTES);memcpy(raw+BANK_TESS_OFF+4u,state+TESSERA_PK_BYTES,TESS_FEATURE_BYTES);}
    else {memset(raw+8u+PROJ_TESS_OFF,0,TESSERA_PK_BYTES);memset(raw+BANK_TESS_OFF+4u,0,TESS_FEATURE_BYTES);}
    uint32_t sum=proj_hash(raw+8u,PROJ_STORE_SIZE-4u);memcpy(raw+8u+PROJ_STORE_SIZE-4u,&sum,4u);bank_checksum(raw);
}
static int bank_step_pack(uint8_t *b,const step_t *s)
{
    if(s->n>4u||s->time>ST_REST||s->flags>7u||s->vel>127u||s->probability>101u||!step_acc_valid(s))return 0;
    uint32_t pos=0;memset(b,0,8u);
    for(uint32_t j=0;j<4u;j++){if(s->note[j]>127u)return 0;wire_bits_put(b,&pos,s->note[j],7u);}
    wire_bits_put(b,&pos,(s->time==ST_NOTE?s->n:s->time==ST_TIE?5u:6u)|(s->flags&SF_RECORDED?8u:0u),4u);
    wire_bits_put(b,&pos,s->flags&3u,2u);wire_bits_put(b,&pos,s->vel,7u);wire_bits_put(b,&pos,s->hit,8u);
    wire_bits_put(b,&pos,s->acc,8u);wire_bits_put(b,&pos,s->probability,7u);return 1;
}
static int bank_pack(uint8_t *raw,const project_t *q,int runtime)
{
    uint32_t magic=BANK_MAGIC,size=BANK_STORE_SIZE,pos=BANK_EXTRA_OFF,f=motion_guard();
    memset(raw,0,BANK_STORE_SIZE);memcpy(raw,&magic,4u);memcpy(raw+4u,&size,4u);
    if(!proj_pack((project_store_t *)(raw+8u),q)){motion_unguard(f);return 0;}
    for(uint32_t k=0;k<NTRK;k++){
        uint32_t active=runtime?trk[k].pattern:q->pattern[k];raw[BANK_ACTIVE_OFF+k]=(uint8_t)active;
        if(runtime)pattern_commit(&trk[k]);
        for(uint32_t b=0;b<NPAT;b++){
            const pattern_t *p=pattern_at(k,b);int16_t time[4];
            for(uint32_t i=0;i<4u;i++)time[i]=runtime?p->timing[i]:(b!=active?TP[P_SLEN+i].def:q->t[k].p[P_SLEN+i]);
            for(uint32_t i=0;i<4u;i++)if(time[i]<TP[P_SLEN+i].min||time[i]>TP[P_SLEN+i].max){motion_unguard(f);return 0;}
            bank_timing_put(raw,k,b,time);
            if(b==active)continue;
            for(uint32_t i=0;i<NSTEP;i++,pos+=BANK_STEP_SIZE){step_t empty={{0},0,ST_REST};if(!bank_step_pack(raw+pos,runtime?&p->step[i]:&empty)){motion_unguard(f);return 0;}}
        }
    }
    if(runtime){
        for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<NTRK;k++)bank_tag_put(raw,BANK_CHAIN_OFF,r*NTRK+k,chain_patterns[r][k]);
        for(uint32_t i=0;i<MOTION_MAX;i++)bank_tag_put(raw,BANK_MOTION_OFF,i,motion_pattern[i]);
    }
    if(q->fm6_fn_ok){uint32_t m=BANK_FN_MAGIC;memcpy(raw+BANK_FN_OFF,&m,4u);memcpy(raw+BANK_FN_OFF+4u,q->fm6_fn,sizeof q->fm6_fn);}
    bank_tess_set(raw,q->tess_valid?q->tess:0);motion_unguard(f);return 1;
}
static uint8_t bank_import_tags[MOTION_MAX];
static int bank_valid(const uint8_t *raw,uint32_t len)
{
    uint32_t magic,size,sum,pos=BANK_EXTRA_OFF;
    if(!bank_full(len))return 0;
    memcpy(&magic,raw,4u);memcpy(&size,raw+4u,4u);memcpy(&sum,raw+len-4u,4u);
    if((magic!=BANK_MAGIC&&magic!=BANK_LEGACY_MAGIC)||size!=len||sum!=proj_hash(raw,len-4u)||!proj_import(&proj_scratch,raw+8u,BANK_PROJ_SIZE))return 0;
    for(uint32_t k=0;k<NTRK;k++){
        if(raw[BANK_ACTIVE_OFF+k]>=NPAT||raw[BANK_ACTIVE_OFF+k]!=proj_scratch.pattern[k])return 0;
        for(uint32_t b=0;b<NPAT;b++){
            for(uint32_t i=0;i<4u;i++){int32_t v=(int32_t)bank_timing_get(raw,k,b,i);if(v<TP[P_SLEN+i].min||v>TP[P_SLEN+i].max)return 0;if(b==raw[BANK_ACTIVE_OFF+k]&&v!=proj_scratch.t[k].p[P_SLEN+i])return 0;}
            if(b==raw[BANK_ACTIVE_OFF+k])continue;
            for(uint32_t i=0;i<NSTEP;i++,pos+=BANK_STEP_SIZE){step_t s;if(!step_wire_unpack(&s,raw+pos))return 0;}
        }
    }
    for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<NTRK;k++)if(bank_chain_get(raw,r,k)>=NPAT)return 0;
    for(uint32_t i=0;i<proj_scratch.motion.count;i++){
        if(bank_motion_get(raw,i)>=NPAT)return 0;
        for(uint32_t j=0;j<i;j++)if(bank_motion_get(raw,i)==bank_motion_get(raw,j)&&proj_scratch.motion.event[i].place==proj_scratch.motion.event[j].place&&proj_scratch.motion.event[i].param==proj_scratch.motion.event[j].param)return 0;
    }
    memset(bank_import_tags,0,sizeof bank_import_tags);
    for(uint32_t i=0;i<proj_scratch.motion.count;i++)bank_import_tags[i]=bank_motion_get(raw,i);
    uint32_t m,fo=bank_fn_off(raw);memcpy(&m,raw+fo,4u);
    if(m==BANK_FN_MAGIC){const uint8_t *fn=raw+fo+4u;for(uint32_t k=0;k<NTRK;k++)if(!fm6_fn_ok(fn+k*FM6_NFN))return 0;memcpy(proj_scratch.fm6_fn,fn,sizeof proj_scratch.fm6_fn);proj_scratch.fm6_fn_ok=1;}
    if(bank_custom(raw)){
        memcpy(&m,raw+BANK_TESS_OFF,4u);if(m!=0u&&m!=BANK_TESS_MAGIC)return 0;
        if(m){memcpy(proj_scratch.tess,raw+8u+PROJ_TESS_OFF,TESSERA_PK_BYTES);memcpy(proj_scratch.tess+TESSERA_PK_BYTES,raw+BANK_TESS_OFF+4u,TESS_FEATURE_BYTES);if(!tess_full_valid(proj_scratch.tess,TESS_FULL_BYTES))return 0;proj_scratch.tess_valid=1;}
    }
    proj_scratch.sum=proj_sum(&proj_scratch);return 1;
}
static void bank_restore(const uint8_t *raw)
{
    uint32_t pos=BANK_EXTRA_OFF,f=motion_guard();chain_config=proj_scratch.chain;
    for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<NTRK;k++)chain_patterns[r][k]=bank_chain_get(raw,r,k);
    memcpy(motion_pattern,bank_import_tags,sizeof motion_pattern);
    for(uint32_t k=0;k<NTRK;k++){
        trk[k].pattern=raw[BANK_ACTIVE_OFF+k];trk[k].pattern_next=0xff;trk[k].pattern_gen++;
        for(uint32_t b=0;b<NPAT;b++){
            pattern_t *p=pattern_at(k,b);for(uint32_t i=0;i<4u;i++)p->timing[i]=(int16_t)bank_timing_get(raw,k,b,i);
            if(b==trk[k].pattern)memcpy(p->step,trk[k].step,sizeof p->step);
            else for(uint32_t i=0;i<NSTEP;i++,pos+=BANK_STEP_SIZE)step_wire_unpack(&p->step[i],raw+pos);
        }
    }
    motion_unguard(f);
}
