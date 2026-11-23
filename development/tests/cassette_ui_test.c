/* SPDX-License-Identifier: GPL-3.0-only
 * Cassette UI state, redraw and production-engine coverage — By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>

static void snapshot(const char *dir, uint32_t engine, uint32_t font, uint32_t frame)
{
    char path[512];
    snprintf(path,sizeof path,"%s/engine-%u-font-%u-frame-%u.ppm",dir,engine,font,frame);
    FILE *f=fopen(path,"wb"); assert(f);
    fprintf(f,"P6\n240 240\n255\n");
    for(uint32_t i=0;i<240u*240u;i++){
        uint16_t c=swap16(host_screen[i]);
        uint8_t rgb[3]={(uint8_t)((c>>11)*255/31),(uint8_t)(((c>>5)&63)*255/63),(uint8_t)((c&31)*255/31)};
        fwrite(rgb,1,3,f);
    }
    assert(!fclose(f));
}
int main(int argc,char **argv)
{
    static const uint32_t engines[]={ENGI_PROPHET,ENGI_FM6,ENGI_WAVETABLE,ENGI_WAVE_LAYERS,ENGI_DRUM,ENGI_FMKIT};
    uint16_t before[240*240];
    for(uint32_t font=0;font<2;font++){
        ui_power_on();settings.palette=UI_CASSETTE_INDEX;palette_set(settings.palette);
        PREF_BITS=font?PREF_BITS|PREF_PIXEL:PREF_BITS&~PREF_PIXEL;
        go_home();
        for(uint32_t n=0;n<sizeof engines/sizeof engines[0];n++){
            set_engine_of(TSEL,engines[n]);
            int16_t params[sizeof TSEL->p / sizeof TSEL->p[0]];memcpy(params,TSEL->p,sizeof params);
            memset(scope_buf,0,sizeof scope_buf);stage.knob_ms=0;fm1_ms=10000;ui.force=1;ui_draw();
            assert(ux.instrument==(n<2?n+1:n<4?3:n==4?4:5));
            assert(T_THEME==T_KNOB(0));
            memcpy(before,host_screen,sizeof before);
            for(uint32_t i=0;i<SCOPE_N;i++)scope_buf[i]=(int16_t)(((int32_t)(i%64u)-32)*300);
            fm1_ms+=320;ui.force=1;ui_draw();
            assert(memcmp(before,host_screen,sizeof before));
            if(argc>1)snapshot(argv[1],engines[n],font,0);
            /* Animated redraws must leave the four parameter controls stable. */
            memcpy(before,host_screen,sizeof before);
            fm1_ms+=320;ui.force=1;ui_draw();
            assert(memcmp(before,host_screen,sizeof before));
            assert(!memcmp(before+Y_LABEL*240,host_screen+Y_LABEL*240,CARD_H*240*sizeof(uint16_t)));
            assert(!memcmp(params,TSEL->p,sizeof params));
            if(argc>1)snapshot(argv[1],engines[n],font,1);
            stage_knob_touch();ui.hot_col=0;ui.force=1;ui_draw();
            fm1_ms+=2000;ui.force=1;ui_draw();
            assert(!memcmp(params,TSEL->p,sizeof params));
            press(B_HOME);assert(ui.home&&ui.home_pg==HOME_TRACKS);ui.force=1;ui_draw();
            press(B_HOME);assert(ui.home&&ui.home_pg==HOME_CHORDS);ui.force=1;ui_draw();
            press(B_HOME);assert(ui.home&&!ui.home_pg);ui.force=1;ui_draw();
            /* Retired theme choices always resolve to the new production UI. */
            assert(NPALETTES==1u);
            for(uint32_t old=0;old<UI_LEGACY_PALETTES;old++){
                assert(palette_stored_ok(UI_PAL_TAG+old));
                assert(palette_from_stored(UI_PAL_TAG+old)==UI_CASSETTE_INDEX);
            }
            palette_set(7);settings.palette=0;ui.force=1;ui_draw();assert(ux.instrument);
        }
    }
    assert(!test_display_preferences());
    puts("cassette UI: six production engines, both fonts, live scope, stable controls, HOME navigation and retired theme migration passed");
    return 0;
}
