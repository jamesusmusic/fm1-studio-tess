/* SPDX-License-Identifier: GPL-3.0-only
 * HOME overview, direct selection and gesture lifetime — By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
int main(void)
{
    assert(HOME_PAGES==3u && NPALETTES==1u);
    for(uint32_t font=0;font<2;font++)for(uint32_t count=1;count<=NTRK;count++){
        ui_power_on();song.ntrk=(uint8_t)count;go_home();
        PREF_BITS=font?PREF_BITS|PREF_PIXEL:PREF_BITS&~PREF_PIXEL;
        HOME_SELECT=HS_BPM;
        int16_t bpm=song.g[G_BPM];
        input_on(&trk[0],60,100);
        uint32_t held[4];memcpy(held,live_held[0],sizeof held);
        turn(EN_ALGO,3);assert(song.sel==count-1u && !ui.home_pg);
        turn(EN_ALGO,-99);assert(song.sel==0u);
        stage_knob_touch();frame();
        press(B_HOME);assert(ui.home_pg==HOME_TRACKS && !stage.out && !stage_knobs_out());
        turn(EN_SELECT,3);assert(song.sel==count-1u && song.g[G_BPM]==bpm);
        turn(EN_SELECT,-99);assert(song.sel==0u);
        assert(!memcmp(held,live_held[0],sizeof held));
        turn(EN_K1,1);assert(stage_knobs_out());frame();
        if(count>1){
            turn(EN_ALGO,1);assert(song.sel==1u && !stage.out && !stage_knobs_out());
            turn(EN_SELECT,-1);assert(song.sel==0u);
        }
        press(B_HOME);assert(ui.home_pg==HOME_CHORDS);
        press(B_HOME);assert(!ui.home_pg);
        press(B_EDIT);assert(!ui.home);
        press(B_HOME);assert(ui.home&&ui.home_pg==HOME_TRACKS);
        uint8_t page=ui.home_pg;hold(B_HOME);assert(ui.menu&&ui.home_pg==page);press(B_OCTDN);
        input_off(&trk[0],60);assert(!live_held[0][1]);
        ui.force=1;ui_draw();
    }
    puts("HOME tracks: 1..4 tracks, both fonts, three views, direct ALGO/SELECT picking, fast/clamped turns, gesture dismissal, EDIT return, HOME hold and held-note ownership passed");
    return 0;
}
