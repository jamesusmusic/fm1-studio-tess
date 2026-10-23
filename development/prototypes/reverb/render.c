/* SPDX-License-Identifier: MIT
 * Offline audition harness — By: Inputsignwave. Not target audio code. */
#include "plate.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>
static int32_t pole(double hz) { return (int32_t)lround((1-exp(-6.283185307179586*hz/PLATE_RATE))*32768); }
int main(int argc,char **argv)
{
    /* raw mono PCM16 in, raw stereo PCM16 wet out; RT60, damping Hz,
     * tone [-1,1], low/high cut Hz, width [0,1], mod Hz, depth ms, predelay ms. */
    if(argc!=12){fprintf(stderr,"usage: render in.raw out.raw rt60 damping tone lowcut highcut width modHz depthMs predelayMs\n");return 2;}
    plate_state s;int16_t memory[PLATE_SAMPLES];assert(plate_init(&s,memory,PLATE_SAMPLES));
    double v[9];for(int n=0;n<9;n++)v[n]=strtod(argv[n+3],NULL);
    plate_params p={0};
    p.feedback=(int32_t)lround(pow(.001,.212/v[0])*32768);
    p.damping=pole(v[1]);p.tone=(int32_t)lround(v[2]*16384);
    p.lowcut=pole(v[3]);p.highcut=pole(v[4]);p.width=(int32_t)lround(v[5]*32768);
    p.increment=(uint32_t)llround(v[6]*4294967296.0/PLATE_RATE);
    p.depth=(int32_t)lround(v[7]*PLATE_RATE*256/1000);
    p.predelay=(int32_t)lround(v[8]*PLATE_RATE/1000);plate_set(&s,&p);
    FILE *in=fopen(argv[1],"rb"),*out=fopen(argv[2],"wb");assert(in && out);
    uint64_t count=0;int32_t peak=0;int16_t x;clock_t begin=clock();
    while(fread(&x,sizeof x,1,in)==1){int32_t l,r;plate_tick(&s,x,&l,&r);
        if(abs(l)>peak)peak=abs(l);if(abs(r)>peak)peak=abs(r);
        int16_t pair[2]={(int16_t)l,(int16_t)r};assert(fwrite(pair,sizeof pair,1,out)==1);count++;}
    assert(!ferror(in));assert(!fclose(in));assert(!fclose(out));
    printf("{\"rate\":%d,\"delay_bytes\":%zu,\"state_bytes_host\":%zu,\"frames\":%llu,\"peak\":%d,\"tank_saturations\":%u,\"host_cpu_seconds\":%.6f}\n",PLATE_RATE,sizeof memory,sizeof s,(unsigned long long)count,peak,s.saturations,(double)(clock()-begin)/CLOCKS_PER_SEC);
    return 0;
}
