#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Coherent oscillator alias/image measurement — By: Inputsignwave.
import cmath
import math
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from gen_wavetable import FRAMES,DRIVES

def fft(values):
    n=len(values);a=[complex(x) for x in values];j=0
    for i in range(1,n):
        bit=n>>1
        while j&bit:j^=bit;bit>>=1
        j^=bit
        if i<j:a[i],a[j]=a[j],a[i]
    length=2
    while length<=n:
        root=cmath.exp(-2j*math.pi/length)
        for start in range(0,n,length):
            w=1
            for k in range(length//2):
                u=a[start+k];v=a[start+k+length//2]*w
                a[start+k]=u+v;a[start+k+length//2]=u-v;w*=root
        length*=2
    return a[:n//2+1]

def unwanted(values,bin):
    z=fft(values);allowed={h*bin for h in range(1,16) if h*bin<len(z)}
    useful=sum(abs(z[i])**2 for i in allowed)
    noise=sum(abs(x)**2 for i,x in enumerate(z) if i and i not in allowed)
    return 10*math.log10(max(noise,1e-30)/useful)

def main():
    folder=Path(sys.argv[1] if len(sys.argv)>1 else 'build')
    spectrum=FRAMES[3];n=16384
    # Same original waveform, folded live without bandwidth limiting: comparison only.
    raw=[sum(a*math.sin(2*math.pi*h*i/2048) for h,a in spectrum.items()) for i in range(2048)]
    peak=max(map(abs,raw))
    for bin in (409,1857):
        candidate=[int(x) for x in (folder/f'spectral-{bin}.txt').read_text().splitlines()]
        naive=[]
        for i in range(n):
            phase=(i*bin)%n
            x=sum(a*math.sin(2*math.pi*h*phase/n) for h,a in spectrum.items())/peak
            naive.append(1-abs(((x*DRIVES[-1]+1)%4)-2))
        a,b=unwanted(candidate,bin),unwanted(naive,bin)
        print(f'wavetable spectral: {bin*44100/n:.2f} Hz, unwanted energy {a:.2f} dB, live-fold comparison {b:.2f} dB, improvement {b-a:.2f} dB')
        assert a < -40 and b-a > 10,(a,b)
    print('Static coherent test; interpolation images included. Moving controls and transient pitch jumps require separate listening.')
if __name__=='__main__':main()
