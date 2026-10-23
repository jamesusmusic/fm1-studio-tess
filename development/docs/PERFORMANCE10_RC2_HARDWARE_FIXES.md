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

## Validation

The pre-fix fixture fails at the null hint; the corrected fixture passes under
ASan/UBSan. Final target linking, full local/Linux regression, exact-package
emulator and delivery measurements are pending. Do not infer hardware acceptance
from the host reproduction: the corrected candidate still needs the user's
repeatable sequence on the FM-1.
