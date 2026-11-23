# Performance 12 Cassette Dev3 validation

By: Inputsignwave · 23 November 2026

Source `1e7132979a21967192e2761db8bf135f17b7f3b5`, tree
`37aa6307846da4878e7ca184b6c39b5177e426fe`, branch INPUTSIGNWAVE-GEN.
Firmware `v1.4.1-inputsignwave.12-cassette-dev3`.

## Navigation repair

User hardware feedback identified the missing multi-track overview in Dev2.
Restore that functional view in the new style. HOME cycles instrument → tracks →
chords; HOME from an editing page opens tracks directly. SELECT and ALGORITHM
pick tracks on the overview; ALGORITHM also works from the other HOME views.
Fast turns traverse multiple tracks with clamping. Preserve the saved HOME SELECT
assignment on the other views, FM6 EDIT's algorithm control, preset browsing,
HOME hold/menu, sound-knob overlays and held-note ownership. The overview's
footer advertises its direct track picker. Keep header status priorities.
Only the new colour theme ships; retired palette assets and COLOR stay excluded.

## Measured native memory

| Resource | Dev2 | Dev3 |
| --- | ---: | ---: |
| App body | 557,524 B | 562,728 B |
| Spare application flash | 15,916 B | **10,712 B** |
| Main audio arena | 50,880 B | **50,752 B** |
| Data arena | 1,552 B | 1,536 B |
| Low / retained arena | 4,228 / 784 B | unchanged |
| Ordinary RAM through BSS end | 96,752 B | 96,768 B |

The useful overview costs **5,204 B flash** versus Dev2. The theme table remains
**52 B / one palette**, not the old 416 B / eight themes. Body limit is 573,440 B;
main arena minimum remains 49,152 B. Arena banks are disjoint; changes include
link-layout/alignment effects. The 3,808-byte boot stub is byte-identical to RC4.

## Completed validation

- Local full registered host/web suite: **passed in 410 s**, including production
  and historical profiles, unchanged sound goldens, MIDI/recording ownership,
  storage, generators, wavetable/plate/drive, MPE and concurrent audio checks.
- New HOME sanitizer test: 1..4 tracks, both fonts, all three views, direct/fast/
  clamped SELECT/ALGORITHM picking, knob-overlay dismissal, HOME from EDIT,
  HOME hold and held-note ownership passed.
- Production UI lint: added overview/chord scenes for 1..4 tracks and both fonts;
  zero layout findings and zero grayscale failures. Existing historical two-view
  fixtures stay intact for comparison, while production exercises three views.
- Native chip build, static CPU guards, audio-to-SRAM checks and retained-asset
  inventory passed without rewriting goldens or increasing budgets.
- Package checksums, chip key, app boundary and recovery loader checksum passed,
  including a standalone check from this install folder. Source archive: all
  **750** development files verified against the source commit's Git blob hashes.
- DSP, preset identities, storage partitions/project formats, generators, MIDI
  protocol and reverb/drive processing are unchanged. No new MPE/BLE work added.

## Final checks

Exact-package chip emulation completed six seconds: **249,216 stereo
frames** at 44.1 kHz, peak **720**, **zero clipped channel samples**, and
**251 LCD updates**. The scripted front-panel run exercised HOME to tracks,
SELECT to track 2, HOME to chords and the instrument page, ALGORITHM to track 1,
and HOME back to tracks while a keyboard note was held. Six LCD captures verify
the views and selected row. Used synthetic project data with dual WT, FM kit,
Prophet, FM6, stereo plate, drive and synced FX LFO. Eight known unsupported
MMIO addresses remain in the model; no crash screen was observed.

Linux native build, foundations/sanitizers and full registered host/web suite:
**success**, full suite **848 s**; 349 unchanged audio goldens, no health,
voice/routing, CPU-budget or crash failures. New navigation sanitizer and the
expanded production UI render sweep passed. No goldens/budgets were rewritten.

[Validation run](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/45213318183),
job `137827633797`, native artifact `14838120327`. Artifact SHA-256 was verified
before extraction. Full Linux output is included in validation-evidence.

## Limits

No physical FM-1 was flashed. Hardware acceptance of HOME/navigation remains
required. The chip emulator models a serial/peripheral subset; its scripted TRS
MIDI does not establish Mac USB-MPE/Bluetooth or physical dual-core timing,
stack high-water, bus/cache contention or actual LCD motion. Host/static weights
and emulator statistics are not physical CPU percentages. Earlier packages are
preserved for rollback. See README_INSTALL.md and UI_GUIDE.md.
