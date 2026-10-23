/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Kerem Kilic (Ellic Studio)
 * Studio 7 lossless bank extension — By: Inputsignwave. */
#include "sound_step.h"
/* FBKT retains the existing 27,904-byte A/B allocation and every step/event.
 * Timing triples and 3-bit bank tags reclaim 112 bytes. Tessera's first 180
 * bytes use the FUN22 reserved gap; HMF1 uses the reclaimed bank tail. FBKK
 * imports remain read-only until SAVE or the protected TSR migration. */
#define BANK_LEGACY_MAGIC 0x4B4B4246u
#define BANK_V6_MAGIC 0x544B4246u /* FBKT */
#define BANK_V7_MAGIC 0x374B4246u /* FBK7: 61-bit inactive steps */
#define BANK_MAGIC 0x384B4246u /* FBK8: lossless four-step groups, same allocation */
#define BANK_STORE_SIZE PROJ_BANK_BYTES
#define BANK_LEGACY_SIZE BANK_STORE_SIZE
#define BANK_PROJ_SIZE PROJ_STORE_SIZE
#define BANK_STEP_SIZE 8u
#define BANK_ACTIVE_OFF (8u + BANK_PROJ_SIZE)
#define BANK_EXTRA_OFF (BANK_ACTIVE_OFF + NTRK)
#define BANK_STEP_COUNT (NTRK * (NPAT - 1u) * NSTEP)
#define BANK_V7_SAVED_BYTES (BANK_STEP_COUNT * 3u / 8u)
#define BANK_V8_GAIN (BANK_STEP_COUNT / 4u * 3u / 8u)
#define BANK_SAVED_BYTES (BANK_V7_SAVED_BYTES + BANK_V8_GAIN)
#define BANK_TIMING_OFF (BANK_EXTRA_OFF + BANK_STEP_COUNT / 4u * S10_GROUP_BITS / 8u)
#define BANK_V6_TIMING_OFF (BANK_TIMING_OFF + BANK_SAVED_BYTES)
#define BANK_TIME_SIZE 3u
#define BANK_CHAIN_OFF (BANK_TIMING_OFF + NTRK * NPAT * BANK_TIME_SIZE)
#define BANK_CHAIN_BYTES ((CHAIN_ROWS*NTRK*3u+7u)/8u)
#define BANK_MOTION_OFF (BANK_CHAIN_OFF + BANK_CHAIN_BYTES)
#define BANK_MOTION_BYTES ((MOTION_MAX*3u+7u)/8u)
#define BANK_FN_OFF (BANK_MOTION_OFF + BANK_MOTION_BYTES)
#define BANK_FN_MAGIC 0x31364E46u
#define BANK_OLD_CHAIN_OFF (BANK_V6_TIMING_OFF + NTRK * NPAT * 4u)
#define BANK_OLD_MOTION_OFF (BANK_OLD_CHAIN_OFF + CHAIN_ROWS*NTRK)
#define BANK_OLD_FN_OFF (BANK_OLD_MOTION_OFF + MOTION_MAX)
#define BANK_TESS_OFF (BANK_FN_OFF + 4u + NTRK*FM6_NFN)
#define BANK_TESS_MAGIC 0x34435354u
#define BANK_S7_OFF (BANK_TESS_OFF + 4u + TESS_FEATURE_BYTES)
#define BANK_SFX_OFF (BANK_S7_OFF+S7_STATE_BYTES)
_Static_assert(BANK_SFX_OFF+SFX_WIRE_BYTES<=BANK_STORE_SIZE-4u,"FX state fits atomic bank");
_Static_assert(BANK_SAVED_BYTES == 840u,"lossless extension capacity");
_Static_assert(BANK_S7_OFF+S7_STATE_BYTES <= BANK_STORE_SIZE-4u,"Studio 7 fits existing A/B allocation");
#define PROJ_TESS_OFF (PROJ_TONE_WIRE - TESSERA_PK_BYTES)
_Static_assert(NELEM(N_DIV) <= 16u,"packed bank division field");
_Static_assert(BANK_TESS_OFF + 4u + TESS_FEATURE_BYTES <= BANK_STORE_SIZE-4u,"Tess state fits the existing project allocation");
_Static_assert(68u+NTRK*(P_COUNT+2u+NSTEP*8u)+sizeof(chain_config_t)+sizeof(motion_store_t)+2u+2u*NTRK <= PROJ_TESS_OFF,"Tessera uses only the reserved FUN22 gap");
static int bank_full(uint32_t n) { return n == BANK_STORE_SIZE; }
static int bank_s8(const uint8_t *raw) { uint32_t m;memcpy(&m,raw,4u);return m==BANK_MAGIC; }
static int bank_s7(const uint8_t *raw) { uint32_t m;memcpy(&m,raw,4u);return m==BANK_MAGIC||m==BANK_V7_MAGIC; }
static int bank_custom(const uint8_t *raw) { uint32_t m;memcpy(&m,raw,4u);return m==BANK_MAGIC||m==BANK_V7_MAGIC||m==BANK_V6_MAGIC; }
static uint32_t bank_offset(const uint8_t *raw,uint32_t off) { return off+(bank_s8(raw)?0u:bank_s7(raw)?BANK_V8_GAIN:BANK_SAVED_BYTES); }
static uint32_t bank_fn_off(const uint8_t *raw) { return bank_custom(raw)?bank_offset(raw,BANK_FN_OFF):BANK_OLD_FN_OFF; }
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
{ return bank_custom(raw)?bank_tag_get(raw,bank_offset(raw,BANK_CHAIN_OFF),row*NTRK+track):raw[BANK_OLD_CHAIN_OFF+row*NTRK+track]; }
static uint8_t bank_motion_get(const uint8_t *raw,uint32_t i)
{ return bank_custom(raw)?bank_tag_get(raw,bank_offset(raw,BANK_MOTION_OFF),i):raw[BANK_OLD_MOTION_OFF+i]; }
static uint32_t bank_timing_get(const uint8_t *raw,uint32_t track,uint32_t bank,uint32_t field)
{
    uint32_t index=track*NPAT+bank;
    if(!bank_custom(raw))return raw[BANK_V6_TIMING_OFF+index*4u+field];
    uint32_t pos=index*24u+ (field==0u?0u:field==1u?6u:field==2u?10u:17u);
    return wire_bits_get(raw+bank_offset(raw,BANK_TIMING_OFF),&pos,field==0u?6u:field==1u?4u:7u)+(field==0u);
}
static void bank_timing_put(uint8_t *raw,uint32_t track,uint32_t bank,const int16_t *timing)
{
    uint32_t pos=0;uint8_t *v=raw+BANK_TIMING_OFF+(track*NPAT+bank)*3u;memset(v,0,3u);
    wire_bits_put(v,&pos,(uint32_t)timing[0]-1u,6u);wire_bits_put(v,&pos,(uint32_t)timing[1],4u);
    wire_bits_put(v,&pos,(uint32_t)timing[2],7u);wire_bits_put(v,&pos,(uint32_t)timing[3],7u);
}
static void bank_checksum(uint8_t *raw)
{ uint32_t sum=proj_hash(raw,BANK_STORE_SIZE-4u);memcpy(raw+BANK_STORE_SIZE-4u,&sum,4u); }
/* Call only on a validated container. Conversion proceeds toward lower
 * addresses; each old record is copied before its bytes can be overwritten. */
static void bank_upgrade_layout(uint8_t *raw)
{
    if(bank_s8(raw))return;
    int seven=bank_s7(raw);
    if(!bank_custom(raw)){
        for(uint32_t k=0;k<NTRK;k++)for(uint32_t b=0;b<NPAT;b++){
            int16_t time[4];for(uint32_t i=0;i<4u;i++)time[i]=(int16_t)raw[BANK_V6_TIMING_OFF+(k*NPAT+b)*4u+i];
            uint32_t pos=0;uint8_t *v=raw+BANK_V6_TIMING_OFF+(k*NPAT+b)*3u;
            memset(v,0,3u);wire_bits_put(v,&pos,time[0]-1u,6u);wire_bits_put(v,&pos,time[1],4u);
            wire_bits_put(v,&pos,time[2],7u);wire_bits_put(v,&pos,time[3],7u);
        }
        uint8_t tags[MOTION_MAX];
        memcpy(tags,raw+BANK_OLD_CHAIN_OFF,CHAIN_ROWS*NTRK);memset(raw+BANK_CHAIN_OFF+BANK_SAVED_BYTES,0,BANK_CHAIN_BYTES);
        for(uint32_t i=0;i<CHAIN_ROWS*NTRK;i++)bank_tag_put(raw,BANK_CHAIN_OFF+BANK_SAVED_BYTES,i,tags[i]);
        memcpy(tags,raw+BANK_OLD_MOTION_OFF,MOTION_MAX);memset(raw+BANK_MOTION_OFF+BANK_SAVED_BYTES,0,BANK_MOTION_BYTES);
        for(uint32_t i=0;i<MOTION_MAX;i++)bank_tag_put(raw,BANK_MOTION_OFF+BANK_SAVED_BYTES,i,tags[i]);
        uint8_t fn[4u+NTRK*FM6_NFN];memcpy(fn,raw+BANK_OLD_FN_OFF,sizeof fn);memcpy(raw+BANK_FN_OFF+BANK_SAVED_BYTES,fn,sizeof fn);
        memset(raw+BANK_TESS_OFF+BANK_SAVED_BYTES,0,4u+TESS_FEATURE_BYTES);
    }
    uint32_t bit=0,oldbit=0;
    for(uint32_t i=0;i<BANK_STEP_COUNT;i+=4u){
        uint8_t old[4][8];
        for(uint32_t j=0;j<4u;j++){
            if(seven)s7_step_unpack(old[j],raw+BANK_EXTRA_OFF,&oldbit);
            else memcpy(old[j],raw+BANK_EXTRA_OFF+(i+j)*8u,8u);
        }
        s10_steps_pack(raw+BANK_EXTRA_OFF,BANK_STEP_COUNT/4u*S10_GROUP_BITS,&bit,old);
    }
    volatile uint8_t *move=raw; /* forward overlap, no freestanding memmove dependency */
    uint32_t delta=seven?BANK_V8_GAIN:BANK_SAVED_BYTES;
    uint32_t end=BANK_S7_OFF+(seven?S7_STATE_BYTES:0u);
    for(uint32_t i=BANK_TIMING_OFF;i<end;i++)move[i]=move[i+delta];
    memset(raw+end,0,BANK_STORE_SIZE-4u-end);
    uint32_t m=BANK_MAGIC;memcpy(raw,&m,4u);
}
static int bank_step_read(step_t *s,const uint8_t *raw,uint32_t *bit)
{
    uint8_t wire[8];
    if(bank_s8(raw)){
        uint8_t group[4][8];uint32_t index=*bit/STUDIO7_STEP_BITS,p=index/4u*S10_GROUP_BITS;
        if(!s10_steps_unpack(group,raw+BANK_EXTRA_OFF,BANK_STEP_COUNT/4u*S10_GROUP_BITS,&p))return 0;
        memcpy(wire,group[index%4u],8u);*bit+=STUDIO7_STEP_BITS;
    }
    else if(bank_s7(raw)){if(!s7_step_unpack(wire,raw+BANK_EXTRA_OFF,bit))return 0;}
    else {memcpy(wire,raw+BANK_EXTRA_OFF+*bit/8u,8u);*bit+=64u;}
    return step_wire_unpack(s,wire);
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
    uint32_t magic=BANK_MAGIC,size=BANK_STORE_SIZE,pos=0,f=motion_guard();
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
            for(uint32_t i=0;i<NSTEP;i+=4u){
                step_t empty={{0},0,ST_REST};uint8_t wire[4][8];
                for(uint32_t j=0;j<4u;j++)if(!bank_step_pack(wire[j],runtime?&p->step[i+j]:&empty)){motion_unguard(f);return 0;}
                if(!s10_steps_pack(raw+BANK_EXTRA_OFF,BANK_STEP_COUNT/4u*S10_GROUP_BITS,&pos,wire)){motion_unguard(f);return 0;}
            }
        }
    }
    if(runtime){
        for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<NTRK;k++)bank_tag_put(raw,BANK_CHAIN_OFF,r*NTRK+k,chain_patterns[r][k]);
        for(uint32_t i=0;i<MOTION_MAX;i++)bank_tag_put(raw,BANK_MOTION_OFF,i,motion_pattern[i]);
    }
    if(q->fm6_fn_ok){uint32_t m=BANK_FN_MAGIC;memcpy(raw+BANK_FN_OFF,&m,4u);memcpy(raw+BANK_FN_OFF+4u,q->fm6_fn,sizeof q->fm6_fn);}
    if(q->s7_valid){if(!s7_state_decode(0,q->s7,S7_STATE_BYTES)){motion_unguard(f);return 0;}memcpy(raw+BANK_S7_OFF,q->s7,S7_STATE_BYTES);}
    if(q->sound_fx_valid && !sfx_encode(raw+BANK_SFX_OFF,&q->sound_fx)){motion_unguard(f);return 0;}
    bank_tess_set(raw,q->tess_valid?q->tess:0);motion_unguard(f);return 1;
}
static uint8_t bank_import_tags[MOTION_MAX];
static int bank_valid(const uint8_t *raw,uint32_t len)
{
    uint32_t magic,size,sum,pos=0;
    if(!bank_full(len))return 0;
    memcpy(&magic,raw,4u);memcpy(&size,raw+4u,4u);memcpy(&sum,raw+len-4u,4u);
    if((magic!=BANK_MAGIC&&magic!=BANK_V7_MAGIC&&magic!=BANK_V6_MAGIC&&magic!=BANK_LEGACY_MAGIC)||size!=len||sum!=proj_hash(raw,len-4u)||!proj_import(&proj_scratch,raw+8u,BANK_PROJ_SIZE))return 0;
    for(uint32_t k=0;k<NTRK;k++){
        if(raw[BANK_ACTIVE_OFF+k]>=NPAT||raw[BANK_ACTIVE_OFF+k]!=proj_scratch.pattern[k])return 0;
        for(uint32_t b=0;b<NPAT;b++){
            for(uint32_t i=0;i<4u;i++){int32_t v=(int32_t)bank_timing_get(raw,k,b,i);if(v<TP[P_SLEN+i].min||v>TP[P_SLEN+i].max)return 0;if(b==raw[BANK_ACTIVE_OFF+k]&&v!=proj_scratch.t[k].p[P_SLEN+i])return 0;}
            if(b==raw[BANK_ACTIVE_OFF+k])continue;
            for(uint32_t i=0;i<NSTEP;i++){step_t s;if(!bank_step_read(&s,raw,&pos))return 0;}
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
        uint32_t to=bank_offset(raw,BANK_TESS_OFF);memcpy(&m,raw+to,4u);if(m!=0u&&m!=BANK_TESS_MAGIC)return 0;
        if(m){memcpy(proj_scratch.tess,raw+8u+PROJ_TESS_OFF,TESSERA_PK_BYTES);memcpy(proj_scratch.tess+TESSERA_PK_BYTES,raw+to+4u,TESS_FEATURE_BYTES);if(!tess_full_valid(proj_scratch.tess,TESS_FULL_BYTES))return 0;proj_scratch.tess_valid=1;}
    }
    if(bank_s7(raw)){
        uint32_t so=bank_offset(raw,BANK_S7_OFF),tag;memcpy(&tag,raw+so,4u);
        if(tag){if(!s7_state_decode(0,raw+so,S7_STATE_BYTES))return 0;memcpy(proj_scratch.s7,raw+so,S7_STATE_BYTES);proj_scratch.s7_valid=1;}
        else for(uint32_t i=0;i<S7_STATE_BYTES;i++)if(raw[so+i])return 0;
        uint32_t tail=so+S7_STATE_BYTES;
        if(bank_s8(raw) && raw[tail]){
            if(!sfx_decode(&proj_scratch.sound_fx,raw+tail))return 0;
            proj_scratch.sound_fx_valid=1;tail+=SFX_WIRE_BYTES;
        }
        for(uint32_t i=tail;i<BANK_STORE_SIZE-4u;i++)if(raw[i])return 0;
    }
    proj_scratch.sum=proj_sum(&proj_scratch);return 1;
}
static void bank_restore(const uint8_t *raw)
{
    uint32_t pos=0,f=motion_guard();chain_config=proj_scratch.chain;
    for(uint32_t r=0;r<CHAIN_ROWS;r++)for(uint32_t k=0;k<NTRK;k++)chain_patterns[r][k]=bank_chain_get(raw,r,k);
    memcpy(motion_pattern,bank_import_tags,sizeof motion_pattern);
    for(uint32_t k=0;k<NTRK;k++){
        trk[k].pattern=raw[BANK_ACTIVE_OFF+k];trk[k].pattern_next=0xff;trk[k].pattern_gen++;
        for(uint32_t b=0;b<NPAT;b++){
            pattern_t *p=pattern_at(k,b);for(uint32_t i=0;i<4u;i++)p->timing[i]=(int16_t)bank_timing_get(raw,k,b,i);
            if(b==trk[k].pattern)memcpy(p->step,trk[k].step,sizeof p->step);
            else for(uint32_t i=0;i<NSTEP;i++)bank_step_read(&p->step[i],raw,&pos);
        }
    }
    motion_unguard(f);
}
