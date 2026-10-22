# Prophet artist-inspired and classic sound auditions

By: Inputsignwave

22 October 2026. User requested Radiohead, Aphex Twin and familiar classic Prophet
sounds, and offered their Pro-800 banks. Standalone audition work only: production
synthesis, firmware packages, device data and Desktop originals remain unchanged.

## Delivered listening set

Sibling folder `Inputsignwave_Prophet_Auditions_2026-10-22`, starting at
`START_HERE.html`: **20 dry sounds and eight approved-reverb examples**.

Twelve newly designed native Prophet patches:

- RH Right Keys, RH Open Keys (a brighter variation), RH Staircase Haze,
  TY Dawn Pulse.
- AFX Tiny Pulse, AFX Pulse Stabs, AFX Soft Triangle, AFX Ambient Veil.
- ISW Sync Lead, ISW PWM Strings, ISW Poly Bell, ISW Velvet Brass.

Eight curated existing Sequential factory references: 511 Brass, 512 Low Strings,
514 Percussive E Piano, 517 Sync I, 528 Bass in Fifths, 541 Pulse Width Mod,
548 Clangorous Bells, 557 Hollow Sound. They already exist in our 200-program
factory bank. Native frames match the official source bytes exactly, not merely
the patch names. Do not count these as newly added firmware presets.

The 12 new patches have distinct parameter payloads with name bytes excluded;
Right Keys/Open Keys are intentionally related variations. Their 133-byte native
payloads total 1,596 B before metadata/encoding/code. This is not a compiled flash
cost or a claim that all combined update features fit. Listening acceptance and
which presets enter the build remain open.

## Provenance and limitations

Sound design adapts the published recreation approaches, not artist-authenticated
presets. No original song recordings or song MIDI were used. Exact synth identity
and record production are not asserted for these approximations.

- [Dan Carr's Radiohead recreations](https://reverb.com/news/recreating-radioheads-keys-and-synths-with-software-instruments)
- [Dan Carr's Thom Yorke recreations](https://reverbmachine.com/blog/thom-yorke-synth-sounds/)
- [Dan Carr's Aphex recreations and free Arturia reference bank](https://reverbmachine.com/blog/aphex-twin-selected-ambient-works-85-92/)
- [Audible Genius alternative Right Place recipe](https://www.syntorial.com/preset-recipe/radiohead-everything-in-its-right-place-keys/)
- [Sequential official documentation/patch sheets](https://sequential.com/support/download/prophet-5-10-documentation/)

A four-preset Arturia Prophet-5 V bank was acquired for private reference, SHA-256
`aa8e05ea1e6817496cf2d263434294311064f046d6f9ee409329bda4c821dfd9` for its
24,312-byte ZIP. It is not wire-compatible with our Prophet and these sounds are
not direct numerical conversions or Arturia renders. Envelope/filter/modulation
curves, multi-voice tuning and plugin effects differ. AFX Ambient Veil and the four
ISW designs are original explorations; none is labelled a recovered artist patch.
Keep downloaded source-bank authorship and data rights distinct from our code.

All dry clips run the current native Prophet DSP through a dedicated host adapter
with generic arpeggios/bass/chords, neutral common tone macros and no active FX.
Preview playback uses a constant per-patch peak-matching gain, not a limiter or
perceptual loudness match. The last 50 ms gets a container-edge fade; hardware
release/CPU behaviour is not proven by it. Eight separately labelled versions
use the approved Moving Air plate prototype, with extended tail playback. They
do not imply that reverb is already connected to shipping firmware.

## Desktop Pro-800 pack review

Found five collections and inventoried **23 SysEx files**, including repeated
bank destinations, firmware versions and technical metadata. All file frame
boundaries parse; version 0x6D/0x6E/0x6F and unknown filler occur. File counts,
product names and repeated destination banks are not unique patch counts.
The private inventory contains original paths/hashes and best-effort names.

These are useful future conversion material but **no Pro-800 payload is converted
into the 20 sounds delivered here**. Read-only protocol reference:
[Sam Staton's measured Pro-800 implementation](https://github.com/samstaton/pro800/blob/main/pro800syx.md).
Waveforms, tuning, envelopes and modulation need sound-calibrated translation;
oscillator-A triangle and other extra Pro-800 functions do not map directly.
Unknown layouts should remain unconverted. Do not publish the paid banks, private
inventory or derivative patch data merely because our software is GPL. The public
repository contains only generic tools, our recipes and this status document.

## Validation and reproduction

Every generated frame is decoded/re-encoded byte-for-byte by the existing native
C parser, with the full patch unchanged by rendering. All 20 renderer scenarios
also pass AddressSanitizer and UndefinedBehaviorSanitizer and agree with normal
render peaks. Raw output and the 28 final WAV files are audible and unclipped.
Exact per-patch settings, source references, gains and SHA-256s are in the private
bundle's PRESET_INDEX.json and validation/AUDIO_VALIDATION.json.

The complete existing host/web suite passes in **188 seconds** with the resident
audio profile and pinned offline SDK. This validates regression continuity, not
a new target firmware package or hardware playback of the new presets.

Reproduce using `tools/prophet_audition.py` with Python/NumPy, clang and generated
build/gen tables. `tools/prophet_audition.c` uses the actual host renderer and
retains the upstream software notices. `tools/pro800_inventory.py` reads local
SysEx without device access or modification. Firmware integration of selected
new patches follows listening approval and complete combined-build validation.
