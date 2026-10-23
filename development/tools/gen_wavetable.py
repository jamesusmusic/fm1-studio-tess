#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Original spectral wave/fold bank — By: Inputsignwave.
"""Generate odd-symmetric, DC-free fold snapshots at four harmonic limits.
No samples, third-party wavetables or DSP code are imported. Fold is a morph
between precomputed triangle-folded spectra, not audio-rate nonlinear processing.
"""
import argparse
import math
from pathlib import Path

SAMPLES=128
LIMITS=(15,7,3,1)
DRIVES=(1.0,1.65,2.3)
FRAMES=( {1:1.0}, {1:1,3:.5,5:.22,9:.12,15:.06},
         {1:1,2:.55,5:.45,9:.25,13:.12}, {h:1/h for h in range(1,16)} )

def generate():
    n=2048
    phase=[2*math.pi*i/n for i in range(n)]
    sine={h:[math.sin(h*x) for x in phase] for h in range(1,16)}
    waves=[]
    for spectrum in FRAMES:
        raw=[sum(a*sine[h][i] for h,a in spectrum.items()) for i in range(n)]
        scale=max(map(abs,raw));raw=[x/scale for x in raw]
        folds=[]
        for drive in DRIVES:
            folded=[1-abs(((x*drive+1)%4)-2) for x in raw]
            coeff=[0]+[2*sum(x*y for x,y in zip(folded,sine[h]))/n for h in range(1,16)]
            bands=[]
            for limit in LIMITS:
                row=[sum(coeff[h]*math.sin(2*math.pi*h*i/SAMPLES) for h in range(1,limit+1)) for i in range(SAMPLES)]
                bands.append(row)
            folds.append(bands)
        waves.append(folds)
    peak=max(abs(x) for frame in waves for fold in frame for row in fold for x in row)
    out=['/* SPDX-License-Identifier: GPL-3.0-only',
         ' * Generated original spectral bank — By: Inputsignwave.',
         ' * tools/gen_wavetable.py; flash source, copied once to shared audio RAM. */',
         '#pragma once','#define WT_FRAMES 4u','#define WT_FOLDS 3u',
         '#define WT_BANDS 4u','#define WT_SAMPLES 128u',
         'static const uint8_t WT_HARMONICS[WT_BANDS]={15,7,3,1};',
         'typedef int16_t wt_bank_t[WT_FRAMES][WT_FOLDS][WT_BANDS][WT_SAMPLES+1u];',
         'static const wt_bank_t WT_FLASH={']
    for frame in waves:
        out.append('    {')
        for fold in frame:
            out.append('        {')
            for row in fold:
                q=[round(x/peak*14000) for x in row];q.append(q[0])
                out.append('            {'+','.join(map(str,q))+'},')
            out.append('        },')
        out.append('    },')
    out+=['};','']
    return '\n'.join(out)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('output',type=Path);p.add_argument('--check',action='store_true');a=p.parse_args();s=generate()
    if a.check:
        if not a.output.exists() or a.output.read_text()!=s:raise SystemExit('wavetable bank is stale')
    else:a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(s)
    print('wavetable: 4 frames x 3 folds x 4 harmonic limits x 129 int16 = 12384 B; '+('checked' if a.check else str(a.output)))

if __name__=='__main__':main()
