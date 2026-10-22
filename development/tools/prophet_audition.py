#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Native Prophet sound design and local audition packaging — By: Inputsignwave.
import argparse, hashlib, html, importlib.util, json, math, shutil, subprocess, wave
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import numpy as np
ROOT=Path(__file__).resolve().parents[1]
RADIO='https://reverb.com/news/recreating-radioheads-keys-and-synths-with-software-instruments'
DAWN='https://reverbmachine.com/blog/thom-yorke-synth-sounds/'
AFX='https://reverbmachine.com/blog/aphex-twin-selected-ambient-works-85-92/'
CLASSIC='https://sequential.com/support/download/prophet-5-10-documentation/'
FIELDS='FREQ_A FREQ_B FINE_B SAW_A PULSE_A SAW_B TRI_B PULSE_B PW_A PW_B SYNC LOW_B KEY_B GLIDE LEVEL_A LEVEL_B NOISE CUTOFF RESONANCE KEY_FILTER FILTER_REV LFO_RATE LFO_INITIAL LFO_SAW LFO_TRI LFO_PULSE WHEEL_MIX WHEEL_FREQ_A WHEEL_FREQ_B WHEEL_PW_A WHEEL_PW_B WHEEL_FILTER POLY_ENV POLY_B POLY_FREQ POLY_PW POLY_FILTER VINTAGE PRESS_FILTER PRESS_LFO ENV_FILTER VEL_FILTER VEL_AMP ATTACK_FILTER ATTACK_AMP DECAY_FILTER DECAY_AMP SUSTAIN_FILTER SUSTAIN_AMP RELEASE_FILTER RELEASE_AMP RELEASE_ON UNISON UNISON_COUNT UNISON_DETUNE'.split()
F={x:i for i,x in enumerate(FIELDS)}
def time_raw(ms):return max(0,min(127,round(math.log(max(1,ms),10000)*127)))
def patch(name,**edits):
    p=bytearray(133)
    defaults=dict(FREQ_A=24,FREQ_B=24,KEY_B=1,SAW_A=1,PW_A=64,PW_B=64,LEVEL_A=100,CUTOFF=65,FILTER_REV=1,DECAY_FILTER=60,DECAY_AMP=60,SUSTAIN_FILTER=0,SUSTAIN_AMP=100,RELEASE_FILTER=55,RELEASE_AMP=55,RELEASE_ON=1,UNISON_COUNT=5)
    defaults.update(edits)
    for k,v in defaults.items():assert 0<=v<=127;p[F[k]]=v
    p[65:85]=name.encode('ascii')[:20].ljust(20,b' ')
    return bytes(p)
def encode(raw,group=0,program=0):
    b=bytearray([240,1,50,2,group,program])
    for i in range(0,len(raw),7):
        c=raw[i:i+7];b.append(sum((x>>7)<<j for j,x in enumerate(c)));b.extend(x&127 for x in c)
    b.append(247);assert len(b)==159;return b

def designs():
    r=[]
    def add(name,category,phrase,description,url,**v):r.append(dict(name=name,category=category,phrase=phrase,description=description,source=url,raw=patch(name,**v),new=True))
    add('RH Right Keys','Radiohead / Thom Yorke','chords','Dark saw keys: Everything in Its Right Place-inspired starting point. No doubled recording or record compression.',RADIO,CUTOFF=59,RESONANCE=20,ENV_FILTER=9,ATTACK_AMP=time_raw(3),DECAY_AMP=time_raw(1200),SUSTAIN_AMP=86,RELEASE_AMP=time_raw(350),DECAY_FILTER=time_raw(220),KEY_FILTER=1)
    add('RH Open Keys','Radiohead / Thom Yorke','chords','Brighter performance variation of Right Keys; use cutoff for the opening-out character.',RADIO,CUTOFF=78,RESONANCE=25,ENV_FILTER=13,ATTACK_AMP=time_raw(3),DECAY_AMP=time_raw(1200),SUSTAIN_AMP=86,RELEASE_AMP=time_raw(400),KEY_FILTER=1)
    add('RH Staircase Haze','Radiohead / Thom Yorke','chords','Slow overlapping pad inspired by Staircase. Prophet-8 layering/independent legato and recording effects are not reproduced.',RADIO,SAW_B=1,LEVEL_A=77,LEVEL_B=68,FINE_B=10,CUTOFF=42,ENV_FILTER=38,ATTACK_FILTER=time_raw(2400),ATTACK_AMP=time_raw(1600),DECAY_FILTER=time_raw(1500),SUSTAIN_FILTER=90,SUSTAIN_AMP=115,RELEASE_FILTER=time_raw(2200),RELEASE_AMP=time_raw(1600),VINTAGE=12)
    add('TY Dawn Pulse','Radiohead / Thom Yorke','chords','Narrow-pulse warm keys inspired by Dawn Chorus; no claim of the original layered/stereo production.',DAWN,SAW_A=0,PULSE_A=1,PW_A=22,CUTOFF=48,RESONANCE=12,ENV_FILTER=22,DECAY_FILTER=time_raw(800),SUSTAIN_FILTER=45,ATTACK_AMP=time_raw(6),DECAY_AMP=time_raw(1400),SUSTAIN_AMP=66,RELEASE_AMP=time_raw(650),KEY_FILTER=1)
    add('AFX Tiny Pulse','Aphex-inspired','arp','Short square pluck, a native adaptation of the published Ageispolis recreation approach. No record delay or Arturia FX.',AFX,SAW_A=0,PULSE_A=1,PW_A=64,CUTOFF=119,ATTACK_AMP=time_raw(2),DECAY_AMP=time_raw(73),SUSTAIN_AMP=0,RELEASE_AMP=time_raw(60),RESONANCE=0)
    add('AFX Pulse Stabs','Aphex-inspired','chords','Low-passed saw/PWM chord stabs inspired by Pulsewidth; envelope times adapted to our native curves.',AFX,PULSE_B=1,PW_B=55,LEVEL_A=82,LEVEL_B=64,FINE_B=7,CUTOFF=29,ENV_FILTER=42,RESONANCE=25,KEY_FILTER=1,DECAY_FILTER=time_raw(350),DECAY_AMP=time_raw(490),SUSTAIN_AMP=0,RELEASE_AMP=time_raw(160),LFO_TRI=1,LFO_RATE=39,LFO_INITIAL=32,WHEEL_PW_B=1)
    add('AFX Soft Triangle','Aphex-inspired','bass','Soft two-voice triangle bass following the published Ageispolis recreation idea; our oscillator B supplies the triangle.',AFX,SAW_A=0,LEVEL_A=0,TRI_B=1,LEVEL_B=100,CUTOFF=114,UNISON=1,UNISON_COUNT=2,UNISON_DETUNE=1,ATTACK_AMP=time_raw(5),SUSTAIN_AMP=127,RELEASE_AMP=time_raw(100))
    add('AFX Ambient Veil','Aphex-inspired','chords','Original Inputsignwave ambient colour: hollow slow pulse pad. A stylistic exploration, not an identified Aphex preset or Xtal sample.',None,SAW_A=0,PULSE_A=1,PW_A=42,TRI_B=1,LEVEL_A=71,LEVEL_B=77,FINE_B=9,CUTOFF=47,ENV_FILTER=19,ATTACK_AMP=time_raw(1800),ATTACK_FILTER=time_raw(2600),SUSTAIN_FILTER=97,SUSTAIN_AMP=110,RELEASE_AMP=time_raw(1900),LFO_TRI=1,LFO_RATE=31,LFO_INITIAL=20,WHEEL_PW_A=1,VINTAGE=16)
    add('ISW Sync Lead','New classic-inspired','arp','Original bright oscillator-sync lead with filter-envelope pitch sweep. Classic Prophet technique, not an artist patch.',None,SAW_B=1,FREQ_A=48,FREQ_B=24,LEVEL_A=95,LEVEL_B=0,SYNC=1,POLY_ENV=25,POLY_FREQ=1,CUTOFF=91,ENV_FILTER=19,RESONANCE=29,DECAY_FILTER=time_raw(380),SUSTAIN_FILTER=15,ATTACK_AMP=time_raw(3),DECAY_AMP=time_raw(600),SUSTAIN_AMP=83,RELEASE_AMP=time_raw(130))
    add('ISW PWM Strings','New classic-inspired','chords','Original slow PWM string ensemble colour. Dry first; no chorus hidden in the patch.',None,SAW_A=0,PULSE_A=1,PULSE_B=1,PW_A=48,PW_B=78,LEVEL_A=77,LEVEL_B=73,FINE_B=13,CUTOFF=85,ENV_FILTER=11,ATTACK_AMP=time_raw(650),SUSTAIN_AMP=116,RELEASE_AMP=time_raw(900),ATTACK_FILTER=time_raw(900),SUSTAIN_FILTER=105,LFO_TRI=1,LFO_RATE=43,LFO_INITIAL=49,WHEEL_PW_A=1,WHEEL_PW_B=1,VINTAGE=12)
    add('ISW Poly Bell','New classic-inspired','arp','Original audio-rate Poly-Mod bell: oscillator B modulates A while remaining out of the mixer.',None,SAW_A=0,PULSE_A=1,TRI_B=1,FREQ_B=62,LEVEL_A=88,LEVEL_B=0,POLY_B=25,POLY_FREQ=1,POLY_ENV=12,CUTOFF=100,ENV_FILTER=8,ATTACK_AMP=time_raw(2),DECAY_AMP=time_raw(700),SUSTAIN_AMP=0,RELEASE_AMP=time_raw(650),DECAY_FILTER=time_raw(450))
    add('ISW Velvet Brass','New classic-inspired','chords','Original rounded two-saw brass with a gently opening filter.',None,SAW_B=1,LEVEL_A=77,LEVEL_B=72,FINE_B=7,CUTOFF=49,ENV_FILTER=41,RESONANCE=18,ATTACK_FILTER=time_raw(110),DECAY_FILTER=time_raw(800),SUSTAIN_FILTER=64,ATTACK_AMP=time_raw(35),DECAY_AMP=time_raw(550),SUSTAIN_AMP=104,RELEASE_AMP=time_raw(240),KEY_FILTER=1)
    spec=importlib.util.spec_from_file_location('p5_factory',ROOT/'tools/gen_prophet_factory.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
    bank=m.records(include_additions=False)
    for i,phrase in [(160,'chords'),(161,'chords'),(163,'chords'),(166,'arp'),(175,'bass'),(184,'chords'),(191,'arp'),(198,'chords')]:
        p=bank[i];name=p[65:85].decode('ascii').strip();number=f'{i//40+1}{i%40//8+1}{i%8+1}'
        r.append(dict(name=f'{number} {name}',category='Existing Sequential classics',phrase=phrase,description=f'Unchanged Sequential factory reference {number}; already present in our 200-program bank, not a new imported preset.',source=CLASSIC,raw=p[:133],new=False,factory_index=i))
    return r

def load(p):
    with wave.open(str(p),'rb') as w:
        assert w.getframerate()==44100 and w.getsampwidth()==2 and w.getnchannels()==2
        return np.frombuffer(w.readframes(w.getnframes()),dtype='<i2').reshape(-1,2).astype(np.float64)
def save(p,a):
    a=np.rint(a);assert np.max(np.abs(a))<32767
    with wave.open(str(p),'wb') as w:
        w.setparams((2,2,44100,0,'NONE','not compressed'));w.writeframes(a.astype('<i2').tobytes())
def main():
    a=argparse.ArgumentParser();a.add_argument('--output',type=Path,required=True);args=a.parse_args();out=args.output.resolve()
    for d in ['patches','dry','with_reverb','source','validation']: (out/d).mkdir(parents=True,exist_ok=True)
    work=ROOT/'build/prophet-audition';work.mkdir(parents=True,exist_ok=True)
    # Follow host-suite flags and suppress its known included-HAL warnings.
    command=['clang','-std=c99','-O2','-ffp-contract=off','-w','-I'+str(ROOT/'build/gen'),str(ROOT/'tools/prophet_audition.c'),'-lm','-o',str(work/'render')]
    subprocess.run(command,check=True)
    items=designs();seen=set()
    for i,e in enumerate(items):
        e['id']=f'{i+1:02d}_'+''.join(c if c.isalnum() else '_' for c in e['name'])
        key=bytes(e['raw'][:65]+e['raw'][85:]);assert key not in seen,e['name'];seen.add(key)
        address=e.get('factory_index',i)
        frame=encode(e['raw'],address//40,address%40);e['native_sha256']=hashlib.sha256(frame).hexdigest()
        (out/'patches'/(e['id']+'.syx')).write_bytes(frame)
    def render(e):
        p=work/(e['id']+'.wav');j=work/(e['id']+'.json')
        subprocess.run([str(work/'render'),str(out/'patches'/(e['id']+'.syx')),e['phrase'],str(p),str(j)],check=True,capture_output=True)
        stats=json.loads(j.read_text());assert not stats['clipped_samples'];dry=load(p)
        # Constant peak-matching gain for comfortable audition; no compressor/limiter.
        gain=12000/max(1,np.max(np.abs(dry)));dry*=gain
        # Container edge fade only, disclosed; engine release remains audible.
        fade=2205;dry[-fade:]*=np.linspace(1,0,fade)[:,None]
        save(out/'dry'/(e['id']+'.wav'),dry)
        e.update(stats);e['preview_gain']=gain;e['dry_peak']=float(np.max(np.abs(dry)));e['dry_rms']=float(np.sqrt(np.mean(dry**2)))
        return e
    with ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(render,items))
    # Use exactly the approved prototype core; effects remain visibly separate.
    spec=importlib.util.spec_from_file_location('plate_audition',ROOT/'prototypes/reverb/audition.py');rev=importlib.util.module_from_spec(spec);spec.loader.exec_module(rev)
    renderer=ROOT/'build/reverb-prototype/render-22050'
    subprocess.run(['clang','-std=c99','-O2',str(ROOT/'prototypes/reverb/render.c'),str(ROOT/'prototypes/reverb/plate.c'),'-lm','-o',str(renderer)],check=True)
    for i in [0,2,3,4,5,7,9,10]:
        e=items[i];dry=np.pad(load(out/'dry'/(e['id']+'.wav')),((0,44100*9),(0,0)))
        mono=dry.mean(axis=1);small=np.convolve(mono,rev.fir(2),mode='same')[::2]
        inp=work/'reverb-in.raw';wet=work/'reverb-wet.raw';inp.write_bytes(np.rint(small).astype('<i2').tobytes())
        settings=[8,6800,.32,120,9000,1,.7,.48,24]
        stats=json.loads(subprocess.run([str(renderer),str(inp),str(wet),*map(str,settings)],check=True,capture_output=True,text=True).stdout)
        assert not stats['tank_saturations']
        w=rev.up(np.frombuffer(wet.read_bytes(),dtype='<i2').reshape(-1,2).astype(float),2)[:len(dry)]
        mix=.8*dry+1.25*w
        # Reverb pair shares one gain if needed; never limit or clip.
        trim=min(1,28000/max(1,np.max(np.abs(mix))));save(out/'with_reverb'/(e['id']+'.wav'),mix*trim)
        e['reverb']={'preset':'Moving Air','settings':settings,'mix':'0.8 dry + 1.25 wet','safety_gain':trim,'tank_saturations':stats['tank_saturations']}
    for e in items:e.pop('raw')
    (out/'PRESET_INDEX.json').write_text(json.dumps(items,indent=2)+'\n')
    summary={'by':'Inputsignwave','engine':'Current native Inputsignwave Prophet engine, no synthesis changes','new_native_patches':12,'existing_factory_references':8,'dry_previews':20,'reverb_previews':8,'raw_clipped_samples':sum(e['clipped_samples'] for e in items),'preview_container_fade_ms':50,'normalisation':'Per-file constant peak matching to 12000 PCM; no limiter; not LUFS','phrases':'Original generic note/chord patterns, not song transcriptions','firmware_changed':False,'hardware_written':False}
    (out/'AUDITION_SUMMARY.json').write_text(json.dumps(summary,indent=2)+'\n')
    parts=[]
    for group in dict.fromkeys(e['category'] for e in items):
        parts.append(f'<h2>{html.escape(group)}</h2>')
        for e in (x for x in items if x['category']==group):
            row=f'<article><h3>{html.escape(e["name"])}</h3><p>{html.escape(e["description"])}</p><div class="pair"><label>Dry<audio controls preload="none" src="dry/{e["id"]}.wav"></audio></label>'
            if e.get('reverb'):row+=f'<label>With approved reverb<audio controls preload="none" src="with_reverb/{e["id"]}.wav"></audio></label>'
            row+='</div>'
            if e['source']:row+=f'<p><a href="{html.escape(e["source"])}">Design / provenance reference</a></p>'
            parts.append(row+'</article>')
    (out/'START_HERE.html').write_text('''<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Inputsignwave — Prophet auditions</title><style>body{max-width:1050px;margin:35px auto;padding:20px;background:#10151e;color:#e2e8ed;font:17px system-ui;line-height:1.55}h1,h2{color:#a8d6d1}h2{border-top:1px solid #354555;padding-top:25px;margin-top:45px}article{padding:5px 0 20px}audio{max-width:100%;width:330px}label{display:grid;gap:7px}.pair{display:flex;gap:25px;flex-wrap:wrap}a{color:#a8d6d1}</style><h1>Inputsignwave Prophet auditions</h1><p>By: Inputsignwave · 12 new native patches + 8 existing Sequential classics.</p><p>Rendered through our actual Prophet engine. Artist-labelled patches are adaptations/inspired recreations, not authenticated artist dumps. The Aphex originals were not necessarily Prophet recordings. No song recordings, original song MIDI, Arturia audio, or paid Pro-800 patch data are used in these clips.</p><p>Dry previews are peak-matched with a constant per-patch gain to 12000 PCM, with no limiter and a final 50 ms container fade. Reverb examples are labelled separately and use the approved Moving Air prototype. These are private auditions; no firmware or device was changed. Your Desktop Pro-800 packs are inventoried separately and have not been converted in this set.</p>'''+''.join(parts)+'''<script>document.querySelectorAll('audio').forEach(a=>a.addEventListener('play',()=>document.querySelectorAll('audio').forEach(b=>{if(b!==a)b.pause()})))</script>''')
    for p in [ROOT/'tools/prophet_audition.py',ROOT/'tools/prophet_audition.c']:shutil.copy2(p,out/'source'/p.name)
    print(json.dumps(summary,indent=2))
if __name__=='__main__':main()
