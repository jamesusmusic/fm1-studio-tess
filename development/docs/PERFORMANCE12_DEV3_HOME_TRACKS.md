# Performance 12 Cassette Dev3 — restore HOME track access

By: Inputsignwave

Dev2 hardware feedback identified a navigation regression: the single-instrument
landing view removed the useful multi-track overview. This fix restores that
functional page in the approved new theme. Retired colour themes stay excluded.

## Controls

- HOME on the instrument screen opens TRACKS with one tap.
- Further taps cycle TRACKS → CHORDS → INSTRUMENT → TRACKS.
- HOME from an editing page opens TRACKS directly. Holding HOME still opens the
  existing menu without cycling views.
- ALGORITHM selects an existing instrument track from any HOME view. Fast turns
  can traverse multiple tracks and stop safely at the first/last existing track.
  Its special FM6 algorithm-edit assignment remains unchanged on FM6 EDIT pages.
- SELECT is also a direct track picker on TRACKS, with a visible footer hint.
  Its saved HOME SELECT assignment remains active on the other HOME views.
- Existing sound knobs continue to edit the selected track, with their temporary
  control/curve display. HOME or a track-picking gesture exposes the overview
  immediately. Preset browsing remains assigned to PRESETS.

## Overview and compatibility

The restored rows show track numbers, sound names, selection, pattern/step state,
mute/record feedback and the existing category information. The new UI's palette,
clipped panels and instrument landing pages remain. Header status priorities,
including MPE, waiting/recording and generator timing states, are preserved.
The chord view remains accessible; no existing sound, storage format, preset,
generator, recording/MIDI ownership or effect behaviour is changed.

Restoring the functional row renderer has a flash/RAM cost. Measure it from the
native target artifacts and keep the existing CPU/SRAM limits. Do not restore old
theme assets or claim the useful track page is an obsolete aesthetic.

Validation includes one to four tracks, both fonts, three-page cycling, direct
SELECT/ALGORITHM focus changes and extreme turns, editing-page return, HOME hold,
knob-gesture dismissal and held-note ownership. Render/lint every new overview
and the retained chord view. Run the registered full suites and exact-package
chip emulator; physical acceptance of the navigation remains required.
