/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess navigation contracts — By: Inputsignwave. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>
#undef assert
#define assert(x) do {if(!(x)){fprintf(stderr,"studio8 assertion %s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
static void visibility(void)
{
    ui_power_on();tessera_reset();song.ntrk=4;
    for(uint32_t e=0;e<NENGINES;e++) {
        set_engine_of(TSEL,e);sec.ok=0;sec_build();
        for(uint32_t p=0;p<NPAGES;p++)if(PAGES[p].fam==FAM_EDIT && page_visible(p)) {
            assert(!generator_page(&PAGES[p]));
            uint32_t section=sec_of(p);assert(section<sec.n);
            assert(PAGES[p].scope==SC_FMOP || str_eq(sec.name[section],sec_rule(p,0)));
        }
        for(uint32_t mode=0;mode<TESS_ENG_COUNT;mode++) {
            tessera.mode[0]=mode;
            for(uint32_t p=0;p<NPAGES;p++) {
                uint32_t g=PAGES[p].graph;
                if(generator_page(&PAGES[p])) {
                    assert(PAGES[p].fam==FAM_SEQ);
                    int want=mode!=0;
                    if(g==GR_S7_AWAKE)want=mode==TESS_ENG_AWAKE;
                    if(g==GR_S7_POLY)want=mode==TESS_ENG_POLY;
                    if(g>=GR_S7_KIT && g<=GR_S7_ANCHOR)want=mode==TESS_ENG_KIT;
                    assert(page_visible(p)==want);
                }
            }
            assert(page_visible(page_titled("PATTERN")));
            assert(page_visible(page_titled("NOTE LENGTH")));
            assert(PAGES[page_titled("HARMONY")].fam==FAM_SEQ);
            assert(PAGES[page_titled("NOTE RANGE")].fam==FAM_SEQ);
        }
    }
    ui_power_on();song.ntrk=4;tessera.mode[0]=TESS_ENG_AWAKE;studio7_track[0].awake.prob=37;
    tessera.mode[1]=TESS_ENG_OFF;go_title("AWAKE EDIT");track_select(1);
    assert(cur_page()->fam==FAM_SEQ && page_visible(ui.page));
    track_select(0);tessera.mode[0]=TESS_ENG_OFF;navigation_fix();
    assert(studio7_track[0].awake.prob==37);
    tessera.mode[0]=TESS_ENG_AWAKE;assert(page_visible(page_titled("AWAKE EDIT")));
    go_home();press(B_GLO);assert(cur_page()->graph==GR_TRK);
    press(B_GLO);assert(cur_page()->graph==GR_OUTPUT && cur_page()->scope==SC_TRACK);
    press(B_GLO);assert(str_eq(cur_page()->title,"GLOBAL"));
    puts("studio8: all engines/modes, EDIT sections, SEQ ownership, hidden-page fallback, retained values and GLO order passed");
}
static void home_actions(void)
{
    ui_power_on();tessera_reset();song.ntrk=2;go_title("NOTES");cursor_set(3);
    uint32_t page=ui.page;hold(B_HOME);assert(ui.menu && ui.menu_sel==MI_CREATE);
    menu_close();assert(!ui.home && ui.page==page && ui.cursor==3 && song.seq_mode);
    TSEL->step[3]=(step_t){.time=ST_NOTE,.n=1,.note={60},.vel=100};
    bulk.on=1;bulk.track=song.sel;bulk.pattern=TSEL->pattern;bulk.engine=library_engine();
    bulk.generation=TSEL->pattern_gen;bulk.anchor=bulk.end=3;
    hold(B_HOME);assert(bulk.on);ui.menu_sel=MI_SELECTED;menu_input(2);
    assert(!ui.menu && pop.on==POP_SHEET && bulk.on && ui.page==page);
    pop_close();bulk_clear();hold(B_HOME);ui.menu_sel=MI_PATTERN;menu_input(2);
    assert(pop.rows==SHEET_HOME_PATTERN && pop.sub[1]=='1' && pop.sub[13]=='1');
    pop.sel=1;sheet_input(0,0,2);assert(ptc_on() && cur_page()->graph==GR_PATGRID && ui.ptc_src==0);
    ui_power_on();song.ntrk=1;go_home();hold(B_HOME);menu_input(2);
    assert(trk_n()==2 && song.sel==1 && !ui.menu);
    ui_power_on();song.ntrk=1;hold(B_HOME);ui.menu_sel=MI_TRACKS;menu_input(2);
    sheet_input(0,0,2);assert(!ui.confirm && trk_n()==1 && msg_is("ONLY ONE TRACK"));
    ui_power_on();song.ntrk=2;ui.song_row=3;hold(B_HOME);ui.menu_sel=MI_SONG;menu_input(2);
    assert(pop.rows==SHEET_SONG && ui.song_row==3);
    puts("studio8: HOME cancel preserves note context; selected notes, copy destination, track add/remove guard and song target passed");
}
static void algorithm(void)
{
    ui_power_on();song.ntrk=2;set_engine_of(TSEL,ENGI_FM6);go_title("ALGO");
    uint8_t before[sizeof fm6_patch[0]];memcpy(before,fm6_patch[0],sizeof before);
    for(int32_t v=0;v<32;v++) {
        fm6_patch[0][FP_ALG]=0;turn(EN_ALGO,v);assert(fm6_patch[0][FP_ALG]==v && song.sel==0);
    }
    turn(EN_ALGO,INT32_MAX);assert(fm6_patch[0][FP_ALG]==31);
    turn(EN_ALGO,INT32_MIN);assert(fm6_patch[0][FP_ALG]==0);
    before[FP_ALG]=0;assert(!memcmp(before,fm6_patch[0],sizeof before));
    go_title("PATTERN");turn(EN_ALGO,1);assert(song.sel==1);
    song.sel=0;set_engine_of(TSEL,ENGI_FM6);go_title("DESIGNER");act_do();assert(designer_on());
    designer_close(1);assert(str_eq(cur_page()->title,"DESIGNER"));
    puts("studio8: algorithms 1..32, extreme deltas, other patch bytes, SEQ track selection and EDIT Designer passed");
}
int main(void){setbuf(stdout,0);setbuf(stderr,0);visibility();home_actions();algorithm();return 0;}
