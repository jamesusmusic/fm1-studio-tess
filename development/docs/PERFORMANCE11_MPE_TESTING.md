# Inputsignwave Performance 11 — USB MPE Dev1

By: Inputsignwave

## What this candidate does

Live lower-zone MPE on **WAVE LAYERS** from the Seaboard RISE 25, through
macOS and the FM-1 USB MIDI input. Existing engines, presets, NIGHT UI,
menu layout, stereo reverb and warm drive are retained. No save addresses,
project field counts or existing route IDs change. Four new route enum values
are appended: `MPE T1` through `MPE T4` (5–8). Existing firmware interprets
these new values differently: restore an ordinary route before rolling back.

One fixed destination track, up to the existing **four WAVE LAYERS voices**.
Manager channel 1, members 2–16; defaults ±48 semitone member bend and ±2
manager bend. RPN0 can negotiate semitones/cents; RPN6 can reduce/disable the
lower zone. USB is dedicated to that zone while an MPE route is selected;
unused channels do not fall through to other tracks. TRS retains CH1–4 routing.
Outside MPE routes the previous ordinary USB/TRS routing remains unchanged.

- **Strike:** note velocity, through the existing sound envelopes.
- **Glide:** independent member bend plus manager bend, smoothed per voice.
- **Press:** individual amplitude from 50% to 100%, plus a bounded filter opening.
- **Slide:** CC74, centred at 64, offsets table position and filter for both layers.
  Existing scans/morph/LFO/filter settings still apply, so gestures can hit limits.
- **Lift:** note-off velocity is retained in the voice snapshot; no additional
  Lift-to-release modulation is mapped in this first candidate.
- Manager CC64 sustain; CC120 All Sound Off, CC123 All Notes Off and CC121 reset.

Patch parameters are never rewritten by gestures. Equal pitches on different
channels receive independent voices. A physical note-off freezes that note's
expression, including while sustained; later member-channel reuse cannot bend
its old tail. Reconfiguration, route changes, panic, USB suspend/reset and lost
queue data release affected ownership. The shared admission/stealing policy and
bounded 64-message dispatch per audio block remain in force.

This is **live input only**. MPE notes bypass the scale/chord/ARP/recording input
expansion. Existing sequencer/keyboard playback still works, but do not expect
on-device REC to capture this live MPE performance or its expression curves.
FM6 and Prophet do not yet render per-note MPE. Selecting an incompatible
engine on the MPE destination makes the USB zone silent; choose WAVE LAYERS or
restore an ordinary route. Direct RISE-to-FM-1 Bluetooth is separate work.

## Connect and audition

1. Install the accompanying `.fwsc` using `INSTALL.command` or the command in
   the delivery README. Keep the known-working RC4 package for rollback.
2. Connect **both RISE 25 and FM-1 to your Mac using USB data cables**.
   No direct USB cable between the two instruments: both need a host.
3. In ROLI Dashboard select Multi Channel / MPE On, **Lower zone**, manager
   channel 1, member channels 2–16 (15 members), **Pitch Bend Range 48**.
   Set Slide/Press tracking to enabled. ROLI's setup references:
   [Dashboard manual](https://roli.com/us/manuals/dashboard) and
   [lower-zone setup](https://support.roli.com/en/support/solutions/articles/36000024602-mainstage-using-the-seaboard-rise-grand-with-mainstage).
4. On FM-1 choose track 1, engine **WAVE LAYERS** and an evolving preset such
   as VOWEL GLASS. In **GLOBAL → SYSTEM → ROUT** choose **MPE T1**.
   The shared header shows `MPE T1` while this route is selected. T2–T4 pin
   other destinations if wanted. Ordinary `CH1-4`/`SEL` restores normal MIDI.
   Start with REC off, transport stopped and a moderate output level.
5. Run the bundled `seaboard_usb_bridge.py --list` to get exact port names,
   then `--input "<RISE input>" --output "<FM-1 output>"`.
   The delivery README supplies the working local Python path. This script
   forwards channels unchanged and excludes updater SysEx. Do not run another
   DAW/IAC routing path in parallel; double routing can duplicate notes.
6. Hold two different notes: bend one while keeping the other stationary.
   Press and slide each independently. Release/retrigger one while the other
   stays held. Try sustain, repeated same notes and then a five-note chord;
   a voice steal after four notes is expected, not fifteen-note polyphony.
7. Add the working REVERB and warm drive, then sequencer playback on the other
   tracks. Listen for pops, stuck notes or whole-chord bending; note the sound,
   FX settings and gesture if anything fails. Unplug/reconnect USB and verify
   all held MPE notes release. Stop the bridge with Ctrl-C to release the zone.

If silent: check the bridge's exact ports, `MPE T1`, WAVE LAYERS on track 1,
track mute/level, and the controller's lower-zone mode. If bends are wrong,
match Dashboard's range to 48 or negotiate the same RPN0 member range.
Do not collapse the input to channel 1 in a DAW.

## Validation and limits

`tests/mpe_test.c` uses the actual source-tagged MIDI queue, allocator, ownership,
controller and stereo renderer, with ASan/UBSan. It covers equal pitches, member
and manager bends, pre-note controls, pressure/slide, note-off velocity, sustain,
repeated notes, channel reuse, steals, USB/TRS isolation, RPN zone/range, route
transition, incompatible engines, resets, panic, overflow and 12,000 mixed
queued events. Separate PCM cases verify audible control response. The legacy
controller/ownership cases run again after MPE. The bridge has offline routing,
ambiguity and shutdown tests. Full production and historical-reference suites
retain their existing golden hashes and budgets.

Native build measurements, final test results and emulator scope are recorded
in the delivery's VALIDATION.md. Host/emulator timings are not physical CPU
headroom measurements. Real RISE/FM-1 latency, gesture feel and USB load remain
hardware acceptance tests for this candidate.
