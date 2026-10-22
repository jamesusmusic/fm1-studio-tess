# Available FM-1 engine sources reviewed for Wave1

By: Inputsignwave

Reviewed 22 October 2026, before finalizing the Wave1 package. This is a source
availability and integration review, not an audition or a measured port of these
engines. No donor DSP, waveform assets, platform code or storage layout was imported.
The pinned revisions below make this review reproducible as projects change.

## Decision for this build

Finish our existing WAVETABLE prototype alongside Prophet, FM6 and 808/909.
The research does not establish a replacement that fits our remaining flash,
shared audio banks, voice budget and existing eight-control engine interface
without a separate port and measurement exercise. Keep SID/CZ and their stored
IDs retired. Keep MACRO disabled. Replacement reverb remains the next prototype;
a focused physical/modal engine follows only after its budget is established.

The current WT's four original frames are deliberately modest. Its fold control
morphs bandwidth-limited folded spectra; it is not live nonlinear input folding
or a full wavetable/FM operator engine. Wider banks and more synthesis modes
would be separate sound-design decisions, not silent changes to saved presets.

## Candidates and what is actually available

| Project / pinned revision | Available engine or relevant work | Fit for Inputsignwave |
| --- | --- | --- |
| [Felucca-Engines](https://github.com/hugelton/Felucca-Engines/tree/04ffe903eb87a752305d1e5b10da01b53e4ab004) | MIT C DSP including ANALOG, LOFI, TRIO, WHEEL, NOISE and PHASE source files. `src/eng_wheel.c` and `src/eng_trio.c` are visible, reusable engine sources. | An organ or oscillator cluster could be a focused future addition. Much overlaps engines previously retired from our code; do not re-enable them wholesale. The whole synth/platform remains a separate project and licence. |
| [Gator](https://github.com/npi3pak/fm1-gator/tree/92f923e0b9fe6101d5cd422cffa5946ce20d785a) | Monophonic Plaits-based firmware with 24 models at 48 kHz. The repository includes Plaits modal, string, wavetable, formant and other engine headers/source under `third_party/eurorack`. Its licensing document distinguishes MIT Plaits/stmlib from its GPL-3.0-only application/platform. | Useful source reference for a carefully selected physical or harmonic model. Its monophonic architecture and rate differ from our polyphonic 44.1 kHz engine slots; target timing, working memory and integration are unmeasured. Not a drop-in multi-engine expansion. |
| [FiMba-1](https://github.com/jadamsowers/fm1-fimba/tree/0485ed0d387ce78216428c7b19f97d77bbe10b4d) | GPL-3.0-only `firmware/src/dsp/kalimba.c` and `.h`: modal tines, body resonances and effects. The header describes five two-pole resonators per tine, materials, pluck behaviour and a body model. Host tests/source are available. | Strong focused reference for an expressive modal/kalimba engine. Extracting a bounded model would be preferable to importing its entire instrument/FX allocation. Resource cost and perceptual quality in our mixer still need measurement. |
| [Prysm](https://github.com/NicholasPratt/Prysm/tree/f4f0bb458b45afaff4f7c843623f226d4dc62b92) | GPL-3.0 firmware with `firmware/src/engine.c` and engine tests. Four voices, two three-operator wavetable chains per voice with FM/ring/mix, plus filters and stereo FX. Documentation includes a modulated Dattorro plate. | Valuable wave/FM and later reverb reference. It is a complete dedicated instrument rather than a small engine slot. Our retained FM6 already covers much FM territory; do not duplicate this entire signal chain in the remaining space. |
| [ODeath WAVE-1](https://github.com/gybe808-dotcom/FeluccaWavetableOdeathMod/tree/2a661dc1e9a2c137323a7f6c35cadb6091fda996) | README advertises 20 waves, four voices, eight band limits and nine presets, and identifies Felucca provenance. **The inspected tree contains the installer, JS and a prebuilt `.fwsc`, not the modified wavetable DSP source.** | A listening/feature reference, not a source donor established by this review. Do not infer that a binary plus a Felucca licence link provides the modified engine source or bank assets. |
| [LOOM-8](https://github.com/charlesvestal/fm1-loom/tree/e88f2e00ec60d299b623c28b038db54409fdeff1) | `app/engine.c` and `.h`, with jacquard wavetable, cane physical models and other engines described in its README. The application is GPL-3.0-only; its licensing document separately identifies MPL-2.0 platform files. | Useful later comparison for compact physical/wavetable models and performance throws. Keep engine work separate from its eight-track UI/sequencer architecture and check individual source provenance before any extraction. |
| [M-VAVE FM1 Open](https://github.com/m-vave/fm1_open/tree/4c7496daac04a95ff10d1afdcc8f9ead2dbabc41) | Public application, synth API headers and **binary** `sdk/lib/libfm1_engine.a`. THIRD_PARTY identifies 303, 808/pluck, FM and PSG ancestry. Project APP licence includes an SDK linking exception. | Useful official API/tooling reference, but the exposed archive is not source for all those engine implementations. Not a compatible replacement for our DSP/memory/boot platform without substantial separate integration. |

The repository tree inspections are important: a firmware advertised as open or
based on an open project is not automatically a usable source module. Source
file byte sizes are not estimates of compiled flash, RAM or CPU.

## Reverb and performance references to carry forward

- [Prysm](https://github.com/NicholasPratt/Prysm) documents modulated plate reverb,
  damping, pre-delay and shimmer. The source is available for investigation;
  none was ported in Wave1.
- [FiMba-1](https://github.com/jadamsowers/fm1-fimba) documents plate space, tape
  colour and granular freeze. Its complete instrument/FX memory is not our reverb budget.
- [AMB-1](https://github.com/charlesvestal/fm1-amb) documents unequal tape loops,
  tape echo and a Dattorro plate. Its current README identifies MPL-2.0, with
  GPL-3.0-only for earlier versions; retain version-specific provenance.
- [Jangada](https://github.com/zednaked/jangada) documents a superwave/ladder
  engine, tape wow/flutter and held-key loops/stutter/throws. These are useful
  listening/workflow references for our existing roadmap. No adoption is implied.

For any later selected port: inspect the exact DSP files and assets, preserve
actual authors and licences, add **By: Inputsignwave** only to our integration,
assign a fresh stored ID, and test signed controls, tuning, note lifetimes,
project/preset/template/backup paths, concurrent rendering and failure handling.
Compile and compare real target maps/disassembly, then audition under mixed-track
and FX load. Emulator throughput alone cannot establish hardware CPU headroom.
