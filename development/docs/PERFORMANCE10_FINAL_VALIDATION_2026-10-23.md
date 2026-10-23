# Performance 10 RC1 — combined build and offline validation

By: Inputsignwave

## Exact candidate

- Native source commit: `7a74b612485b335a94203bff6a28207178be4feb`.
- Source tree: `a41fda4125205a799d5b3dc08b22c163eb416bc9`.
- Build: Linux/JieLi pi32v2, pinned AC79 SDK, `MELODEE_RAM_AUDIO=1`.
- Actions run: https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39879540131
- Native target artifact ID: `12469848129`; ZIP SHA-256:
  `64c8ab72fbb0ba4490dd5f9fd9d8f683afe4865f5732f73c92777d47c67b8a20`.
- Flashable file: `inputsignwave-1.4.1-performance10-rc1.fwsc`.
- Firmware SHA-256:
  `f7a1b11232d97f81562d2961062dd309ed8f52e70439fdd690fa9053166c6226`.
- Package identity: `FM-1_91041`; offline package checksums, chip key,
  application boundary and recovery-loader checksum verified.

This is the complete combined feature candidate, not an audition-only reverb or
WT-only build. It includes NIGHT, both retained and new wavetable engines,
25 additional FM6 voices, three FM kits, 12 additional Prophet patches, shared
stereo plate, drive, independent FX LFOs and versioned persistence/MIDI controls.
See [controls](PERFORMANCE10_CONTROLS.md) and the asset provenance files.
No FM-1 was flashed during validation.

## Exact target memory

| Measurement | Bytes |
|---|---:|
| Application including unchanged boot stub | 576,160 |
| Boot stub | 3,808 |
| Application body | 572,352 |
| Application body limit | 573,440 |
| Application flash spare | **1,088** |
| Main audio working arena | **51,520** |
| Data-tail audio bank | 2,656 |
| Low audio bank | 4,260 |
| Retained-tail audio bank | 784 |
| Sum of disjoint audio banks, excluding cache | 59,220 |
| Ordinary RAM consumed through BSS end | 95,648 |

The audio-bank sum is not one contiguous buffer and is not unused memory.
Engine states, table banks, delay history and optional undo use it. Main arena
minimum remains 49,152 bytes. New WAVE LAYERS uses 356 bytes per selected track
plus one shared 5,616-byte bank; FM KIT uses 4,544 bytes per track; the plate
uses a 12,136-byte delay allocation plus its fixed state. Original WT retains
its independent 12,384-byte bank. Host/target ABI differences are not silently
converted into claims about exact target stack usage.

A sanitizer allocation fixture restricted the host's disjoint banks to
49,152 / 2,048 / 4,260 / 784 bytes, without cache. All 2,401 ordered production
engine combinations acquired their engine state and one shared plate, with four
notes requested per track. Peak live allocation with optional recording-undo
requests was 50,868 bytes. The optional snapshot was unavailable in 337 cases
under this deliberately smaller constraint; recording/audio remain prioritized
and existing UI reports unavailable record undo. This does not promise every
combination of additional long-delay effects and undo will fit. Allocation failure
is bounded, and optional effects must not overwrite sounding engine state.

## Signal, integration and UI checks

The complete registered local suite passed against the final native artifacts
in **238 seconds**: 349 historical golden renders unchanged, zero health, voice,
routing or crash failures, and no CPU entries over budget.
Linux Actions run 39879540131 also passed: target compile/link, registered
sanitisers and the entire host/web suite (**431 seconds** for the full suite).
The published target artifact and all measurements above belong to that run.

The complete suite includes the historical reference profile: ROOM/REVERB and
retired engines still appear in reference-only test output, while production
retirement tests verify their silence/bypass and absence from linked DSP.
Original golden PCM expectations were not rewritten to conceal sound changes.

Production tests include:

- 248 retained native preset/FX PCM comparisons against the reference profile.
- 2,401 ordered engine assignments with real concurrent worker execution,
  release/retrigger, controls, spread and shared Chorus; serial and concurrent
  PCM are identical, without resource failures or worker timeouts.
- Original WT envelope/retrigger, 32,768 corner cases and spectral checks.
- New layers: 8,192 pitch/control cases, output guards, deterministic distinct
  families, independent ADSRs/filters, release, project and user-preset recall.
- Exact fingerprints of all 25 approved unpacked FM6 voices; original INIT;
  immutable per-hit FM patches, overlap/release and 60 capped-engine changes
  with 16 held-key requests at each switch, under ASan/UBSan.
- SFX1 atomic decode, project/template recall, old TPLL neutral migration,
  duplicate/insert plate refusal, buffer release, bounded drive and LFO neutrality.
- MIDI FX state read/write and malformed/truncated atomic rejection; WT layer
  controls; FM-kit USB MIDI recording, note-off/STOP and no stuck note owners.
- Plate size/wobble, bounded 90-second stress, Swell, and 30-second closed-input
  Hold with exact stored-tail energy, followed by release decay.
- UI: 261 screens × eight palettes plus page/value sweeps, **zero lint findings**;
  47 permitted ellipsised free texts, zero off-gray pixels in MONO. NIGHT
  first-upgrade migration preserves subsequent user palette choices.
- Web/editor, FBK8/TPLM and old-layout migrations, storage failure/rollback,
  MIDI/REC, routing/transport, native patch/preset and historical CPU guards.

## Exact-package emulator

fm1-emu v0.2.0 macOS ARM64 ran the unmodified `.fwsc` directly for 1,000 emulated
ms: 42 LCD frames, 35,584 stereo frames (silent idle, as expected).

For the combined case, the application extracted from the final package was
asserted byte-identical to the linked native binary. Only synthetic saved NOR
data was overlaid: WAVE LAYERS, FM ELECTRONIC, AFX Tiny Pulse Prophet, Glide FM6,
shared PLATE, kit warm drive and a clocked plate-tone LFO. Playback was activated
at 400 ms. No user flash data was used or modified.

At 1,800 emulated ms: 8,466 interrupts, 499 audio halves, **63,872 stereo frames**
at 44,100 Hz; peak 393, 51,502 nonzero channel samples, **zero clipped samples**.
64 LCD frames, display on; the 1,600 ms snapshot visibly shows all four correct
tracks in NIGHT. The fixture is a quiet short functional smoke, not a loudness
master or comprehensive listening evaluation. Eight unsupported MMIO addresses
remain the known emulator bring-up limitation. No invalid-instruction failure
occurred. The app exercised the serial fallback; hardware dual-core behavior is
covered separately by the concurrent host matrix, not by this emulator.

## CPU evidence and remaining hardware acceptance

The final compiled disassembly passes the existing static target guards.
New functions' initial static weights are recorded separately: WAVE LAYERS
render 1,374; FM kit render 362; plate tick 312; plate wrapper 26; warm drive 427.
These are control-flow/instruction estimates, not cycles or percentages, and do
not include arbitrary callees automatically. No existing budget was raised.

The emulator reported 44.4 M ISR instructions per emulated second and 17.4% of
its executed instruction stream in CPU0 ISR. These are emulator diagnostics;
**they are not physical FM-1 CPU utilization or deadline margin**. Physical stack
high-water, cache/bus contention, worst-case timing and analog audition require
hardware. Audition exposed settings, note edges, long holds, sync changes and
mixed-engine polyphony on the FM-1 before treating RC1 as a stable release.

Back up a full bank before installation. Older firmware cannot read new FBK8
banks or TPLM templates. The new reader retains old formats, IDs and original
presets; standalone legacy FUN22 retains its historical scope and does not carry
SFX1. Pro-800 conversions remain separate. The accompanying source and provenance
retain original author notices; custom contributions are By: Inputsignwave.
