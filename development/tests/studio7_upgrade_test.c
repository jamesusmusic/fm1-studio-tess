/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess migration verification — By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
int main(void)
{
    ui_power_on();static uint8_t raw[BANK_STORE_SIZE],copy[BANK_STORE_SIZE],seven[BANK_STORE_SIZE];
    FILE *f=fopen("tests/fixtures/studio6-fbkt.bin","rb");assert(f&&fread(raw,1,sizeof raw,f)==sizeof raw);fclose(f);
    uint32_t magic;memcpy(&magic,raw,4);assert(magic==BANK_V6_MAGIC&&bank_valid(raw,sizeof raw));
    assert(!proj_scratch.s7_valid&&proj_scratch.tess_valid);
    assert(!project_restore_runtime(&proj_scratch));bank_restore(raw);
    uint32_t hashes[4][8];for(uint32_t tr=0;tr<4;tr++)for(uint32_t b=0;b<8;b++)hashes[tr][b]=proj_hash(pattern_at(tr,b),sizeof(pattern_t));
    memcpy(copy,raw,sizeof raw);bank_upgrade_layout(copy);bank_checksum(copy);
    assert(bank_s7(copy)&&bank_valid(copy,sizeof copy)&&!proj_scratch.s7_valid);
    assert(!project_restore_runtime(&proj_scratch));bank_restore(copy);
    for(uint32_t tr=0;tr<4;tr++)for(uint32_t b=0;b<8;b++)assert(hashes[tr][b]==proj_hash(pattern_at(tr,b),sizeof(pattern_t)));
    assert(tessera.mode[0]==TESS_ENG_EUCLID&&tessera.euclid[0].pulses==7);
    assert(!bank_s7(raw)); /* original fixture remains unchanged */
    studio7_track[0].kit.anchors[63]=255;studio7_track[0].meter.drift=-100;
    project_capture(&proj_scratch);assert(bank_pack(copy,&proj_scratch,1)&&bank_valid(copy,sizeof copy));
    /* Construct the previous FBK7 layout with its actual 61-bit encoder,
     * preserving a nonempty S7 state. Its source remains unchanged on import. */
    memcpy(seven,copy,sizeof seven);
    uint32_t to=0,from=0;
    for(uint32_t i=0;i<BANK_STEP_COUNT;i++){
        step_t step;uint8_t wire[8];assert(bank_step_read(&step,copy,&from));
        assert(bank_step_pack(wire,&step)&&s7_step_pack(seven+BANK_EXTRA_OFF,&to,wire));
    }
    memcpy(seven+BANK_TIMING_OFF+BANK_V8_GAIN,copy+BANK_TIMING_OFF,BANK_S7_OFF+S7_STATE_BYTES-BANK_TIMING_OFF);
    memset(seven+BANK_S7_OFF+S7_STATE_BYTES+BANK_V8_GAIN,0,BANK_STORE_SIZE-4u-BANK_S7_OFF-S7_STATE_BYTES-BANK_V8_GAIN);
    magic=BANK_V7_MAGIC;memcpy(seven,&magic,4);bank_checksum(seven);
    assert(bank_valid(seven,sizeof seven)&&proj_scratch.s7_valid);
    uint32_t oldhash=proj_hash(seven,sizeof seven);
    assert(!project_restore_runtime(&proj_scratch));bank_restore(seven);
    for(uint32_t tr=0;tr<4;tr++)for(uint32_t b=0;b<8;b++)hashes[tr][b]=proj_hash(pattern_at(tr,b),sizeof(pattern_t));
    assert(oldhash==proj_hash(seven,sizeof seven));
    bank_upgrade_layout(seven);bank_checksum(seven);
    assert(bank_s8(seven)&&bank_valid(seven,sizeof seven)&&proj_scratch.s7_valid);
    assert(!project_restore_runtime(&proj_scratch));bank_restore(seven);
    for(uint32_t tr=0;tr<4;tr++)for(uint32_t b=0;b<8;b++)assert(hashes[tr][b]==proj_hash(pattern_at(tr,b),sizeof(pattern_t)));
    assert(studio7_track[0].meter.drift==-100&&studio7_track[0].kit.anchors[63]==255);
    assert(BANK_STORE_SIZE-4u-(BANK_S7_OFF+S7_STATE_BYTES)==204u);
    uint8_t before[S7_STATE_BYTES];memcpy(before,copy+BANK_S7_OFF,sizeof before);
    bank_tess_set(copy,proj_scratch.tess);assert(!memcmp(before,copy+BANK_S7_OFF,sizeof before));
    copy[BANK_S7_OFF+S7_STATE_BYTES]=1;bank_checksum(copy);assert(!bank_valid(copy,sizeof copy));
    assert(studio7_track[0].meter.drift==-100);
    puts("studio7: frozen Performance 6 FBKT imports/upgrades all 32 banks losslessly; old custom writes preserve S7G1; malformed tail refused");
    return 0;
}
