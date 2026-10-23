/* SPDX-License-Identifier: GPL-3.0-only
 * Offline native Prophet audition adapter — By: Inputsignwave.
 * Uses the existing Melodee/Felucca renderer with retained upstream notices.
 * No MIDI device access, persistent storage writes or firmware changes. */
#define main hostsim_main
#include "../tests/hostsim.c"
#undef main
#include <assert.h>
int main(int argc,char **argv)
{
    assert(argc==5); /* native .syx, generic phrase kind, output.wav, report.json */
    FILE *in=fopen(argv[1],"rb");assert(in);uint8_t raw[P5_FRAME_MAX+1];size_t n=fread(raw,1,sizeof raw,in);fclose(in);
    p5_patch_t patch;assert(p5_patch_decode(&patch,raw,(uint32_t)n));
    uint8_t encoded[P5_FRAME_MAX];assert(p5_patch_encode(&patch,encoded,sizeof encoded)==n&&!memcmp(encoded,raw,n));
    host_tracks_init();host_preset(&trk[0],ENGI_PROPHET,0);
    p5_patch[0]=patch;p5_ready[0]=1;p5_track_accept(&trk[0]);
    trk[0].p[P_LEVEL]=48;
    for(uint32_t k=0;k<8;k++)assert(trk[0].p[P_E0+k]==0);
    int bass=!strcmp(argv[2],"bass"),arp=!strcmp(argv[2],"arp");
    uint32_t frames=FS*12u,previous=0;int32_t peak=0;uint32_t clips=0;
    FILE *out=fopen(argv[3],"wb");assert(out);wav_hdr(out,frames);
    uint8_t notes[4]={bass?36:60,bass?43:64,bass?39:67,bass?46:72};
    for(uint32_t at=0;at<frames;at+=CTL){
        uint32_t bits=0;
        if(arp){if(at>=FS/4 && at<FS*6u){uint32_t step=(at-FS/4)/(FS/3);if((at-FS/4)%(FS/3)<FS/5)bits=1u<<(step%4);}}
        else if(bass){
            if(at>=FS/4 && at<FS*6u){uint32_t step=(at-FS/4)/(FS*3/4);if((at-FS/4)%(FS*3/4)<FS/2)bits=1u<<(step%4);}
        }else{
            if(at>=FS/4 && at<FS*5u/4)bits=1;
            if(at>=FS*3u/2 && at<FS*5u/2)bits=4;
            if(at>=FS*3u && at<FS*7u)bits=15;
        }
        for(uint32_t k=0;k<4;k++){
            if((previous&(1u<<k))&&!(bits&(1u<<k)))trk_note_off(&trk[0],notes[k]);
            if((bits&(1u<<k))&&!(previous&(1u<<k)))trk_note_on(&trk[0],notes[k],100);
        }
        previous=bits;int32_t pcm[CTL*2];mix_block(pcm,CTL);
        for(uint32_t i=0;i<CTL && at+i<frames;i++){
            for(uint32_t c=0;c<2;c++){int32_t v=abs(pcm[2*i+c]);if(v>peak)peak=v;clips+=v>=32767;}
            wav_put(out,pcm[2*i],pcm[2*i+1]);
        }
    }
    assert(!memcmp(&patch,&p5_patch[0],sizeof patch));assert(!fclose(out));
    out=fopen(argv[4],"w");assert(out);fprintf(out,"{\"raw_peak\":%d,\"clipped_samples\":%u,\"frames\":%u,\"native_round_trip\":true,\"patch_unchanged_by_render\":true}\n",peak,clips,frames);fclose(out);
    return clips||peak<8;
}
