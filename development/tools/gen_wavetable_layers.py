#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Band-limited bank preparation — By: Inputsignwave.
# Requires NumPy only when regenerating assets. Firmware uses the checked-in bank.
import argparse, json, wave, hashlib
from pathlib import Path
import numpy as np

def main():
    ap=argparse.ArgumentParser();ap.add_argument('auditions',type=Path);ap.add_argument('output',type=Path)
    a=ap.parse_args(); selections=json.loads((a.auditions/'APPROVED_SELECTIONS.json').read_text())['accepted_wavetable_directions']
    lines=['/* SPDX-License-Identifier: GPL-3.0-only',
           ' * Bank preparation and mip reduction — By: Inputsignwave.',
           ' * Banks 0..5: Surge Synth Team, GPL-3.0 (Surge generated wavetables).',
           ' * Banks 6..7: Adventure Kid / Kristoffer Karl Axel Ekstrand, CC0.',
           ' * Sources pinned in assets/wavetable-layers-provenance.json. */',
           '#pragma once', '#define WL_FRAMES 6u', '#define WL_ROW_BYTES 117u',
           'typedef int8_t wl_bank_t[8][WL_FRAMES][WL_ROW_BYTES];', 'static const wl_bank_t WL_FLASH={']
    sources=[]
    for bank in selections:
        path=a.auditions/bank['table']
        with wave.open(str(path),'rb') as f:
            assert f.getsampwidth()==2 and f.getnchannels()==1
            x=np.frombuffer(f.readframes(f.getnframes()),dtype='<i2').astype(float).reshape(16,128)
        lines.append('{ /* '+bank['name']+' */')
        for at in np.linspace(0,15,6):
            lo=int(at); hi=min(15,lo+1); row=x[lo]*(1-(at-lo))+x[hi]*(at-lo)
            spectrum=np.fft.rfft(row-row.mean()); data=[]
            for n,harm in [(64,15),(32,7),(16,3),(8,1)]:
                spec=spectrum.copy(); spec[harm+1:]=0
                y=np.fft.irfft(spec,n=128)[::128//n]
                q=np.clip(np.rint(y/256),-127,127).astype(int).tolist()
                data.extend([q[0],q[2]] if n==8 else q+[q[0]])
            assert len(data)==117
            lines.append('{'+','.join(map(str,data))+'},')
        lines.append('},')
        sources.append(dict(name=bank['name'],source=bank['source'],audition_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),precision='signed PCM8',frames=6,mip_samples=[65,33,17,2]))
    lines.append('};');a.output.write_text('\n'.join(lines)+'\n')
    manifest=a.output.parents[2]/'assets'/'wavetable-layers-provenance.json'
    manifest.write_text(json.dumps(dict(contributions='By: Inputsignwave',surge_commit='a30341be00484d3de6b65869998100ccc6db6945',akwf_commit='8de90bf94376670947369e69de0af6b9fbd19286',banks=sources),indent=2)+'\n')
if __name__=='__main__': main()
