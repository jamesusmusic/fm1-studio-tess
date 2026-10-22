# Compact stereo plate: first executable audition

By: Inputsignwave

22 October 2026. The replacement reverb now has a standalone executable DSP
candidate and listening bundle. **Production firmware is unchanged.** This is
ready for user audition and subsequent integration, not a flashable release or
a validated claim that the full combined roadmap fits.

## Choice of base

Use Mutable Instruments **Rings**, Emilie Gillet's MIT-licensed modulated
Griesinger/Dattorro topology, as the first base. Original pinned reference:
`pichenettes/eurorack` at `f4cfa7697095a28d292bff758beb51deb49b5a86`,
`rings/dsp/fx/reverb.h`. Its original 65,536-byte delay engine is replaced here by
separate scaled int16 rings, with a portable fixed-point implementation. All
upstream notices are retained; the port, new controls and harness are credited
**By: Inputsignwave**. Slöer remains a creative reference, not a code source or
assertion of matching its algorithm/sound. See
[implementation, source attribution and integration contract](../prototypes/reverb/README.md).

## Implemented in this candidate

One stereo tank with four input diffusers, two cross-coupled branches, fractional
slow modulation and predelay. Independent controls: bounded decay feedback,
internal decay damping, bipolar wet tone shelf, wet low-cut, wet high-cut, stereo
width, movement rate/depth and predelay. Caller-owned allocation and a wet-only
API preserve a separate dry path. Parameters are bounded and slewed; no audio-loop
allocation or floating-point DSP. Fixed-point multiplies use widened integer
products, whose target cost is still to be measured.

| Core rate | Delay sample storage | Additional measured host state |
| --- | ---: | ---: |
| 22,050 Hz | 24,274 B | 208 B |
| 11,025 Hz | 12,136 B | 208 B |

The target ABI, alignment, allocator, resampler and caller add costs. These are
requirements, not spare-RAM claims. Full-rate output in the listening files uses
an offline 193-tap anti-alias/reconstruction FIR, not a target-ready resampler.
The smaller option may suit combined dual-layer WT/FM kit/history loads better;
choose by listening and complete target profiling rather than assuming the
brighter variant fits. The existing Wave2 flash/arena measurements do not include
this prototype.

## Audition bundle

Sibling folder: `Inputsignwave_Reverb_Audition_2026-10-22/`; open `START_HERE.html`.
Three settings of the same algorithm: **Clear Plate**, **Dark Ambient** and
**Moving Air**, on approved FM6 Kalimba, FM percussion and Vowel Glass concept
inputs. These are offline inputs, not proof of finished FM kits or dual-layer WT.

There are **14 effect renders / 35 listening WAV files**: equal-send mixes,
full-file RMS-matched mixes, wet-only and dry references, four one-control
comparisons (tone warm/bright and wet low/high cuts) and an 11.025 kHz comparison.
44.1 kHz PCM16 throughout. Input gains are identical across settings for each
source; no limiter or per-effect peak normalisation. Exact source hashes,
parameters, gains and render metrics are in `AUDITION_REPORT.json`; file hashes
and independent spectral checks are in `AUDIO_VALIDATION.json`.

This is a private audition of previously acquired sounds, not a licence clearance
for distributing Yamaha/Eno recreation data in firmware. Retain the original
sound-audition provenance and clear redistribution separately before shipping.

## Validation

- Both supported core rates pass C99 warning-as-error builds and **ASan/UBSan**.
- Missing/undersized buffers return wet silence; clean input silence stays silent;
  deterministic impulses repeat; stereo output differs and width zero is mono.
- Impulses decay by the late-tail gate; controls change the signal independently;
  reinitialisation clears previous tank/filter/LFO history.
- A 90-second full-scale random-input/extreme-control sweep stays bounded under
  sanitizers. This deliberately harsh test can saturate the bounded output;
  it is distinct from normal-listening validation.
- **Zero tank saturations in all 14 audition renders; zero clipped samples in
  all 35 delivered WAV files.** The final second is silent after the rendered
  tail interval. This is not a claim of click-free hardware operation.
- Spectral checks distinguish tone from the two filters: bright/warm high-to-low
  band-power ratio changes by 20.10x; a 900 Hz low-cut leaves 7.54% of the neutral
  60–500 Hz band power; a 1,800 Hz high-cut leaves 23.11% of the neutral
  3.5–9 kHz band power, using the same input, tank and sends.
- The complete existing `tests/run_tests.sh` suite passes with the resident
  audio profile and pinned offline SDK. Production DSP is unchanged by this
  standalone prototype; this regression run does not establish target reverb cost.
- The initial impulse test exposed integer filter-history deadbands. Fractional
  histories corrected the implementation; the original decay assertion remains.

## Remaining integration work

Fresh effect/control identities, shared send routing, target resampling, ownership,
allocation failure/dry bypass, UI/NIGHT, project/preset storage, MIDI/REC, actual
transport-synced motion, tail/bypass/preset transitions and combined target
flash/RAM/CPU/emulator validation. Prototype movement is free running; an exposed
phase-increment field is not completed clock integration. Preserve retired IDs,
FBKT storage and history. Warm-drive/tape saturation, freeze/swell and pitch-shift
shimmer are not implemented by this first plate candidate. Hardware CPU deadlines
and listening remain separate acceptance checks.
