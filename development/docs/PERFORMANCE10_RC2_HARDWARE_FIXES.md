# Performance 10 RC2 — FX preview crash and labels

By: Inputsignwave

## Hardware report and reproduced cause

User reports repeatable red screen after selecting Warm Drive, raising it to
about 30% and playing wavetable notes. Photo shows Phase Prism and FX controls.
Fault fields: vector `00000001`, PC `020233A0`, EMU `00000000`, DBG `00000010`,
return `0202375E`. In the exact RC1 disassembly, these addresses map to
`text_fit` / `cv_free_text`, consistent with the bus-invalid read guard.

`pv_fx` indexed `hint[type]` for 12 effect IDs, but its array contained only ten
initialized strings. New Plate and Drive entries were null. The old UI sweep
only rendered columns for arbitrary parameter values, leaving the full preview
unexercised. A new ASan/UBSan fixture reproduces the fault before the change:
`gfx.c:1117: runtime error: applying zero offset to null pointer`.

## Changes and areas touched

- `firmware/src/ui_pages.c`, `pv_fx`: initialize a hint for every effect,
  including Plate/Drive; compile-time assertion requires complete FX coverage.
  Legacy retired slots describe dry signal. Rack names resolve retired aliases.
- `firmware/src/params.c`, `N_FX` / `param_format`: retired IDs 7/8 use empty
  aliases, so both picker forms omit them without renumbering saved FX. Loaded
  legacy slots display OFF. Rename on-screen WARM DRIVE to DRIVE, preserving the
  warm saturation DSP. The output control uses OUT so it fits its column.
- `firmware/src/ui_draw.c`, `head_chip_text`: legacy aliases resolve to OFF.
- `tests/performance10_fx_ui_test.c`: complete previews for all effects, every
  insert/send page, Phase Prism playback at 30% mix; retired picker omission,
  mapping round trips, bidirectional knob stepping, legacy OFF formatting and
  normal-font Drive name. Registered in `run_retirement_tests.sh`.
- `tests/ui_render.c`: full new FX previews and picker, both wavetable engines,
  all palettes, exported NIGHT frames. This supplements the old column sweep.
- Firmware/workflow version advances to RC2; no audio DSP, preset or storage IDs
  are changed by this correction.

## Candidate identity and measured memory

- Native source: `e01dbaad1dc10723bceb4d9c771aca981e1eeb32`.
- Source tree: `6f65c403c6b3c0bca2ac0e8d5538f96edc3d6c08`.
- Linux Actions run: https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39946141707
- Native target artifact: `12498741041`; archive SHA-256:
  `8b70241e5a0a1573983a28743a88e5bf9bb3776a18e497fccaf7e8f272d9def9`.
- Firmware: `inputsignwave-1.4.1-performance10-rc2.fwsc`; SHA-256:
  `bacd22ba9412f30495e032bbb630234827edeaaf01ed6a0e7ff5c4926cd073ef`.
- Application body **572,432 / 573,440 B**, **1,008 B spare**; +80 B vs RC1.
- Main audio arena **51,520 B**; disjoint tails 2,656 + 4,260 + 784 B,
  excluding optional cache. RAM capacities are unchanged from RC1.
- Package checksums, chip key, application boundary and recovery loader verified;
  exact target static CPU guards pass, without changing their budgets.

## Validation

The pre-fix fixture fails at the null hint; the corrected fixture passes under
ASan/UBSan. The complete local registered suite passed in **247 seconds**, with
349 unchanged golden renders, zero health/voice/routing failures or crashes,
no CPU entries over budget, and 2,401 engine assignments with serial/concurrent
PCM parity. New complete-screen/picker sweeps have **zero layout lint findings**
in every palette; only two permitted ellipsised free texts remain.

Linux Actions run 39946141707 passed native compile/link, sanitizers and the
complete registered suite (**426 seconds**). It used the exact source commit
and target artifact identified above. No golden sounds or CPU budgets were reset.

The final-package app was compared byte-for-byte with the linked native binary,
then run with synthetic NOR containing Phase Prism and track-insert Drive at
38/127 (30%) mix. Buttons/notes were scripted without user flash data. The first
2,200 emulated-ms run reached the FX rack with correct Drive/30% and shared Plate,
produced 81,536 stereo frames (44.1 kHz), peak 108, 82,538 nonzero channel samples
and zero clipping; 27 LCD frames, display on. The run did not show a crash screen.
Host sanitizer tests separately exercise every full FX preview panel during
playback, including the exact null-hint path reproduced from RC1.

Direct native-binary inspection independently confirms the fault and correction:
RC1's `pv_fx.hint` table at `0x02066E80` has NULL pointers in entries 10/11;
RC2's table at `0x02066ED0` has valid strings `Stereo plate` / `Warm saturation`.
All 12 RC2 entries are non-null. The native-hints JSON is in delivery evidence.
Further 2,400 ms emulator runs produced 90,368 non-clipped stereo frames and
stayed on the rack despite scripted navigation; these do not add emulator
coverage of the full preview. Full-panel coverage comes from the host sanitizer
fixture and UI renders. The hardware reproduction sequence remains the final
acceptance check.

Eight known unsupported MMIO addresses remain in the emulator. These functional
checks do not establish physical CPU utilization, deadline margin or hardware
acceptance. No FM-1 was flashed by this task. After installing RC2, repeat the
hardware sequence: select Phase Prism, enable Drive, raise MIX to about 30%,
play/release notes and adjust DRIVE / OUT / TONE. Also open Plate's preview.

RC1 should be superseded by RC2. All Performance 10 sounds/features remain;
there are no new storage formats or DSP changes in this hotfix. The existing
heavy-configuration optional-record-undo limitation still applies. Back up the
full bank before installation. Install instructions use the verified Python
runtime inside `my custom branch /FM1_Melodee_Tess_Integration/.venv`.
