# Inputsignwave: next-update reverb auditions

By: Inputsignwave

21 October 2026. Planning and research only; no DSP or firmware changes yet.

## Confirmed retirement decision

The user approved removing both ROOM and REVERB from the next update, replacing
them entirely with our own reverb offering. Remove both old implementations and
selection entries from the shipping build; use the installed Dev2 or archived
host renders for comparisons. Preserve both stored IDs and settings as inert
legacy data. Old ROOM/REVERB slots must transparently bypass without delay-buffer
allocation, retain the dry signal and display an unavailable status. Do not reuse
either ID for a new algorithm or silently migrate saved sounds. Test load/save and automated values
on retired slots as well as new-effect routing.

## Listening shortlist

| Candidate | Intended character to test | Implementation approach |
| --- | --- | --- |
| Compact plate | Dense, smooth tail; useful on FM chords, snares and melodic percussion | Prototype a reduced-rate, fixed-point Dattorro/Griesinger-style tank; compare with the current FDN. |
| Modulated ambient space | Long, dark or airy tails, slow movement and broad stereo image | Prototype a replacement modulated tank; compare diffusion, damping and stereo motion against archived renders of the retired FDN. |
| Tight room/chamber | Short space with clear attacks and less obvious ringing | Prototype a compact room/chamber mode within the new reverb design; compare against archived ROOM renders. |
| Tape-coloured space | Darkening repeats within a tail, gentle wobble and optional saturation | A character layer applied to a chosen tank; assess separately from the underlying algorithm. This is not automatically a fourth independent algorithm. |

These describe desired auditions, not achieved sound quality. Start with plate
and ambient space. Keep archived ROOM and REVERB renders as listening references
and preserve existing projects. Spring, reverse and pitch-shift shimmer can be
revisited after these comparisons; they are outside the first prototype scope.

## Source research

- [Mutable Instruments Rings reverb](https://github.com/pichenettes/eurorack/blob/master/rings/dsp/fx/reverb.h)
  by Emilie Gillet is an MIT-licensed implementation of a Griesinger topology
  described in the Dattorro paper. Its modulated tank is a useful plate-like
  reference. The declared 32,768-entry, 16-bit engine needs **64 KiB** of delay
  storage as supplied, before other state. It is not a drop-in FM-1 candidate;
  shortening or lowering the rate changes bandwidth and character and must be
  auditioned. Review supporting files and preserve their notices if ported.
- [DaisySP-LGPL ReverbSc](https://github.com/electro-smith/DaisySP-LGPL/blob/master/Source/Effects/reverbsc.h)
  is an eight-delay stereo reference. Its 98,936-float buffer alone occupies
  **395,744 bytes** with 32-bit floats. Use as a host comparison initially,
  not a promised target port. This module is **LGPL v2.1**, independently of
  the main DaisySP library's licence; retain its actual authors and terms.
- [Walrus Audio Slöer](https://www.walrusaudio.com/products/sloer-stereo-ambient-reverb)
  remains a creative reference for stereo width, modulation shapes and ambient
  behaviour. This does not identify its internal DSP or provide code to port.
  Our tape character is a separate design idea, not a claim about that pedal.

## Current implementation and resource constraints

`firmware/src/fx.c` implements ROOM as four damped combs at FS/4 with a 2 KiB
allocation. REVERB is an eight-line orthogonal feedback tank with input/output
allpasses at FS/8, with a 4 KiB allocation. At the 44.1 kHz audio rate the tank
runs at 5,512.5 Hz. This describes the installed build; both ROOM and REVERB are
now scheduled for removal. Removing them avoids their buffer allocations when
selected, but releases no delay memory from slots where they were already inactive. Measure code
and state savings separately. A replacement FDN is another implementation of
an algorithm family already used here.

The current Performance 8 Dev2 build has approximately 8.4 KiB of app-slot
headroom and only 544 bytes of ordinary SRAM tail. The audio arena is 49,600
bytes against its 49,152-byte gate. Delay allocations come from managed audio
memory; neither these margins nor optional cache memory constitute a spare
general-purpose reverb buffer. Re-measure the actual target after each change.

Coordinate the budget with wavetable/resonator work and future performance-FX
capture buffers. Removing engine code may free flash without freeing equivalent
runtime RAM. The user has now approved removing SID and CZ, including SID drum kits. Remove
the shared SID core as part of that work, then measure the actual savings before
assigning memory to reverb. The installed Performance 8 Dev2 remains unchanged.

Initially test a single candidate in a shared send slot. Prototype builds may
contain different candidates individually; fitting every candidate simultaneously
is not a requirement. Any eventual shared-only restriction must be explicit in
the UI and compatible with project loading.

## Audition and acceptance sequence

1. Render identical dry inputs through archived ROOM/REVERB implementations and each
   candidate: isolated kick/snare/hats, bright FM notes, sustained chords and
   evolving Prophet sounds. Include silence after each phrase to hear the tail.
2. Provide both equal-send and loudness-matched comparisons. Record settings,
   sample rate, buffer size and source revision; do not disguise level changes
   as improved quality. Listen for ringing, grain, metallic tones, bass buildup,
   transient smearing, stereo motion and mono cancellation.
3. Compare neutral settings first, then modulation, damping, stereo width and
   tape colour. Evaluate swell/freeze as optional later performance behaviours;
   define input, release and gain handling before enabling them.
4. Profile promising bounded implementations on the target with four active
   tracks, high voice counts and other FX. Check CPU deadlines, arena failures,
   app/SRAM limits, parameter sweeps, long decays and silence stability.
5. Verify bypass, wet/dry gain, tail switching, STOP/restart, track deletion,
   project save/load and legacy FX IDs. Keep feedback bounded and transitions
   click-controlled. Run the existing regression/sanitizer/resource gates;
   change sound baselines only for deliberately changed effects.
6. Use the chip emulator for package, UI and routing smoke checks where supported.
   Make listening and timing decisions from host renders and actual FM-1 tests;
   emulator timing alone does not establish hardware audio performance.
7. Give the user audio comparisons before choosing the shipping algorithm and
   controls. Firmware implementation, target suitability and listening approval
   remain open; this research does not claim they have passed.

## Likely code areas

- `firmware/src/fx.c`: tank/diffuser DSP, rate conversion, allocation sizing,
  feedback and parameter smoothing; preserve current routing and gain contracts.
- FX state definitions, central parameter metadata and effect-selection/UI code:
  inspect references before adding types or controls, retain saved IDs and ranges,
  and show controls only where they apply.
- Shared-send processing and audio allocation: account for core ownership and
  simultaneous effects without silently reducing voices or dropping processing.
- Host DSP/routing tests and resource checks: add meaningful stability and
  compatibility cases with reproducible audition renders.
- Documentation and credits: explain the selected controls and source provenance;
  credit our adaptations as By: Inputsignwave alongside retained upstream notices.

This document adds planned work. It does not report a current audio defect or
authorize a new bootloader, flash layout or storage format.
