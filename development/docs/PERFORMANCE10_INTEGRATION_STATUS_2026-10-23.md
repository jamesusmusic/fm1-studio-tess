# Performance 10 combined candidate

By: Inputsignwave

23 October 2026. **The complete combined RC1 firmware has been built and offline
validated.** This is a hardware-audition candidate, not a physical-hardware
acceptance claim. Final native source is `7a74b612485b335a94203bff6a28207178be4feb`.
See the [exact final validation report](PERFORMANCE10_FINAL_VALIDATION_2026-10-23.md).

## Included

- NIGHT on first upgrade, with the agreed menu layout. Subsequent palette choices
  remain saved. Sound Designer stays under EDIT; OUTPUT is second under GLOBAL.
- WAVE LAYERS, a new engine alongside the original six WT sounds: eight approved
  spectral families, two independent layers, per-layer ADSR/filter/tuning/level,
  scanning, A/B balance and clocked parameter morph. Six frames per family,
  four pitch bands, 5,616 bytes of shared table data. Four-note polyphony.
- Original wavetable envelope/retrigger fixes retained; smooth layer gain,
  position and filter changes, independent release and stable cache lifetime.
- Twenty-five additive FM6 voices: Eno recreations, DX7 percussion, synth colours
  and 808 SUB. Original voices and INIT keep their IDs. Three eight-lane FM kits
  include new and existing percussion; patches are captured per hit, hats choke,
  each lane has a level. Native 808/909 and individual voices remain available.
- Twelve additive Prophet patches after the original 200 programs, plus INIT.
- Shared stereo PLATE derived from the approved Mutable Instruments Rings plate:
  size, decay, predelay, tone, wet low/high cuts, damping, width, movement/rate,
  optional tape wobble, wet-return Swell and sustained closed-input Hold.
- WARM DRIVE and independent per-FX-slot LFO shape, depth, destination and rate,
  with free timing or sync/division. One shared plate instance, no track insert
  plate; unavailable duplicate selections are skipped with a status message.
- SID/CZ/SID-kit and old ROOM/REVERB remain retired. Pro-800 conversions excluded.

## Preservation and storage

FBK8 preserves every valid inactive-step field with a fixed 241-bit/four-step
codec, recovering exactly 168 bytes. The atomic bank allocation remains 27,904 B,
with unchanged addresses and object IDs; FBK7 and older readers are retained.
SFX1 uses 61 of the resulting 204 spare bytes for all slot LFO and plate controls.
TPLM adds a 64-byte template extension while retaining older template readers.
Generic user presets retain their record format. MIDI command 101 reads/replaces
the 65-byte validated sound-FX state atomically. New favorites use tagged unused
settings bytes rather than overwriting native stars. Save a full backup before
upgrading: old firmware cannot read new FBK8/TPLM saves.

## Validation and delivery

The complete registered local suite passed against the final native artifacts in
**238 seconds**: 349 unchanged golden renders, no health/routing/voice/crash
failures and no CPU budget failures. The production matrix covers 2,401 ordered
engine combinations with serial/concurrent PCM parity. New signal, storage,
MIDI/recording, UI and sanitizer contracts pass. UI sweep has zero lint findings. Linux CI also passed target compile, sanitizers
and the full registered suite (431 seconds).

Exact application body: **572,352 / 573,440 bytes**, leaving **1,088 bytes**.
Main audio working arena: **51,520 bytes**. Other disjoint banks: 2,656 + 4,260 +
784 bytes, excluding optional cache. These are arena capacities, not free RAM.

The final `.fwsc` passed package checks and direct emulator boot. A combined
synthetic song using its exact application ran 1,800 emulated ms, produced
63,872 stereo frames with no clipping and a correct NIGHT display. Eight known
unsupported MMIO addresses remain. Emulator CPU diagnostics are not physical
CPU load or deadline evidence. Hardware listening/timing acceptance remains.
Optional recording undo can be unavailable in heavy allocations; audio and
recording take priority and the UI reports this. See the report for measured
capacity cases, limitations, source identity and checksums.

Local delivery folder: `Inputsignwave_Performance10_RC1_2026-10-23`, beside the
repository. It contains the flashable firmware, source snapshot, controls, added
sounds, installation instructions, provenance, licenses and validation evidence.
No device has been flashed by this task. Pro-800 conversions remain separate.
