# Performance 12: cassette UI candidate

By: Inputsignwave

The existing NIGHT palette becomes the approved cassette-futurist design. Its
stored palette ID stays unchanged. Other saved palette choices are respected;
select NIGHT in Settings to see the new design if another palette is active.

## What changes

- NIGHT has a near-black blue background, ivory text and clipped panel corners.
- Selected instrument identity follows the engine, independently of track-number
  chips: Prophet luminous blue, FM6 cyan, both wavetable engines sage, native
  drums coral, FM drum kits amber. Coral is slightly brighter than the preview
  to pass the existing RGB565 selection-text contrast requirement.
- The NIGHT HOME landing page keeps its four existing engine controls visible,
  with the sound name and selected track below the animated scope. Filter and
  envelope gestures show the existing parameter curves. Other controls keep the
  scope visible, with mesh movement held steady during the gesture.
  Drum tracks keep their eight lane-hit indicators below the graph.
- The trace shows **master audio output**, not an isolated track or a simulated
  oscillator. The sparse wireframe is an artistic view of the same output.
  Silence gives a stationary flat mesh. No fabricated sound animation is used.
- The graph refresh is limited to 12.5 Hz except forced interaction redraws.
  The existing header, canvas, scope buffer and fonts are reused. There is no
  image bank, added DSP work, audio-thread drawing or dynamic allocation.

HOME still switches between the landing page and chord view. The four physical
knobs, preset/track selection, EDIT/SEQ/FX/GLOBAL pages and OCT actions keep their
existing bindings. NIGHT's landing illustration replaces its old multi-track
Stage rows; the mixer and chord view remain available. Other palettes retain
the old Stage presentation. Rubik and Melodee Pixel remain selectable.

## Scope and compatibility

This is a UI update on the existing Performance 11 USB-MPE development base.
It adds no further MPE or Bluetooth implementation. Existing DSP, preset banks,
track/project formats, storage partitions, sound IDs and retired IDs are kept.
The larger navigation and song/generator roadmap changes remain future work.

## Validation requirements

Run the full registered production/reference host suite, unchanged audio goldens,
sanitizers, every-palette layout/font sweep, native target budget guards and a
boot/audio/LCD check using the exact packaged app in the chip emulator.
`tests/cassette_ui_test.c` exercises all six production engine types in both
fonts, same-track engine-colour changes, animated frames with stable control
values, palette round trips and HOME chord-view navigation. Hardware testing
is still required to judge LCD motion and audio under actual dual-core load.
