# Performance 10 combined candidate

By: Inputsignwave

23 October 2026. Complete feature source is implemented. Final native link,
exact-package emulator and delivery measurements are being regenerated for the
hardened candidate; this document is not a physical-hardware acceptance claim.

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

## Validation

The complete registered local suite passed in 225 seconds before the final
memory reductions. It includes unchanged historical PCM goldens, 2,401 ordered
production engine combinations with serial/concurrent PCM parity, storage and
older-template migration, editor/web and UI sweeps (zero lint findings).
New sanitizer tests cover 8,192 wavetable pitch/control cases, independent layer
ADSRs/filters, exact approved FM6 unpacking, kit overlap/chokes and engine changes,
FX state/project/template persistence, shared plate/drive/LFO routing, MIDI control
round trips and recorded drum note ownership. Plate stress tests cover bounded
feedback, Swell and exact stored-tail energy for 30 seconds of closed-input Hold.

The preceding combined native snapshot linked with 744 bytes of application flash
headroom. Final source additionally compacts FM6 voices and the highest WT band,
and caps per-track state arrays to their actual polyphony: 356 bytes for WAVE
LAYERS and 4,544 bytes for FM kits on the host ABI. These are working allocations,
not total firmware RAM or physical CPU measurements. The final delivery report
will record the actual target link and exact-package checks.

The preceding exact native app emulator ran 1,800 emulated ms, produced 63,872
stereo frames without clipping and displayed 64 LCD frames. Eight unsupported
MMIO addresses remain an emulator limitation; it does not establish physical
CPU load, timing margin or analog output quality. Hardware audition remains
necessary after offline validation. No FM-1 has been flashed by this build task.
