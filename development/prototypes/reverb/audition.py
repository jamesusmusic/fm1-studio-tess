#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# Reproducible audition packaging — By: Inputsignwave.
import argparse, hashlib, html, json, math, shutil, subprocess, wave
from pathlib import Path
import numpy as np

HERE=Path(__file__).resolve().parent

def load(path):
    with wave.open(str(path),'rb') as w:
        assert w.getsampwidth()==2 and w.getframerate()==44100
        return np.frombuffer(w.readframes(w.getnframes()),dtype='<i2').reshape(-1,w.getnchannels()).astype(np.float64)

def save(path,a):
    a=np.rint(a)
    assert np.max(np.abs(a))<32767, (path,np.max(np.abs(a)))
    if a.ndim==1:a=a[:,None]
    with wave.open(str(path),'wb') as w:
        w.setparams((a.shape[1],2,44100,0,'NONE','not compressed'))
        w.writeframes(a.astype('<i2').tobytes())

def fir(factor):
    # Linear-phase anti-alias/reconstruction FIR; offline host adapter only.
    n=np.arange(-96,97,dtype=np.float64)
    h=np.sinc(n*.90/factor)*(.90/factor)*np.kaiser(len(n),9)
    return h/h.sum()

def up(a,factor):
    out=np.zeros((len(a)*factor,2))
    for c in range(2):
        z=np.zeros(len(a)*factor);z[::factor]=a[:,c]
        out[:,c]=np.convolve(z,fir(factor)*factor,mode='same')
    return out

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--sources',type=Path,required=True);ap.add_argument('--output',type=Path,required=True);args=ap.parse_args()
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    work=HERE.parents[1]/'build/reverb-prototype';work.mkdir(parents=True,exist_ok=True)
    for rate in [22050,11025]:
        subprocess.run(['clang','-std=c99','-O2','-Wall','-Wextra','-Werror',f'-DPLATE_RATE={rate}',str(HERE/'render.c'),str(HERE/'plate.c'),'-lm','-o',str(work/f'render-{rate}')],check=True)
    src=args.sources
    sources={
        'FM6_Kalimba':src/'FM6/previews/Eno_recreated/02_Kalimba_2.wav',
        'FM6_Drums':None,
        'Wavetable_Vowel_Glass':src/'Wavetable/Vowel_Glass/slow_scan_C3.wav',
    }
    drums=[]
    for n in ['06_BASS_DR_1','02_SNARE_DR_1','09_HI-HAT']:
        drums.append(load(src/f'FM6/previews/vrc104a/{n}.wav').mean(axis=1))
    kit=np.zeros((44100*4,2))
    for beat in range(8):
        for lane in ([0,2] if beat%2==0 else [1,2]):
            # Source already contains velocity hits; use first half-second hit.
            hit=drums[lane][:22050];at=int(beat*.45*44100)
            kit[at:at+len(hit),:]+=hit[:,None]*(.75 if lane==2 else 1)
    # One tank, contrasting parameter settings, not three independent algorithms.
    # RT60 is a nominal feedback mapping; filtering changes measured decay.
    presets={
      'Clear_Plate':[4.0,7000,0.0,90,8500,1.0,.35,.20,18],
      'Dark_Ambient':[12.0,3000,-.50,160,6500,1.0,.18,.38,35],
      'Moving_Air':[8.0,6800,.32,120,9000,1.0,.70,.48,24],
    }
    controls={
      'Tone_Warm':dict(tone=-1),'Tone_Bright':dict(tone=1),
      'Filter_Lowcut_900Hz':dict(lowcut=900),
      'Filter_Highcut_1800Hz':dict(highcut=1800),
    }
    indices={'tone':2,'lowcut':3,'highcut':4}
    entries=[]; source_meta=[]
    def render(name,dry,params,rate=22050):
        factor=44100//rate;mono=dry.mean(axis=1)
        small=np.convolve(mono,fir(factor),mode='same')[::factor]
        raw=work/'in.raw';raw.write_bytes(np.rint(small).astype('<i2').tobytes())
        dest=work/'wet.raw'
        r=subprocess.run([str(work/f'render-{rate}'),str(raw),str(dest),*map(str,params)],check=True,capture_output=True,text=True)
        stats=json.loads(r.stdout);assert stats['tank_saturations']==0,(name,stats)
        wet=up(np.frombuffer(dest.read_bytes(),dtype='<i2').reshape(-1,2).astype(np.float64),factor)[:len(dry)]
        assert stats['peak']<32767,(name,stats)
        mix=.80*dry+1.25*wet
        entry={'name':name,'rate':rate,'parameters':dict(zip(['nominal_rt60_s','damping_hz','tone','lowcut_hz','highcut_hz','width','movement_hz','depth_ms','predelay_ms'],params)),**stats}
        entry['mix_peak']=float(np.max(np.abs(mix)));entry['wet_rms']=float(np.sqrt(np.mean(wet**2)))
        entry['lr_correlation']=float(np.corrcoef(wet.T)[0,1])
        entry['last_second_wet_rms']=float(np.sqrt(np.mean(wet[-44100:]**2)))
        entries.append(entry);return wet,mix,entry
    audio={}; matched={}; wetfiles={}; dryfiles={}
    for key,path in sources.items():
        dry=kit.copy() if path is None else load(path)
        if dry.shape[1]==1:dry=np.repeat(dry,2,axis=1)
        gain=12000/max(1,np.max(np.abs(dry)));dry*=gain
        source_meta.append({'name':key,'source':str(path) if path else 'Assembly of the first accepted FM6 kick/snare/hat hits; not an implemented FM kit','source_sha256':hashlib.sha256(path.read_bytes()).hexdigest() if path else None,'input_gain':gain})
        dry=np.pad(dry,((0,44100*16),(0,0)));dryfiles[key]=dry
        for label,params in presets.items():
            wet,mix,entry=render(key+'__'+label,dry,params)
            audio[entry['name']]=mix;wetfiles[entry['name']]=wet
            matched[entry['name']]=mix.copy()
        if key=='Wavetable_Vowel_Glass':
            for label,changes in controls.items():
                params=presets['Clear_Plate'].copy()
                for k,v in changes.items():params[indices[k]]=v
                wet,mix,entry=render(key+'__'+label,dry,params);audio[entry['name']]=mix
            wet,mix,entry=render(key+'__Compact_11k_Plate',dry,presets['Clear_Plate'],11025);audio[entry['name']]=mix
    # Provide paired RMS matches within each input, including identical tail length.
    # Plain RMS matching, explicitly not BS.1770/LUFS. Document every applied gain.
    for key in sources:
        names=[key+'__'+x for x in presets];ref=math.sqrt(np.mean(audio[names[0]]**2))
        for name in names:
            gain=ref/max(.01,math.sqrt(np.mean(audio[name]**2)));matched[name]*=gain
            next(e for e in entries if e['name']==name)['rms_match_gain']=gain
    # Single shared safety trim across all files, never per-effect peak normalisation.
    peak=max(np.max(np.abs(a)) for a in [*audio.values(),*matched.values(),*wetfiles.values(),*dryfiles.values()])
    trim=min(1,28000/max(1,peak))
    for folder,items in [('equal_send',audio),('rms_matched',matched),('wet_only',wetfiles),('dry',dryfiles)]:
        (out/folder).mkdir(exist_ok=True)
        for name,data in items.items():save(out/folder/(name+'.wav'),data*trim)
    report={'by':'Inputsignwave','base':'MIT Rings topology, Emilie Gillet; adapted fixed-point C','upstream_revision':'f4cfa7697095a28d292bff758beb51deb49b5a86','audio_rate':44100,'source_gains':source_meta,'shared_safety_trim':trim,'equal_send':'0.80 dry + 1.25 wet, same for all presets','matching':'Plain full-file RMS per input; not LUFS; wet-only follows equal-send gain, not matched gain','resampling':'Offline 193-tap Kaiser FIR, 0.90 * core Nyquist; target resampler not implemented','renders':entries,'firmware_integrated':False,'hardware_tested':False}
    (out/'AUDITION_REPORT.json').write_text(json.dumps(report,indent=2)+'\n')
    (out/'source').mkdir(exist_ok=True)
    for f in HERE.iterdir():
        if f.is_file():shutil.copy2(f,out/'source'/f.name)
    rows=[]
    for key in sources:
        rows.append(f'<h2>{html.escape(key.replace("_"," "))}</h2><p>Dry reference</p><audio controls preload="none" src="dry/{key}.wav"></audio>')
        for label in presets:
            name=key+'__'+label
            rows.append(f'<h3>{label.replace("_"," ")}</h3><div class="pair"><label>Equal send<audio controls preload="none" src="equal_send/{name}.wav"></audio></label><label>RMS matched<audio controls preload="none" src="rms_matched/{name}.wav"></audio></label><label>Wet only<audio controls preload="none" src="wet_only/{name}.wav"></audio></label></div>')
    rows.append('<h2>Separate tone and filter controls</h2><p>Compare with Vowel Glass / Clear Plate above. Only the named control changes.</p>')
    for label in [*controls,'Compact_11k_Plate']:
        name='Wavetable_Vowel_Glass__'+label
        rows.append(f'<h3>{label.replace("_"," ")}</h3><audio controls preload="none" src="equal_send/{name}.wav"></audio>')
    (out/'START_HERE.html').write_text('''<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Inputsignwave — Plate audition</title><style>body{max-width:1100px;margin:40px auto;padding:20px;background:#10151e;color:#e2e8ed;font:17px system-ui}h1,h2{color:#a8d6d1}h2{margin-top:50px;border-top:1px solid #34404c;padding-top:25px}.pair{display:flex;flex-wrap:wrap;gap:20px}label{display:grid;gap:8px;font-size:14px}audio{max-width:100%;width:310px}p{line-height:1.6}a{color:#a8d6d1}</style><h1>Inputsignwave stereo plate — audition 1</h1><p>By: Inputsignwave · Base topology: Mutable Instruments Rings, Emilie Gillet (MIT).</p><p>Three settings of one compact modulated plate. 22.05 kHz fixed-point core, 44.1 kHz listening files. Independent tone, wet low/high cuts and decay damping. Equal-send files preserve level differences; RMS matches help compare character. No limiter or per-file peak normalisation. Listen through the silence to hear the tail.</p><p>This is a host prototype, not a flashable firmware update. It is not a Slöer emulation. Tape saturation, freeze, shipping tempo sync, UI, saved controls and the target resampler are still pending. The 11.025 kHz comparison saves half the delay memory at reduced bandwidth. Source/reproduction files and exact settings accompany these clips.</p>'''+''.join(rows)+'''<script>document.querySelectorAll('audio').forEach(a=>a.addEventListener('play',()=>document.querySelectorAll('audio').forEach(b=>{if(a!==b)b.pause()})))</script>''')
    print(json.dumps({'output':str(out),'renders':len(entries),'shared_trim':trim,'max_tank_saturations':max(e['tank_saturations'] for e in entries),'delay_bytes':{str(e['rate']):e['delay_bytes'] for e in entries}},indent=2))
if __name__=='__main__':main()
