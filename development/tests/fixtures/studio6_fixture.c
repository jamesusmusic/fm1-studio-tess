/* SPDX-License-Identifier: GPL-3.0-only
 * Studio Tess migration fixture — By: Inputsignwave.
 * Build against the preserved Performance 6 source, not the new encoder. */
#define UI_TEST_NO_MAIN 1
#include "../../../performance6/source/tests/ui_test.c"
int main(int argc,char **argv)
{
    ui_power_on();tessera_reset();song.ntrk=4;
    for(uint32_t tr=0;tr<4;tr++)for(uint32_t b=0;b<8;b++){
        pattern_request(&trk[tr],b);trk[tr].p[P_SLEN]=32+b;trk[tr].p[P_SSWING]=b;
        for(uint32_t i=0;i<64;i++){
            step_t *s=&trk[tr].step[i];memset(s,0,sizeof *s);
            s->time=i%3;s->n=s->time==ST_NOTE?4:0;s->vel=1+(i*3+tr+b)%127;
            for(uint32_t j=0;j<4;j++)s->note[j]=20+(tr*17+b*5+i+j)%100;
            s->flags=(i+b)%8;s->hit=(uint8_t)(i*13+b*17+tr);s->acc=(uint8_t)(s->hit&(i*7+3));
            if(!s->hit&&s->time!=ST_REST)s->acc=(uint8_t)(i+1);s->probability=(i+b)%102;
        }
    }
    tessera.mode[0]=TESS_ENG_EUCLID;tessera.euclid[0].pulses=7;
    project_capture(&proj_scratch);if(!bank_pack(proj_wire_u.raw,&proj_scratch,1))return 1;
    FILE *f=fopen(argc>1?argv[1]:"tests/fixtures/studio6-fbkt.bin","wb");if(!f)return 2;
    int ok=fwrite(proj_wire_u.raw,1,BANK_STORE_SIZE,f)==BANK_STORE_SIZE;fclose(f);return !ok;
}
