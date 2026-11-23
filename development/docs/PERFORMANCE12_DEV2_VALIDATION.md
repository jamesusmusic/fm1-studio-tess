# Performance 12 Cassette Dev2 — validation

By: Inputsignwave · 23 November 2026

Source: `5d5a36f85bef514d97f30dc79912aa77e2758348`, tree
`1ca1f7ba1f86f21d6fd129e778a2f2ffaafd09b7`, branch `INPUTSIGNWAVE-GEN`.
Firmware: `v1.4.1-inputsignwave.12-cassette-dev2`.

## Production cleanup

Only the approved new UI ships. The seven other palette assets and COLOR menu
selector are excluded. Legacy HOME rows, layout helpers, recording-row painter
and row caches are reference-only. The old palette table is **416 B**; the
production table is **52 B** (one theme). Native ELF inspection confirms the
legacy row/layout renderer is absent. Shared graph and knob helpers may be
inlined; their absence as separate symbols does not mean their features were
removed. Mixer, chord view, both fonts, all sound controls and instrument colours
remain. Historical UI fixtures stay available for host regression comparisons.

## Exact native memory

| Resource | Dev1 | Dev2 |
| --- | ---: | ---: |
| App body | 564,232 B | 557,524 B |
| Spare app flash (573,440 B limit) | 9,208 B | **15,916 B** |
| Main contiguous audio arena | 50,752 B | **50,880 B** |
| Data arena | 1,520 B | 1,552 B |
| Low / retained arena | 4,228 / 784 B | unchanged |
| Ordinary RAM through BSS end | 96,784 B | 96,752 B |

**6,708 bytes of application flash recovered.** Audio arenas are disjoint and
must not be treated as a single allocation. Main arena minimum remains 49,152 B.
Differences include link-layout/alignment effects, not just individual fields.
The 3,808-byte boot stub is byte-identical to hardware-tested Performance 10 RC4.

## Completed checks

- Native chip compilation, audio-to-SRAM placement and static CPU-budget guards
  passed, without increasing limits or changing audio goldens.
- Production UI sanitizer fixture: six engines, two fonts, live scope, stable
  parameter values, HOME/chord navigation, retired-theme fallback, COLOR removal
  and working FONT selector passed.
- New production settings sanitizer fixture: all eight tagged IDs accepted and
  migrated to NIGHT; the other 604-byte settings fields remain unchanged,
  including calibration, favourites, hold, boot, screen and zoom. Malformed IDs
  remain rejected. Production save/boot restores only the new style.
- Production UI render sweep: 261 scenes, one theme, both fonts and the existing
  page/value sweep; zero layout findings and zero grayscale failures.
- Package checksums, chip key, app boundary and recovery helper checksum passed
  independently from the install folder. Source archive: all 747 development
  files verified against Git blob hashes from the source commit.
- No DSP, preset bank, MIDI/recording semantics, generator, project format,
  retired sound/FX ID or flash partition was changed.

## Final checks

Local full registered host/web suite: **passed in 369 s**, including production
retirement and 349 unchanged historical audio goldens, no health/voice/routing
failures, no CPU-budget failures and no crashes. Production sanitizer,
settings/migration, combined FX, storage, recording/MIDI, MPE and concurrent audio
checks passed.

The exact packaged app completed six seconds in fm1-emu v0.2.0 with the synthetic
four-track dual-WT/FM-kit/Prophet/FM6 project, plate, drive and clocked FX LFO:
**249,216 stereo frames**, 44.1 kHz, peak **718**, **zero clipped
channel samples**, and **82 LCD updates**. The five-second LCD capture shows the
playing transport, retained MPE status, stable sound controls, note label and
active master-output waveform/mesh. Eight known unsupported MMIO addresses remain
in the emulator model; no crash screen was observed. Never used user-device data.

Linux native build, foundations/sanitizers and full registered host/web suite:
**success**, full suite **671 s**; 349 unchanged audio goldens, no health,
voice/routing, CPU-budget or crash failures. Production UI/settings sanitizer
tests and the one-theme layout sweep passed. No limits or goldens were rewritten.

[Validation run](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/45185873760),
job `137731373449`, native artifact `14823951513`. Artifact ZIP SHA-256 was verified
before extraction. Complete logs accompany this package.

## Limits and installation

The source and script checks do not substitute for physical acceptance. The
chip emulator models only a supported peripheral subset and serial CPU fallback;
its TRS MIDI scripts do not establish Mac USB-MPE/Bluetooth behaviour or physical
dual-core CPU timing, stack high-water, cache/bus contention or LCD motion.
Hardware testing remains the next step. No device has been flashed by this task.

See README_INSTALL.md and UI_GUIDE.md. Dev1 and the earlier hardware-tested RC4
remain untouched for rollback. Custom work: By: Inputsignwave.
