/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess migration verification — By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
int main(void)
{
    ui_power_on();static uint8_t raw[BANK_STORE_SIZE],copy[BANK_STORE_SIZE];
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
    uint8_t before[S7_STATE_BYTES];memcpy(before,copy+BANK_S7_OFF,sizeof before);
    bank_tess_set(copy,proj_scratch.tess);assert(!memcmp(before,copy+BANK_S7_OFF,sizeof before));
    copy[BANK_S7_OFF+S7_STATE_BYTES]=1;bank_checksum(copy);assert(!bank_valid(copy,sizeof copy));
    assert(studio7_track[0].meter.drift==-100);
    puts("studio7: frozen Performance 6 FBKT imports/upgrades all 32 banks losslessly; old custom writes preserve S7G1; malformed tail refused");
    return 0;
}
