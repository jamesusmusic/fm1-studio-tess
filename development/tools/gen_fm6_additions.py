#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Approved native voice bank integration — By: Inputsignwave.
# Original voice rights/provenance remain in the manifest; this licences the tool.
import argparse, json, hashlib
from pathlib import Path
from gen_fm6_patches import pack

def encode(raw):
    # Exact minimum-size per-voice LZ packets. Backreferences stay inside
    # this voice; every factory program can be decoded independently.
    size=len(raw);cost=[10**9]*(size+1);pick=[None]*size;cost[size]=0
    for i in range(size-1,-1,-1):
        for n in range(1,min(128,size-i)+1):
            c=n+1+cost[i+n]
            if c<cost[i]:cost[i]=c;pick[i]=(n,0)
        for distance in range(1,i+1):
            length=0
            while i+length<size and raw[i+length]==raw[i+length-distance] and length<130:length+=1
            for n in range(3,length+1):
                c=2+cost[i+n]
                if c<cost[i]:cost[i]=c;pick[i]=(n,distance)
    out=[];i=0
    while i<size:
        n,distance=pick[i]
        out.append(128+n-3 if distance else n-1)
        out.extend([distance] if distance else raw[i:i+n]);i+=n
    return out

def main():
    ap=argparse.ArgumentParser();ap.add_argument('auditions',type=Path);ap.add_argument('output',type=Path);a=ap.parse_args()
    rows=json.loads((a.auditions/'APPROVED_SELECTIONS.json').read_text())['accepted_fm6'];assert len(rows)==25
    data=[];offsets=[0];fingerprints=[];manifest=[]
    for row in rows:
        b=(a.auditions/row['path']).read_bytes()
        assert hashlib.sha256(b).hexdigest()==row['sha256'] and len(b)==163 and sum(b[6:162])%128==0
        voice=pack(list(b[6:161]));data+=encode(voice);offsets.append(len(data))
        h=2166136261
        for v in voice:h=((h^v)*16777619)&0xffffffff
        fingerprints.append(h)
        record={k:row[k] for k in ['name','source','sha256','rights']}
        record.update(packed_sha256=hashlib.sha256(bytes(voice)).hexdigest(),packed_fnv32=h);manifest.append(record)
    text=['/* Approved additional voice data; integration — By: Inputsignwave.',
          ' * Original authors/rights: assets/fm6-additions-provenance.json.',
          ' * 808 SUB: Sven Trogus, GPL-3.0-only. Integration credit does not',
          ' * grant redistribution rights to original Yamaha/community data. */',
          '#pragma once','#define FM6_ADDED_COUNT 25u',
          'static const uint16_t FM6_ADDED_OFFSET[26]={'+','.join(map(str,offsets))+'};',
          'static const uint8_t FM6_ADDED_DATA['+str(len(data))+']={']
    for at in range(0,len(data),24):text.append(','.join(map(str,data[at:at+24]))+',')
    text+=['};','#define FM6_ADDED_PRESETS \\']
    for i,r in enumerate(rows):text.append('    FM6_PR('+json.dumps(r['name'].upper())+', 0)'+(',' if i<24 else '')+(' \\' if i<24 else ''))
    a.output.write_text('\n'.join(text)+'\n')
    target=a.output.parents[2]/'assets/fm6-additions-provenance.json'
    target.write_text(json.dumps(dict(contributions='By: Inputsignwave',count=25,uncompressed_bytes=3200,compressed_bytes=len(data)+52,voices=manifest),indent=2)+'\n')
    print(f'FM6 additions: 3200 -> {len(data)+52} B including offsets; exact decode tested separately')
if __name__=='__main__':main()
