#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Read-only private Pro-800 pack inventory — By: Inputsignwave.
# Protocol reference: https://github.com/samstaton/pro800/blob/main/pro800syx.md
# No patch conversion, MIDI traffic or source-file modifications.
import argparse, hashlib, json
from pathlib import Path
HEADER=bytes.fromhex('f00020320001240078')
def inventory(path):
    data=path.read_bytes();at=0;frames=[]
    while at<len(data):
        if data[at]!=240:break
        end=data.find(b'\xf7',at)
        if end<0:break
        frames.append(data[at:end+1]);at=end+1
    layouts={};examples=[];hashes=set();addresses=[];invalid=0
    for f in frames:
        if f[:9]!=HEADER or len(f)<13 or any(x>127 for x in f[1:-1]):invalid+=1;continue
        decoded=bytearray();packed=f[11:-1]
        for i in range(0,len(packed),8):
            chunk=packed[i:i+8]
            for j,x in enumerate(chunk[1:]):decoded.append(x|((chunk[0]>>j&1)<<7))
        if len(decoded)<150:invalid+=1;continue
        version=decoded[4];key=f'0x{version:02X}';layouts[key]=layouts.get(key,0)+1
        # Best-effort metadata only: unknown/older layouts are not silently converted.
        name=bytes(decoded[150:166]).split(b'\0')[0].decode('ascii','replace')
        if len(examples)<8:examples.append(name)
        addresses.append(f[9]+128*f[10]);hashes.add(hashlib.sha256(decoded).hexdigest())
    return {'file':str(path.resolve()),'sha256':hashlib.sha256(data).hexdigest(),'bytes':len(data),'frames':len(frames),'valid_frame_boundaries':at==len(data),'invalid_or_unmodelled_frames':invalid,'layout_versions':layouts,'example_names_best_effort':examples,'unique_payloads_in_file':len(hashes),'address_range':[min(addresses),max(addresses)] if addresses else None,'converted':False}
def main():
    p=argparse.ArgumentParser();p.add_argument('source',type=Path);p.add_argument('output',type=Path);a=p.parse_args()
    rows=[inventory(f) for f in sorted(a.source.rglob('*.syx'))]
    a.output.write_text(json.dumps(rows,indent=2)+'\n')
    print(json.dumps({'files':len(rows),'source_bytes':sum(r['bytes'] for r in rows),'boundary_failures':sum(not r['valid_frame_boundaries'] for r in rows),'converted':False}))
if __name__=='__main__':main()
