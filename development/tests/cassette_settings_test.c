/* SPDX-License-Identifier: GPL-3.0-only
 * Retired UI settings migration without collateral reset — By: Inputsignwave. */
#define PERSISTENCE_TEST_NO_MAIN
#include "persistence_test.c"
#include <assert.h>
int main(void)
{
    reset();settings_init();settings_hold=2;settings_boot=2;settings_screen=3;
    settings.zoom=1;panel.enc[0]=3;favorites.user=1u<<4;
    persist_t canonical={0};settings_export(&canonical);
    assert(NPALETTES==1u && sizeof canonical==604u);
    for(uint32_t old=0;old<UI_LEGACY_PALETTES;old++){
        persist_t saved=canonical;saved.palette=UI_PAL_TAG+old;
        assert(settings_legacy_valid(&saved));
        persist_t before=saved;
        assert(settings_import(&saved,sizeof saved));
        assert(settings.palette==0u && saved.palette==UI_PAL_TAG);
        assert(settings_hold==2u && settings_boot==2u && settings_screen==3u);
        assert(settings.zoom==1u && panel.enc[0]==3 && favorites.user==(1u<<4));
        saved.palette=before.palette;
        assert(!memcmp(&saved,&before,sizeof saved));
        persist_t exported={0};settings_export(&exported);assert(exported.palette==UI_PAL_TAG);
    }
    assert(!palette_stored_ok(UI_PAL_TAG-1u));
    assert(!palette_stored_ok(UI_PAL_TAG+UI_LEGACY_PALETTES));
    puts("cassette settings: all eight tagged theme IDs migrate; hold, boot, screen, zoom, calibration and favourites preserved; malformed IDs rejected");
    return 0;
}
