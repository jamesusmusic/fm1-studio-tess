# Seaboard RISE 25: live MPE and Bluetooth integration plan

By: Inputsignwave

## Current checkpoint

The live USB implementation is now present for WAVE LAYERS, pending the native
candidate's validation and hardware audition. See
[Performance 11 USB MPE testing](PERFORMANCE11_MPE_TESTING.md) for the exact
route, mappings, setup and limitations. The older `experiments/mpe/` files remain
the independently tested design prototype; production uses `firmware/src/midi_mpe.c`
and `mpe_controls.h`. Direct Bluetooth and full expression recording remain planned.

The original 24–32 KiB allowance was a planning reserve, not a measured feature
size. Measure the actual first voice implementation in the recovered space
before making further cuts to satisfy an arbitrary allowance.

## Live input first

1. Add explicit source + lower-zone + destination-track routing. Preserve normal
   USB/TRS channel routing while MPE is off and on all channels outside the zone.
   Do not treat the Seaboard's member channels as separate musical tracks.
2. Bind input identity to an allocated voice: source, channel, note and allocation
   generation. The present `midi_sel_on` map owns notes at track level and
   `midi_expression` changes a whole track; neither is sufficient for MPE.
   Two channels playing the same pitch must bend and release independently.
   Preserve note-off velocity for Lift: the existing `midi_event` currently
   reduces note-off to a zero-velocity note event and discards that value.
3. Preserve member bend/pressure/slide state before note-on. On note-off detach
   the released voice's expression snapshot so member-channel reuse cannot move
   an older tail. Voice stealing must invalidate the old binding atomically.
4. Start with WAVE LAYERS' existing four-note polyphony. Apply member + manager
   bend in the per-voice pitch preparation; map pressure to a per-voice amplitude
   offset and CC74 to a per-voice filter/table offset. Do not rewrite common
   patch parameters on every gesture. Keep the saved sound intact and default
   expressive mappings explicit and reversible.
5. Keep modulation preparation on CPU0 and snapshot controls for worker jobs.
   Never let incoming MIDI change data while CPU1 renders a voice. Test serial/
   concurrent PCM contracts and overload behavior, including a saturated MIDI
   stream under four-track audio and screen redraws.
6. Add panic, zone-disable/reconfiguration, source-disconnect and queue-overflow
   handling. Release the affected MPE bindings without dropping keyboard,
   sequencer or another source's notes unnecessarily. Retain existing global
   safety fallback when an overflow cannot be attributed safely.
7. Extend to FM6 and Prophet separately. Their native bend ranges, controller
   caches and shared patch data require per-voice integration, not temporary
   patch rewriting or assigning one whole track per finger.

Essential tests: equal pitches on different channels, opposite bends under
manager bend, pressure/slide independence, release tails and channel reuse,
sustain changes, stolen voices, out-of-order/invalid controllers, rapid source/
track changes, normal MIDI neutrality and bounded processing per audio block.
Test ±48 semitone member bend as well as negotiated RPN ranges; the ordinary
non-MPE RPN path currently caps generic bend at 24 and must retain its behavior.

## USB bridge before direct radio

First audition through RISE Bluetooth → Mac MIDI routing → USB → FM-1. The
existing input queue retains a source tag, but `seq.c` currently forwards only
status/channel/data to `midi_event`; extend source-aware dispatch deliberately.
The prototype contexts demonstrate isolation but are not connected to that queue.

Direct RISE → FM-1 requires a BLE Central/Host backend. The official SDK exposes
scan, select/connect, disconnect and pairing controls, but the transport lives
behind SDK archives and is not a header-only addition to our bare-metal runtime.
Build the smallest independently testable backend and measure its complete link,
RAM/stack use and audio timing before integrating. Do not equate archive size
with linked firmware size or replace our engines/storage with the official engine.

Physical RF pairing, reconnect, interference, dense expression streams and
latency need the RISE/FM-1 hardware. Chip emulation does not prove RF behavior.
Avoid Bluetooth audio in this experiment; the goal is BLE MIDI control.

## Recording and storage

Initial scope is live expressive input. Existing note recordings do not retain
all per-note expression curves. Do not advertise full MPE capture/playback until
there is a bounded, versioned expression-event format with ownership, timing,
backup/migration and capacity tests. Keep FBK8/TPLM and current object IDs intact
while the first live-input prototype is evaluated.

## References

- [MIDI Manufacturers Association MPE 1.0 specification, hosted by ROLI](https://d30pueezughrda.cloudfront.net/campaigns/mpe/mpespec.pdf)
- [ROLI gesture messages](https://support.roli.com/en/support/solutions/articles/36000019142-what-midi-messages-do-seaboards-and-lightpads-transmit-)
- [ROLI bend-range guidance](https://rolisupport.freshdesk.com/en/support/solutions/articles/36000028378-setting-the-ideal-pitch-bend-range)
- [Official FM-1 SDK BLE role/control interface](https://github.com/m-vave/fm1_open/blob/4c7496daac04a95ff10d1afdcc8f9ead2dbabc41/sdk/include/fm1.h)
- Existing platform review: `OFFICIAL_FM1_OPEN_REVIEW.md`.
