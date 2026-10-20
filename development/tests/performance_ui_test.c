/* SPDX-License-Identifier: GPL-3.0-only */
#define UI_TEST_NO_MAIN
#include "ui_test.c"
#include <assert.h>
int main(void)
{
    ui_power_on();engine_load_of(TSEL,ENGI_FM6,0);
    ui.home=0;ui.page=page_titled("NOTE LENGTH");
    edit_param(0,1);edit_param(1,1);edit_param(2,20);edit_param(3,1);
    assert(performance[0].mode==DUR_FIXED && performance[0].length==3 && performance[0].fade==20 && performance[0].overlap);
    ui.page=page_titled("FM AMP");assert(page_visible(ui.page));
    edit_param(0,30);edit_param(1,10);edit_param(2,-50);edit_param(3,20);
    ui.page=page_titled("FM AMP MODE");edit_param(0,1);assert(performance[0].amp);
    ui.page=page_titled("FM TONE");edit_param(0,-20);edit_param(1,10);edit_param(2,12);
    performance_t want=performance[0];
    int saved=up_store_to(0,0,"PERF SOUND");assert(saved==0 || saved==3);assert(up_used(0));
    uint8_t native[128],extension[16];memcpy(native,up_rec(0)->packed,128);memcpy(extension,up_rec(0)->packed+128,16);
    native[110]=(native[110]+1u)%32u;
    int updated=native_update(ENGI_FM6,0,native);assert(updated==0 || updated==3);
    assert(!memcmp(extension,up_rec(0)->packed+128,16));
    int renamed=up_rename(0,"PERF RENAMED");assert(renamed==0 || renamed==3);
    assert(!memcmp(extension,up_rec(0)->packed+128,16));
    apply_preset(0);assert(!performance[0].amp && performance[0].brightness==64);
    undo_clear();
    assert(!up_load(0));assert(!memcmp(&performance[0],&want,sizeof want));
    undo_step(0);assert(!performance[0].amp);undo_step(1);assert(performance[0].amp);
    assert(native_page(page_titled("FM AMP"),FAM_ENV) && native_page(page_titled("FM TONE"),FAM_ENV));
    puts("performance UI: note controls, FM AMP/TONE, ENV navigation, preset bypass defaults, FM sound save/reload and sound undo/redo passed");
    return 0;
}
