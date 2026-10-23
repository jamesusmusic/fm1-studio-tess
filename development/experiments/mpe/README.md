# Seaboard RISE MPE controller-state prototype

By: Inputsignwave

This directory is an experiment, not an enabled firmware feature. No production
file includes this header. It adds no runtime state or code to the measured
Space2 firmware. It neither opens Bluetooth nor routes/allocates audio voices.

`mpe_controls.h` models an MPE 1.0 lower zone with a channel-1 manager, up to
15 member channels, independent 14-bit bend, channel pressure and CC74, manager
bend/pedal, RPN 6 zone configuration and RPN 0 pitch-bend sensitivity. The manager
defaults to 2 semitones and members to 48; RPN sensitivity is bounded to 96
semitones plus fractional cents. Reconfiguration resets control state and returns
an explicit result requiring the future caller to release its voice bindings.
Upper zones, MIDI-CI, voice ownership, engine modulation, persistence and gesture
recording are not implemented here. Note messages deliberately remain outside
this control-state prototype. One caller-owned context is required per MIDI source.

Run its independent host checks from `development/`:

```sh
cc -std=c11 -O1 -Wall -Wextra -Werror -g \
  -fsanitize=address,undefined -fno-sanitize-recover=all \
  experiments/mpe/mpe_controls_test.c -o build/mpe_controls_test
build/mpe_controls_test
```

Tests cover opposite member bends under a common manager bend, distinct
pressure/slide values, two source contexts, pedal ownership, RPN cancellation,
zone disable and 100,000 malformed/mixed control messages. The host context is
130 bytes. This is not the complete MPE RAM budget: per-voice bindings, release
snapshots, routing and UI still need implementation and target measurement.

The MIDI Manufacturers Association MPE 1.0 specification is the protocol
reference; see the [implementation plan](../../docs/SEABOARD_MPE_PLAN.md).
