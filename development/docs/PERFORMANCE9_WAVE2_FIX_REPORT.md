# Performance 9 Wave2: wavetable note edges and preset envelopes

By: Inputsignwave

22 October 2026. Wave1 hardware feedback: some presets sound alike and notes pop
at onset/release. This patch corrects WT amplitude handling and sounding retriggers;
it does not add a reverb, change the four-frame bank or replace stored preset IDs.

## Reproduced cause and fix

`eng_wavetable.c:wt_render` added `low2 * 2` directly to the output. Unlike the
retained renderers, it never called `voice_amp`. The voice system advanced ADSR and
retired voices, but the waveform did not follow the supplied gain ramp. Consequently:

- Attack/decay/release differences between factory sounds were largely inaudible.
- Velocity and amplitude modulation/performance gain were omitted at this stage.
- A waveform could start or disappear at nonzero amplitude, causing a click.

A new zero-gain test **fails on Wave1**: the oscillator produces samples despite
both gain endpoints being zero. Wave2 applies the existing shared, per-sample
`voice_amp` ramp before adding the oscillator output. The final release/emergency
fade block now reaches near silence before voice retirement. No shared envelope,
retained engine or mixer algorithm was changed.

`wt_note_on` also cleared the tone filters and parameter ramps on every retrigger,
including already sounding voices. Wave2 keeps these states while a voice has a
nonzero envelope/output gain. A genuinely idle voice resets oscillator/scan/sub
phase and filter state, then starts through the amplitude ramp.

## Preset scope

Factory names, values and order are preserved. INIT WAVE remains sustained;
HOLLOW KEYS and GLASS DRIFT retain their different spectra/envelopes; FOLDED BASS
retains its lower tuning/sub; SLOW SCAN has a gradual attack; BRIGHT PLUCK now
actually decays while the key is held. This restores intended distinctions rather
than adding more waveform data. The bank still contains four related source
frames, and its harmonic limiting intentionally reduces brightness in high
registers. Hardware listening must judge whether more timbral variety is wanted.

## New audible-output checks

- Zero amplitude endpoints produce exact digital silence at oscillator peak.
- An attack from zero starts with a zero sample even at oscillator peak.
- A one-block release from peak ends below 1,000 output units before stopping.
- Actual mixer renders: slower attack reduces onset energy; low velocity reduces
  level; short/long releases have different tail energy; BRIGHT PLUCK decays.
- Same-note sounding retrigger output matches a copy left running, with identical
  modulation/gain, instead of clearing filter state under a live envelope.
- Direct pitch/control fixtures now explicitly request full gain. Previously
  their zero-initialized gain fields went unnoticed because WT ignored gain.
- Four-track allocation checks use a conservative 60 KiB main-bank bound, plus
  the smaller host data bank, without optional cache.

Measured single-note window sums (absolute mixer output, fixed deterministic fixture):

| Contract | Wave2 measurement |
| --- | ---: |
| Fast / slow attack, first 40 ms | 12,484,984 / 2,342,104 |
| Velocity 127 / 25, 100–200 ms | 31,552,873 / 6,370,145 |
| Long / shortest release, 300–400 ms after a 100 ms gate | 14,322,498 / 5,493 |
| BRIGHT PLUCK, first / late held 100 ms windows | 9,694,714 / 0 |

Six six-second production host chord auditions have zero clipped samples. These
are software measurements, not a claim of physical click-free acceptance. Initial
tests check bounds, persistence, pitch, repeatability and spectral bandwidth but
missed the amplitude contract; those prior passes did not establish correct note
edges. Wave2 adds the missing behavioural gates without changing historical goldens.

## Build and validation status

Compiled source revision: `006b29ed860081f8d86818929f61689a381b1677`.
Linux native build and registered checks:
[run 39802915690](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39802915690).
The full local suite passed in 286 s after restoring the pinned SDK paths and
testing the matching Wave2 application/package. Linux job 119908343338 completed
successfully: native compile, foundation sanitizers and registered host/web suite.
The first local run used stale packaging inputs/missing SDK files and failed four
package/recovery checks; the complete corrected rerun passed. This records software
validation, not physical acceptance or delivery of a new standalone release bundle.

### Exact linked budget

| Quantity | Wave1 | Wave2 |
| --- | ---: | ---: |
| App including unchanged boot stub | 548,328 B | 548,372 B |
| Application body | 544,520 B | 544,564 B |
| Free application flash | 28,920 B | **28,876 B** |
| Main contiguous audio arena | 62,912 B | **62,848 B** |
| WT shared allocation while active | 12,384 B | 12,384 B |
| WT static render cost | 269 | **283**, within existing +10% gate |

The correction costs 44 B flash and 64 B resident-arena capacity; it allocates no
new voice or waveform buffers. The optional cache remains unnecessary for tested
engine assignments. Static cost is not CPU cycles or percentage. Retained engine
budgets remain unchanged; retired SID/CZ/ROOM/REVERB DSP remains absent.

The `.fwsc` was decoded and its application bytes compared exactly with the native
artifact. Package SHA-256:
`79b3b42be716b6548e1211d837cfe798c0bc9804988f78d4ce335178ae754326`.

### Emulator scope

Pinned fm1-emu v0.2.0, scripted keys over 1,800 emulated milliseconds:

- Exact-package XIP/stub startup: 45 LCD frames, 70,912 stereo audio frames,
  peak 5,471, zero clipped samples.
- Same decoded app with a synthetic saved WAVETABLE/GLASS DRIFT project, plus
  FM6/Prophet/808 tracks: 32 LCD frames, 72,320 stereo frames, peak 291,
  zero clipped samples. The fixture is separate from the installable package.

Both runs encounter the same eight unsupported MMIO addresses and use the firmware's
single-core fallback in this emulator. These are boot/playback checks, not physical
CPU headroom, a full dual-core hardware test or proof of no audible clicks on-device.
