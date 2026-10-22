# Approved sound direction after auditions

By: Inputsignwave

22 October 2026. Records the user's choices from the sound audition player.
These are requirements for the restarted update; they are not implemented by
this document. No restart baseline or final engine/control layout is inferred.

## Accepted and rejected imports

- Accept all eight wavetable audition directions: Vowel Glass, Metallic PM,
  Hollow Motion, Prime Spectrum, Rippling Digital, Phase Prism, Organic Voice
  and FM Spectral Morph. Bring these timbral directions into a real bank/scan
  design; do not silently reduce the accepted set to the earlier six-bank sketch.
  Target format, frame count and footprint still need profiling.
- From the original sloopDX melodic previews, accept **808 SUB only**. Reject
  the other 19 imported melodic sounds. This does not remove our existing FM6
  factory sounds, Prophet or the native 808/909 drums.
- Reject all four sloopDX FM drum sets and their imported base voices. Do not
  port its noise/sweep/burst kit machinery for this selection. Keep our current
  808/909 kits.
- Accept the **DX7 percussion audition section**: twelve previewed voices from
  VRC104A/B. The full 64-voice source banks remain a curation pool; their unheard
  contents are not automatically all approved for shipping.
- Accept all four Eno recreations and all eight previewed **more synth colours**
  from VRC106B/107A. Do not equate these eight auditioned sounds with blanket
  approval of every unheard patch in the two source banks.

That is 25 accepted FM6 preview selections (1 + 12 + 4 + 8), plus eight accepted
wavetable directions. Exact voice identities, source slots and paths are recorded
in `Inputsignwave_Sound_Auditions_2026-10-22/APPROVED_SELECTIONS.json` beside the
repository. Preserve source authors and the existing licence/provenance notes;
audition approval is a sound choice, not a third-party rights grant.

## Additive preset rule and Prophet approval

User clarification, 22 October: **all approved new presets are additions**.
Retain the existing FM6/Prophet presets, native 808/909 kits and existing WT preset
identities/sounds; add the richer WT, new FM6, FM kits and Prophet sounds in new
slots/banks. Do not overwrite or quietly repurpose the previous presets to make
room. If exact WT sound compatibility needs the existing bank representation,
profile retaining those assets alongside the new bank rather than silently
changing what old projects play. Test old-project and saved-preset recall.

All **12 new Prophet audition patches are approved**: RH Right Keys, RH Open Keys,
RH Staircase Haze, TY Dawn Pulse; AFX Tiny Pulse, AFX Pulse Stabs, AFX Soft Triangle,
AFX Ambient Veil; ISW Sync Lead, ISW PWM Strings, ISW Poly Bell, ISW Velvet Brass.
The eight Sequential reference sounds in that audition are already present and
stay present; do not duplicate them or count them as new presets. The eight
reverb-treated examples are demonstrations, not eight additional native patches.
Pro-800 source banks were inventoried but not converted or implicitly approved
as part of these 12. Preserve third-party provenance and data-rights boundaries.

This additive rule does not reverse the previously explicit retirement of SID,
CZ/SID kits and ROOM/REVERB. Preserve their old stored IDs/data and unavailable
behaviour. Update existing WT envelope/retrigger behaviour deliberately while
retaining preset identity and the intended timbre; corrections are not permission
to replace sounds. Combined storage, asset and CPU fit remain to be measured.

## FM percussion must be playable as kits

The user requests both the accepted new DX7 percussion and the FM percussion
already in our firmware as drum kits. Keep the individual FM6 sounds available
too; kit mapping must not consume or overwrite their existing preset identities.

Proposed first maps, to refine during implementation:

1. **FM ELECTRONIC:** kick, snare, hat, clap, tom, electronic percussion, cowbell,
   and reverse/metallic accent drawn from the accepted DX7 voices.
2. **FM TUNED:** existing BELL, BELLS, MARIMBA, DX MARIMBA, KALIMBA, STEEL DRUM,
   TUBULAR and PLUCK as an initial eight-lane map. Tune each lane deliberately.
   The two bell/marimba variants remain distinct patches, not duplicate names.

These maps are proposals, not completed or heard kits. Implement lane-to-patch
selection and per-hit voice state so simultaneous hits retain their own FM6
patches. Define pitch/reference note, velocity, tuning/decay, lane levels and
optional choke behaviour. Integrate with the existing eight-lane sequencer,
anchors/generation, record/capture, MIDI and project/preset storage. Preserve
native 808/909 behaviour and retired engine/kit IDs. Profile overlapping hits,
worst-case FM algorithms and mixed engine/reverb/history allocations before
setting polyphony. Do not turn a 32-voice bank import into an apparent kit without
implementing the routing and state lifetime.

## Actual build status

- **Installed/delivered Wave1:** the original four source frames and six factory
  presets, wave/fold morph, scan, tone/sub and tuning. Prophet/FM6/808/909 retained;
  SID/CZ/SID kits and ROOM/REVERB retired. No new reverb.
- **Compiled Wave2 fix, source `006b29ed860081f8d86818929f61689a381b1677`:** applies
  the missing ADSR/velocity/performance gain ramp and preserves sounding filter/
  control state on retrigger. The bank and six factory presets are unchanged.
  Local full suite passed in 286 s; Linux native build, sanitizers and registered
  host/web checks succeeded in run 39802915690. Exact-package and saved-WT
  emulator checks passed. Physical click-free acceptance remains pending.
- **Delivery distinction:** Wave2's native artifact exists locally in
  `development/build/wave2-target/`; no standalone Wave2 delivery folder/ZIP was
  handed over before the user stopped/restarted the update. The previous install
  command targets Wave1. No Wave2 flash by the user is assumed.
- **Not in either firmware:** the eight richer WT concepts, selected new FM6
  banks, new FM drum-kit implementation, or replacement reverb. The separate
  audition WAVs do not establish that those features have been integrated.

Wave2 has 28,876 B app flash free and a 62,848 B main audio arena; the existing
shared WT bank still allocates 12,384 B within the working pools. These figures
apply to the fix artifact, not the proposed richer engine or kit update.
