# Performance 12 Cassette UI validation

By: Inputsignwave · 23 November 2026

Source: `53795af8ca22b7f0c0b743b3d9f5d8e4e6179c4d` on
`INPUTSIGNWAVE-GEN`. Firmware version:
`v1.4.1-inputsignwave.12-cassette-dev1`.

## Scope

UI-only changes on the Performance 11 USB-MPE development base. No audio engine,
effect, factory bank, MIDI semantics, project format, reserved engine/FX ID or
storage partition is changed. The old multi-track Stage rows become the approved
instrument landing view in NIGHT. The other palette choices retain Stage rows;
menu/control bindings and HOME chord-view access remain as before.

## Completed checks

- Local full registered host/web suite: **passed in 435 s**. It includes the
  production retirement profile, retained-sound parity, ordered mixed audio
  matrix, storage/power-cut tests, transport, recording, MIDI ownership, USB,
  new wavetable/plate/drive, generators, MPE and concurrent audio checks.
- Added production UI test under AddressSanitizer/UndefinedBehaviorSanitizer:
  all six engines and both fonts, same-track engine colour changes, live-scope
  changes, stable parameter controls during animation, HOME chord-view switching,
  palette round trips. Passed; no sanitizers disabled.
- Existing all-palette page/value/font render sweep: zero layout findings and
  zero grayscale-test failures. Full report is included. Free preset/list names
  may use their existing ellipsis behaviour.
- Native chip build: succeeded using the pinned AC79 SDK and JieLi toolchain.
  All static target CPU guards and audio-to-SRAM placement checks passed without
  changing budgets or audio goldens. Static instruction weights are not physical
  CPU percentages or cycle/deadline measurements.
- Exact package: checksums, chip key, application boundary and recovery helper
  checksum passed, both in the repository and from the distribution folder.
- Boot stub: first **3,808 bytes** are byte-identical to hardware-tested RC4.
- Source archive: all **744** development files checked against Git blob hashes
  from the source commit. Attribution and licence material accompanies the build.

## Measured memory

| Resource | Previous MPE candidate | Cassette candidate |
| --- | ---: | ---: |
| App body | 562,860 B | 564,232 B |
| App body limit | 573,440 B | 573,440 B |
| Spare application flash | 10,580 B | **9,208 B** |
| Main contiguous audio arena | 50,784 B | **50,752 B** |
| Main arena required minimum | 49,152 B | 49,152 B |
| Other arena banks (data/low/retained) | 1,520 / 4,228 / 784 B | unchanged |
| Ordinary RAM through BSS end | 96,784 B | 96,784 B |

The UI costs **1,372 bytes** of flash and **32 bytes** of main arena space after
link alignment. The disjoint audio banks are not one contiguous allocation.
Existing scope/canvas/font storage is reused; no bitmap bank or new allocation
is introduced. Saved projects and presets remain in their original partitions.

## Chip emulator and Linux completion

The exact packaged app completed a six-second chip-emulator run: 249,216
stereo frames at 44.1 kHz, 101 LCD updates, 418,046 nonzero channel samples,
peak 687 and zero clipped channel samples. A separate LCD capture verified
the landing view, parameter strip, note label and retained MPE status.
The eight unsupported MMIO addresses match the known peripheral subset;
no crash screen was observed. Both runs used synthetic project data, never
a user-device NOR image. Logs, audio statistics and the LCD image are included.

Linux native-build + foundations/sanitizers + full registered host/web workflow:
**success**, full suite **800 s**, 349 golden renders unchanged, no health,
voice/routing, CPU-budget or crash failures. The new production cassette UI test
also passed under Linux sanitizers. No baselines or budgets were rewritten.

[Validation run](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/45180832239),
job `137713886663`, artifact `14820562735`. The downloaded native artifact was
verified against GitHub's SHA-256 digest before extraction. Complete Linux output
is included in `validation-evidence/linux-ci.log`.

## Hardware limits

The chip emulator models boot, front-panel interaction, LCD output and audio in
its supported peripheral subset. Its scripted MIDI is TRS, and it does not prove
Mac USB/Seaboard behaviour. Its CPU1/peripheral model is not a physical dual-core
timing test. Do not interpret host timings or emulator statistics as hardware CPU
headroom. Stack high-water, bus/cache contention, real USB reliability, display
motion/brightness and worst-case live latency still need device acceptance.

INSTALL.command has been syntax/dependency checked, but has not been executed
against a device. Previous builds and the user's physical FM-1 remain untouched.
