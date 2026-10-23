/* SPDX-License-Identifier: GPL-3.0-only
 * Lossless sound-extension envelope prototype — By: Inputsignwave.
 * Does not change the production FBKT/FBK7 loader or flash allocation.
 * Byte-preserving RLE; new metadata fits only if the complete legacy payload
 * compresses sufficiently. Capacity failure must keep the previous A/B copy.
 * Wire: ISX1, legacy byte length LE32, packed length LE32, extension length LE32,
 * CRC32 of the uncompressed legacy bytes + extension, then packed bytes, extension.
 */
#ifndef INPUTSIGNWAVE_SOUND_CODEC_H
#define INPUTSIGNWAVE_SOUND_CODEC_H
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#define ISX_HEADER 20u
static uint32_t isx_u32(const uint8_t *b)
{return (uint32_t)b[0]|(uint32_t)b[1]<<8|(uint32_t)b[2]<<16|(uint32_t)b[3]<<24;}
static void isx_put(uint8_t *b,uint32_t n)
{for(unsigned i=0;i<4;i++)b[i]=(uint8_t)(n>>(8*i));}
static uint32_t isx_crc(uint32_t c,const uint8_t *p,size_t n)
{while(n--){c^=*p++;for(unsigned i=0;i<8;i++)c=(c>>1)^((0u-(c&1u))&0xedb88320u);}return c;}
/* Destination is scratch. Caller commits only a nonzero result; no in-place API. */
static size_t isx_encode(uint8_t *out,size_t cap,const uint8_t *raw,size_t n,const uint8_t *ext,size_t en)
{
    if(!out||!raw||(!ext&&en)||n>UINT32_MAX||en>UINT32_MAX||cap<ISX_HEADER||en>cap-ISX_HEADER)return 0;
    size_t at=0,pos=ISX_HEADER,limit=cap-en;
    while(at<n){
        size_t run=1;while(at+run<n&&run<130&&raw[at+run]==raw[at])run++;
        if(run>=3){if(limit-pos<2)return 0;out[pos++]=(uint8_t)(128+run-3);out[pos++]=raw[at];at+=run;}
        else{
            size_t start=at;at+=run;
            while(at<n&&at-start<128){run=1;while(at+run<n&&run<3&&raw[at+run]==raw[at])run++;if(run>=3)break;at++;}
            size_t len=at-start;if(len>128)return 0;
            if(limit-pos<len+1)return 0;out[pos++]=(uint8_t)(len-1);memcpy(out+pos,raw+start,len);pos+=len;
        }
    }
    size_t packed=pos-ISX_HEADER;if(packed>UINT32_MAX)return 0;
    if(en)memcpy(out+pos,ext,en);memcpy(out,"ISX1",4);isx_put(out+4,(uint32_t)n);isx_put(out+8,(uint32_t)packed);isx_put(out+12,(uint32_t)en);
    uint32_t crc=isx_crc(isx_crc(~0u,raw,n),ext,en)^~0u;isx_put(out+16,crc);return pos+en;
}
/* Two passes: validate structure/CRC without touching outputs, then decode.
 * Output buffers must be separate from input and each other. Caller owns schema
 * validation and commits runtime state only after that succeeds as well. */
static int isx_decode(uint8_t *raw,size_t cap,uint8_t *ext,size_t ecap,const uint8_t *in,size_t size)
{
    if(!in||size<ISX_HEADER||memcmp(in,"ISX1",4))return 0;
    size_t n=isx_u32(in+4),packed=isx_u32(in+8),en=isx_u32(in+12);
    if(n>cap||en>ecap||packed>size-ISX_HEADER||en!=size-ISX_HEADER-packed||(!raw&&n)||(!ext&&en))return 0;
    for(unsigned pass=0;pass<2;pass++){
        size_t pos=ISX_HEADER,at=0,end=ISX_HEADER+packed;uint32_t crc=~0u;
        while(pos<end){
            uint8_t tag=in[pos++];size_t count=tag<128?(size_t)tag+1:(size_t)(tag-128)+3;
            if(count>n-at)return 0;
            if(tag<128){if(count>end-pos)return 0;if(pass)memcpy(raw+at,in+pos,count);else crc=isx_crc(crc,in+pos,count);pos+=count;}
            else{if(pos==end)return 0;uint8_t b=in[pos++];if(pass)memset(raw+at,b,count);else for(size_t i=0;i<count;i++)crc=isx_crc(crc,&b,1);}
            at+=count;
        }
        if(at!=n)return 0;
        if(!pass){crc=isx_crc(crc,in+end,en)^~0u;if(crc!=isx_u32(in+16))return 0;}
        else if(en)memcpy(ext,in+end,en);
    }
    return 1;
}
#endif
