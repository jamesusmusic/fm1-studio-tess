# Wave1 space, wavetable alternatives and acquired reverb sources

By: Inputsignwave

22 October 2026. Research only: no firmware DSP, presets, package or installed
device changed. The user is auditioning Performance 9 Wave1 on hardware.

## What is in Wave1

The new sound engine is WAVETABLE, fresh stored ID 21. There is **no replacement
reverb** in this package. SID, CZ, SID drum kits, ROOM and REVERB have been retired;
Prophet, FM6, 808/909 and the other retained effects remain. Legacy reverb slots
retain their data and bypass dry; they do not run an old or new reverb tank.

## Measured current budget

Source: `build/wavetable-budget-verified.json` and the linked target map, not the
compressed firmware package size. The app body excludes the unchanged 3,808-byte stub.

| Resource | Wave1 |
| --- | ---: |
| Application body / body limit | 544,520 / 573,440 B |
| Remaining application flash | **28,920 B (28.24 KiB; 5.04%)** |
| Flash added versus the removal baseline | 14,712 B |
| Main contiguous audio working arena | **62,912 B (61.44 KiB)** |
| Other disjoint audio banks, cache excluded | 3,120 + 4,260 + 784 B |
| Shared WT bank while active | **12,384 B (12.09 KiB)** |

The working arena is a pool for engine states, effects and history/capture; it is
not all unused memory. The WT bank is allocated inside these pools, shared by WT
tracks, and released when idle. Main capacity after that bank alone is 50,528 B,
before other allocations. Host tests peaked at 35,220 B of dynamic state with WT
plus three FM6 tracks. Every tested four-track engine/kit assignment allocated,
but this does not prove every FX/history combination fits. Small banks cannot be
combined into a single large tank. Optional cache is excluded from these claims.

Flash is tight. Delay buffers primarily consume RAM, whereas code and waveform
assets consume flash; their budgets are separate. Hardware CPU headroom remains
unmeasured: target static cost checks and emulator throughput are not CPU percentages.

## Other wavetable engines and banks

Source revisions are pinned in `FM1_ENGINE_RESEARCH_2026-10-22.md` and the acquired
manifest. All four previously reviewed projects were rechecked: their heads are
unchanged on this review. These are source-array costs, **not compiled port costs**.

| Candidate | Sound/features | Exact source data cost and implication |
| --- | --- | --- |
| Our Wave1 | Four original frames, fold-spectrum morph, scan; three fold variants and four harmonic limits | 4 × 3 × 4 × 129 × 2 = **12,384 B flash**, plus one equally sized shared RAM bank. Already included in the current build. |
| [Prysm](https://github.com/NicholasPratt/Prysm/tree/f4f0bb458b45afaff4f7c843623f226d4dc62b92) | BASIC, HARM, PULSE, VOCAL, DIGI, CMPLX; eight waves per bank; six operators per voice arranged as two carrier/modulator chains | 6 × 8 × 256 × 2 = **24,576 B waveform flash**. Source operator caches alone: **30,840 B RAM** (6,168 morphed/double-buffered + 24,672 bank copies), before voices/FX. Replacing just our assets would add 12,192 B flash and leave 16,728 B before port code changes. A selected bank or simplified operator design is more practical than the whole instrument. GPL-3.0-only. |
| [LOOM-8 Jacquard](https://github.com/charlesvestal/fm1-loom/tree/e88f2e00ec60d299b623c28b038db54409fdeff1) | Shapes, nasal, grit, vocal, spiral, fold, octaves, glass, chirp; scan, phase bend and ripple | Generator declares 9 × 16 × 256 int8 samples = **36,864 B flash**; **4,096 B per bank**. It reads the const bank directly rather than copying our mip/fold format. Replacing our entire bank with all nine would leave only **4,440 B flash before code changes**. One or two selected banks are realistic research options. Its generator/render path does not provide our four mip levels; these are not quality-equivalent representations. GPL-3.0-only. |
| [Plaits via Gator](https://github.com/npi3pak/fm1-gator/tree/92f923e0b9fe6101d5cd422cffa5946ce20d785a) | 192 integrated waveforms, three-dimensional wave terrain, differentiated output | Actual initializer contains 25,344 int16 samples = **50,688 B flash**, before other lookup tables/code. Even replacing our bank exceeds today's flash allowance by **9,384 B**, before code changes. A subset needs a redesigned index/map and allocation review; this source is not a drop-in slot. MIT DSP within a GPL application. |
| [ODeath WAVE-1](https://github.com/gybe808-dotcom/FeluccaWavetableOdeathMod/tree/2a661dc1e9a2c137323a7f6c35cadb6091fda996) | Advertises 20 waves, four voices and eight band limits | Modified wavetable DSP/bank source is still absent from the inspected tree. **Incremental flash, RAM and CPU unknown**. Its 610,086-byte packaged firmware is not an engine-size measurement. Listening reference only. |

For our existing format, each additional frame costs **3,096 B in flash and shared
RAM**. Eight total frames would add 12,384 B and leave 16,536 B flash before other
changes; twelve would leave only 4,152 B. Twenty in this format would need 61,920 B
for the bank alone. Preserve space for reverb first; select a small set of distinct
new timbres after the user evaluates Wave1. Replacing rather than stacking engines
changes the arithmetic; only a real target link can establish a complete port's cost.

## Reverbs acquired for the next stage

Local folder beside the repository: `FM1_Reverb_Wavetable_Research_2026-10-22/`.
It contains **43 original source/licence/reference files from seven pinned
repositories**, with exact revision, upstream blob ID, byte size, URL and SHA-256
in `SOURCE_MANIFEST.json`. Files are unchanged upstream snapshots, not attributed
to Inputsignwave. These are selected DSP/reference files, not complete standalone
firmware projects. Rings' pinned stmlib support headers/licence are included.

| Acquired reference | Delay sample memory as supplied | Use for our next stage |
| --- | ---: | --- |
| [Prysm fixed-point modulated Dattorro plate](https://github.com/NicholasPratt/Prysm/blob/f4f0bb458b45afaff4f7c843623f226d4dc62b92/firmware/src/fx.c), GPL-3.0-only | Tank **66,830 B**; pre-delay 8,192 B; shimmer line 8,192 B: **83,214 B sample storage**, excluding small state and unrelated delay/chorus | Strong fixed-point/modulation reference with optional pitch-shift shimmer. Too big unchanged. Reduce rate, bound pre-delay and initially omit shimmer. Do not import its whole FX chain. |
| [FoMni floating-point Dattorro plate](https://github.com/charlesvestal/fm1-fomni/blob/7e7547f384fe70b7e89cfd725b0a211bcc60db05/firmware/src/dsp/omni.c), GPL-3.0-only | 33,825 floats = **135,300 B**, excluding descriptors/state | Readable FM-1 plate reference and useful host comparison; requires substantial buffer/rate adaptation. |
| [LOOM-8 effects](https://github.com/charlesvestal/fm1-loom/blob/e88f2e00ec60d299b623c28b038db54409fdeff1/app/dsp/fx.c), GPL-3.0-only except MIT PSX module | Actual shared reservation **65,536 B**. Comb/allpass plate uses **22,864 B of delay regions**; lush half-rate Dattorro regions span **53,344 B**; full-scale PSX modes require the full ring | Gives contrasting comb/allpass, lush plate and characterful PSX approaches. Its shared power-of-two ring cannot simply be shrunk to the used region span. A standalone compact comb adaptation is plausible; lush and PSX need constraints/redesign. |
| [Mutable Instruments Rings modulated Griesinger/Dattorro topology](https://github.com/pichenettes/eurorack/blob/f4cfa7697095a28d292bff758beb51deb49b5a86/rings/dsp/fx/reverb.h), MIT, Emilie Gillet | 32,768 × uint16 = **65,536 B**, before filter/LFO state | Good stereo diffusion/modulation reference. Reduce rate and delay lengths together; adjust LFOs and damping for our 44.1 kHz output. Too big unchanged. |
| [el-visio Dattorro C implementation](https://github.com/el-visio/dattorro-verb/tree/41e976a0228a2472156f5c010b3bfbb444e777d2), MIT | Power-of-two allocation totals 42,368 doubles = **338,944 B**, before descriptors/state | Simple independent host reference. Not an embedded-ready port: change double buffers, delay allocation/rate, failure handling and API before considering target use. |

Byte figures above cover delay samples, not full state, flash, stack or CPU. They
come from declared lengths/initializers, not a compile of these donors for our
target. Default buffers may reserve memory even when their mix is zero. A reverb
working in a dedicated FM-1 instrument does not prove it fits our combined engine,
sequencer and performance-FX workload. None of these sources establishes Slöer's
internal DSP; that pedal remains an aesthetic/function reference.

## Recommended first audition and implementation boundary

1. Build a host audition of a **fixed-point modulated plate**, taking Prysm's
   bounded integer processing as the first reference and Rings/FoMni as comparisons.
   Also audition the compact comb/allpass approach for a contrasting texture.
2. Prototype **one shared send**, targeting roughly **16–24 KiB delay storage**
   initially, with explicit allocation failure/dry bypass. This is a design budget,
   not a verified allocation or sound-quality result. Compare rate/length choices
   before selecting bandwidth; about 11.025 kHz with int16 storage offers a much
   smaller plate than the full-rate references, at a bandwidth tradeoff.
3. Prioritize smooth long tails, damping/tone, predelay, slow modulation and stereo
   width. Add our tape/wobble character and controlled freeze/swell after the basic
   tank is stable. Shimmer/pitch shifting costs another buffer and comes later.
4. Compare equal-send and loudness-matched renders of FM6 chords, Prophet pads,
   Wave1 scan sounds and drums, including silence/tail decay and mono compatibility.
   Only then port the selected candidate at a **fresh FX ID**, preserving retired
   ROOM/REVERB data and bypass semantics.
5. Link and profile the complete candidate: remaining flash, disjoint-bank fit,
   active engine + FX/history allocations, static target cost, stack/failure paths,
   sanitizer tests, dual-core consistency and exact-package emulator playback.
   Hardware listening and deadline/CPU measurements remain separate checks.

Expected integration areas: a new `eng`-independent reverb module; `fx.c` routing,
allocation, reset/tail and bypass; FX identity/control descriptors in `core.h`,
`params.c` and resources/UI/editor; project/preset compatibility and MIDI/automation;
host fixtures, target budget gates and emulator scenarios. No boot/update/storage
layout changes are needed for this research. Preserve all upstream authors and
licences; mark our eventual integration **By: Inputsignwave**.
