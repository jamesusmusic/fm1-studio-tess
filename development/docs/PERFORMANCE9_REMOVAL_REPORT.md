# Inputsignwave Performance 9 removal baseline

By: Inputsignwave

21 October 2026. Candidate `1.4.1-inputsignwave.9-removal1`, compiled from
`fbc8b5c89e895b3028c0ba6a3aa34dd031c87814`. This is the measured foundation
for the next engines and reverb. They have **not** been added yet. No hardware
was flashed or used for these measurements; installed Performance 8 Dev2 and
historical archives remain available for rollback.

## Developer summary

SID, CZ, SID drum synthesis, ROOM and REVERB are absent from the target DSP.
Prophet, FM6, 808/909 drums and other FX remain. The third default track now uses
FM6. IDs 3/15, drum-kit value 6 and FX values 7/8 stay reserved. Legacy engine
tracks and SID kits play silently; legacy ROOM/REVERB slots bypass without
allocating delay memory or muting the dry signal. Raw CZ patch bytes remain in
project compatibility data and survive load/save. Replacement algorithms must
receive fresh IDs. Retired engines cannot be chosen in normal browsing or
Sound Designer; their saved identities display as unavailable. Hardware timing
and endurance are still acceptance work for the eventual combined build.

## Exact target measurements

Baseline app SHA-256:
`8505cd80a47fbe1ce1c80bd4c38469262241719b69dc5f729cafce3c5948e65b`.
Candidate app SHA-256:
`00c31c7a88f917e3b017df3725ab61bff878d33ef799b84ceebdf8aebd7e4ac3`.

| Measurement | Performance 8 Dev2 | Removal baseline | Change |
| --- | ---: | ---: | ---: |
| App including unchanged 3,808-byte stub | 568,604 B | 533,616 B | 34,988 B smaller |
| Free space in app body slot | 8,644 B | 43,632 B | 34,988 B recovered |
| Ordinary SRAM consumed through BSS end, including alignment | 97,760 B | 95,184 B | 2,576 B recovered |
| Main contiguous audio arena | 49,600 B | 64,640 B | 15,040 B larger |
| Ordinary-data SRAM tail | 544 B | 3,120 B | 2,576 B larger |
| Low-SRAM tail | 1,636 B | 4,260 B | 2,624 B larger |
| Retained-SRAM tail | 784 B | 784 B | unchanged |
| Total of those audio-working banks, excluding optional cache | 52,564 B | 72,804 B | 20,240 B recovered |

The app body limit remains `0x8C000` (573,440 B). The main audio-arena gate
remains 49,152 B. Flash figures describe the existing app slot, not unused NOR
elsewhere. SRAM banks are disjoint: **72,804 B cannot be allocated as one delay
buffer**, and active engines, note history and undo also need that memory.
Optional cache RAM is excluded. Removing an unselected engine does not free its
whole dynamic state; these gains chiefly reflect less resident DSP and FX state.

Reproduce with the actual ELF/map/disassembly/binary artifact directories:

```sh
python3 tools/removal_budget_report.py BASELINE_DIR CANDIDATE_DIR --output budget.json
python3 tests/target_budget.py CANDIDATE_DIR/melodee.dis tests/target_budget.txt
```

## CPU evidence and its limits

All target cost gates pass without rewriting their limits. The following are
weighted static instruction scores from the target compiler, including the
existing extracted-helper accounting. They are **not cycles or device CPU %**.

| Target path | Dev2 | Removal baseline |
| --- | ---: | ---: |
| FM6 renderer and extracted helpers | 3,560 | 3,560 |
| Prophet sample kernel | 1,065 | 1,065 |
| Prophet modulation kernel | 105 | 105 |
| Drum renderer and extracted helper | 72 | 75 |
| Track renderer and extracted helpers | 606 | 558 |
| Time-FX kernel, now ECHO only | 345 | 179 |
| Shared-FX kernel | 202 | 203 |

The small drum/shared-FX differences remain inside existing gates. Removing
SID and old reverb eliminates their processing when those sounds would have
been active; it does not establish a fixed CPU saving for a dry Prophet/FM6
project. Native host benchmarks separately record instruction counts and elapsed
time for retained FX, four tracks, three-slot chains and shared/separate sends.
Those host measurements are comparisons, not FM-1 deadline measurements.

## Validation

- Full local suite: **passed in 210 s**. Production checks run first; historical
  fixtures explicitly compile with `MELODEE_RETIRED_REFERENCE=1`. Reference mode
  is rejected by the target build and is never included in this firmware.
- Production profile: **236 bit-identical PCM comparisons**, covering every
  retained engine preset (808/909) and retained FX against the historical
  implementation. Rendered energy is checked so silence cannot produce a false
  equality pass.
- Production concurrency: **256 ordered assignments** of Prophet, FM6, 808 and
  909; two notes per track, live edits, release/retrigger, spread and shared
  Chorus. Serial/concurrent PCM matches; matrix hash `fb6bacbaf07b3b6e`.
- Production allocation sweep: **256 four-track combinations**, peak **30,448 B**,
  zero allocation failures. This is host dynamic-state usage for these scenarios,
  not a complete worst-case target RAM or stack proof. Host arena capacity and
  pointer layouts differ from the device; full FX chains can require much more.
- Production ASan/UBSan: legacy project round trips, silent retired sounds,
  no-allocation dry bypass, live 808-to-retired-kit release and the allocation
  sweep pass with UBSan configured to halt on error.
- Historical regression: **337 unchanged sound renders**, zero health,
  voice/routing or CPU-budget failures and zero crashes. These retain coverage
  of archived DSP; the separate production comparisons establish the reduced
  profile's retained sound behaviour. No golden/CPU limits were regenerated.
- Actual target disassembly confirms no `sid_*`, `cz_native_*`, SID-drum render
  or `fx_reverb*` functions remain. Linker/resource/residency checks pass.
- Production UI renderer: **261 scenes across eight palettes**, plus page/value
  sweeps; zero lint findings and zero MONO pixels off gray. This is separately
  compiled with the default production profile.
- Exact final `.fwsc` emulator smoke: **3,000 emulated milliseconds**, 46 LCD
  frames, 123,904 stereo frames at 44.1 kHz, peak 5,513, zero clipped samples.
  Eight MMIO addresses remain unsupported in pinned `fm1-emu` v0.2.0; the run
  exercises serial fallback, not physical dual-core startup or cycle accuracy.
  Its instruction-derived ISR percentage is deliberately not used as device CPU %.
- The first target build was also smoke-tested; final-package evidence above
  refers to the corrected target app hash in this report.
- [Linux CI run 39675736561](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39675736561):
  **passed**, job `119485058339`; native target/resource/residency checks,
  foundation ASan/UBSan, production checks and complete host/web suite
  (**360 s**). Historical 337-render regression has zero sound/health/routing/
  CPU-budget failures and zero crashes. The build is not a hardware certification.

The new tests exposed host memory issues: pointer-bearing 909 state required
8-byte-aligned host allocation starts/banks, while the target remains 4-byte
aligned. Allocation sizes and cache matching retain their existing rounding.
A signed left shift in drum output was replaced with equivalent multiplication;
PCM comparisons verify that sound is preserved. None of these changes alters
bootloader, flash partitions, update protocol or dual-core startup.

## Code areas

- `firmware/src/engines.c`, `core.h`: active engine list, state allocation,
  preserved IDs, retired identity and default third track.
- `retired_audio.c`: small compatibility definitions/raw CZ payloads; no SID/CZ
  synthesis or factory bank. Existing historical source files retain notices.
- `eng_drum.c`, `voice.c`: remove SID kit/core callbacks and CZ/SID render and
  modulation branches; preserve retained drum models and scheduling.
- `fx.c`: exclude ROOM/REVERB kernels and private histories, retain state used
  by other FX, and bypass retired IDs before memory allocation/processing.
- `params.c`, `ui.c`, `ui_input.c`: unavailable labels, hidden CZ pages and
  selection that skips retired engines/effects while preserving saved values.
- `melodee.c`, `cz_bank.c`, `editor_cz.c`, `sysex_library.c`: retire CZ transport
  servicing/new imports while preserving old storage/read/export compatibility.
- `resources.c`: host pointer alignment and existing allocation size semantics.
- Tests: `retirement_test.c`, `run_retirement_tests.sh`, `audio_matrix_test.c`,
  `fx_performance.c`, host harness and target cost checks. `run_tests.sh` clearly
  separates production checks from historical reference tests. CI builds the
  removal candidate with the default production profile.

## Next implementation budget

Proceed with the compact wavetable/wavefolder engine and one shared-send
ambient/plate reverb prototype. Start by testing an 8 KiB delay-memory target,
then compare a 16 KiB version if sound quality benefits and combined stress
results allow it. These are prototype budgets, not allocations already approved
by measurement. Reserve working room for engines, recording/undo, transitions and
future performance FX. Reassess the focused modal/resonator after the first two
prototypes have actual target reports. No full-size 64 KiB reference reverb is
shown to fit alongside the active engines.

See [reverb audition goals](REVERB_AUDITION_PLAN.md) and [roadmap](../ROADMAP.md).
