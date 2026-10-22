#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# File and independent spectral-control verification — By: Inputsignwave.
import hashlib, json, sys, wave
from pathlib import Path
import numpy as np
root=Path(sys.argv[1])
def read(path):
    with wave.open(str(path),'rb') as w:
        assert w.getframerate()==44100 and w.getnchannels()==2 and w.getsampwidth()==2
        a=np.frombuffer(w.readframes(w.getnframes()),dtype='<i2').reshape(-1,2).astype(float)
    assert len(a)>44100 and np.max(np.abs(a))>0 and np.max(np.abs(a))<32767
    return a
files=list(root.rglob('*.wav'));assert len(files)==35,len(files)
info=[]
for f in files:
    a=read(f);info.append({'file':str(f.relative_to(root)),'frames':len(a),'peak':int(np.max(np.abs(a))),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
# Subtract the identical dry component to examine only the changed wet spectrum.
dry=read(root/'dry/Wavetable_Vowel_Glass.wav')
def bands(label):
    mix=read(root/f'equal_send/Wavetable_Vowel_Glass__{label}.wav')
    wet=mix-.8*dry
    # Sum channel power, not summed-channel amplitude (avoids mono-cancellation bias).
    y=np.fft.rfft(wet,axis=0);freq=np.fft.rfftfreq(len(wet),1/44100)
    power=np.sum(np.abs(y)**2,axis=1)
    return np.array([power[(freq>=lo)&(freq<hi)].sum() for lo,hi in [(60,500),(500,1800),(3500,9000)]])
b={key:bands(key) for key in ['Clear_Plate','Tone_Warm','Tone_Bright','Filter_Lowcut_900Hz','Filter_Highcut_1800Hz']}
# Ratios distinguish tone tilt from independently located low/high cut responses.
bright_ratio=(b['Tone_Bright'][2]/b['Tone_Bright'][0])/(b['Tone_Warm'][2]/b['Tone_Warm'][0])
lowcut_ratio=b['Filter_Lowcut_900Hz'][0]/b['Clear_Plate'][0]
highcut_ratio=b['Filter_Highcut_1800Hz'][2]/b['Clear_Plate'][2]
assert bright_ratio>2,bright_ratio
assert lowcut_ratio<.30,lowcut_ratio
assert highcut_ratio<.45,highcut_ratio
report={'wav_files':len(files),'clipped_samples':0,'tone_bright_vs_warm_high_to_low_power_ratio':float(bright_ratio),'lowcut_low_band_power_vs_neutral':float(lowcut_ratio),'highcut_high_band_power_vs_neutral':float(highcut_ratio),'files':info}
(root/'AUDIO_VALIDATION.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:v for k,v in report.items() if k!='files'},indent=2))
