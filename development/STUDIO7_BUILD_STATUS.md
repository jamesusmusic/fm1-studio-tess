# Studio Tess 7 development build

By: Inputsignwave

This is the active expanded patch, based on the preserved Performance 6 snapshot.
The software validation record and remaining physical acceptance are documented
in `STUDIO7_VALIDATION.md`. No device has been flashed by this work.

## Device feedback and next priority

20 October 2026: the user reports that the installed Performance 7 build is
working perfectly. The next priority for 21 October is page/menu reorganisation
(keeping beat and rhythm-morphing controls in SEQ, with conditional generator
visibility; Sound Designer moves to EDIT and OUTPUT to GLO) and a complete
page/function guide explaining their relationships. See the
top of `ROADMAP.md`. The current firmware layout has not been changed.

## Implemented paths

- Selectively adapted Tess v1.3.3 (`f0971d59ae5e30150d8f9f049b71d84373cfa34c`):
  Grids, BOUNCE, AWAKE and POLY, preserving all existing synth engines and the
  original four generator implementations. Upstream copyrights remain in gen files.
- PATTERN MODE adds those generators and KIT. BOUNCE retains up to three
  simultaneous pitches. HIT/REPLACE duration releases the previous batch once;
  other pitches in the same batch join it. Grids maps three roles to kit lanes.
- Project-owned local meter 1..16 over 4/8/16, up to eight positive accent groups
  adding exactly to the meter, ratios 1..8 against 1..8, phase in 1/4096 bar,
  drift +/-100 basis points, freeze of drift, immediate/next-bar alignment.
  The existing sequencer clock is the only tempo source, including MIDI clock.
- COND matches donor OFF, 1:2, 2:2, 1:4..4:4, 1:8, 8:8, FIRST and !FIRST.
  Reference chooses local bars, master 4/4 bars, FORM section bars or pattern
  cycles. Local EVOLVE counts musical bar boundaries rather than rounded
  integer steps per odd/compound bar.
- SEED 1..128 and bounded ROLL. Seeded old generators use their own random stream;
  unseeded existing generators preserve the original draws and sound baseline.
  New generators use independent streams. AWAKE A/B values remain editable.
- KIT develops a selected source bank into eight coordinated role lanes.
  Lane roles swap without duplicates; density, variation, fill, accent/ghost
  velocity and +/-1/16-step human timing are bounded. Sixteen pending lane events
  per track provide one-step lookahead for early hits. Anchors retain existing
  source hits, velocities, accents and timing; deleting a hit cannot resurrect it.
  The source bank is separate from an empty REC destination.
- Four melodic duration owners and eight independent drum key owners retain MIDI
  release ownership. Captures store the performed fractional onsets in the
  existing tempo-relative events; STEP replay bypasses generator clocks.
  Capacity is checked conservatively before a fast/polyphonic take is armed.
- New controls use the existing page/navigation system. Ongoing local/freeze
  status is in the shared header. No screen/layout redesign is included.

## Code and storage

`studio7.c` connects the sequencer and generators; `gen.c`, `gen.h` and
`gen_grids.h` contain the pinned donor helpers. `studio7_clock.h`,
`studio7_kit.h`, `studio7_state.h` and `studio7_step.h` hold pure tested contracts.
`ui_studio7.c`, params/UI files expose controls. `seq.c`, `tessera.c` and
`tess_features.c` integrate timing, capture, harmony and note lifetimes.
`project.c`, `pattern_store.c`, `tracks.c` and `editor_backup.c` handle persistence.

FBK7 retains the exact 27,904-byte A/B allocation and object IDs. All 1,792
inactive-bank steps use a lossless 61-bit record instead of 64 bits, recovering
672 bytes without removing inactive fields. S7G1 uses 644 bytes, including all
four generators' inactive settings, complete AWAKE sequences and 64 anchor masks
per track. Active step records and the existing FUN22 and 296-byte custom blocks
keep their old wire layouts. FBKK and Performance 6 FBKT import through their old
offsets and are converted only on a subsequent save/rename/migration. No new NOR
sectors, loader changes or 0x9D000 writes are involved. Older firmware cannot read
FBK7; retain a Performance 6 backup for rollback.

The secondary canvas is reduced from 24 to 4 rows to recover 9,600 bytes of SRAM.
Larger rectangles use its existing primary-buffer fallback and wait for DMA.
The LCD ownership test covers both overlapped small draws and the new fallback;
this trades some main-loop display overlap for audio memory, without changing
screen dimensions or audio processing. Target resource checks remain mandatory.

`eng_fm6.c` skips per-sample correction bookkeeping on blocks with no correction
active or pending. The original sample math is retained; all 337 sound hashes
and CPU budgets passed in the focused and complete baseline suites.

## Verification record

- Performance 6 full host baseline: passed after the FM6 neutral fast path;
  337 unchanged sound hashes, no health, voice or CPU budget failures.
- Local .7 integration: new modes, repeatability, STOP ownership, grouped 5/8
  with 3:2, freeze, fractional capture/STEP replay, full saves, track removal and
  malformed encoder deltas passed.
- Pure modules: 6,816 hit/gate encodings, 200,000 full step round trips,
  four million rational clock events, 131,072 deterministic kit trials,
  state round trips and malformed/truncated input rejection passed locally,
  including AddressSanitizer/UndefinedBehaviorSanitizer in the foundations stage.
- Original generator note/state baseline: unchanged after adapter integration.
- Actual .7 full local host/web run: passed in 164 seconds, including target
  disassembly budgets and registered recovery tests. All 337 golden renders remain
  unchanged with no health, routing, CPU-budget failures or crashes.
- Target `8057dbd`: native Linux compilation and package creation passed. Audio
  working arena 49,600 bytes (minimum 49,152); four largest engine states 30,448.
  Resident-call checks and unavailable-runtime-helper checks pass.
- Actual .7 emulator v0.2.0: package boot/note audio and synthetic four-track
  AWAKE/POLY/BOUNCE/KIT project completed 3,000 emulated milliseconds each;
  45/148 LCD frames, audible-range PCM and zero clipped samples. Package/application
  hashes match the rebuilt target. Eight unsupported MMIO addresses and an offline
  CPU1 worker limit this evidence to the emulator's serial fallback.
- Final CI full host/web plus strict/sanitizer checks: passed, 20 October 2026.
  Full suite completed in 434 seconds, with 337 unchanged renders and no budget,
  routing or health failures. Run:
  https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39449681801 .
- Hardware check remains for cold starts, real MIDI/USB, busy kits and long sessions.
  The user reports no active audio faults in the current installed build.

## Roadmap after this patch

Hardware validation and measured performance polish first. Sound/timbre morphing,
BRANCH/LOGIC, SLOTH/OCHD and MPE remain later phases; do not import the donor's
engine-removal profile or MACRO replacement. Continue preserving the working
studio engines, atomic saves, preset compatibility and existing audio baselines.
