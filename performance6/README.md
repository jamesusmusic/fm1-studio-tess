# Studio Tess Performance 6

**By: Inputsignwave** — Studio Tess custom features, integration, bug fixes and hardening. Original Melodee, Tess, Felucca and third-party credits remain applicable.

**v1.4.1-studio-tess.6 — dual-core hardware-test candidate on Audio Hardening 5.**

Read **INSTALL.md** first. Preserve a full pre-install backup: new HMF2 project
saves cannot be read by Audio5/.4. Rollback needs the old firmware and that backup.

Included features:

- Independent generator note length: STEP / FIXED / HIT, with held notes across rests.
- Fixed musical durations, bounded overlap and replacement/retrigger handling.
- Per-voice release/fade and FM6 transition protection.
- FM AMP ADSR and FM TONE macros, retaining native operator pages and patches.
- Project and FM6 sound saving, USB MIDI note lifetimes, and one-pass REC/undo.
- All prior generators, harmony, SONG, MIDI and Audio5 hardening retained.

MPE, exploratory timbre morphing and unrelated UI changes remain deferred.

Use **firmware-candidates/studio-melodee-tess-1.4.1-dual-performance6.fwsc**.
The single-core candidate in diagnostics is for comparison only. Exact crash
symbols, source, source patch, Audio5 rollback firmware/source and test logs are included.

The **114-job host/web suite passed**, including the new feature tests. All 337
existing sound baselines stayed unchanged with the new processing bypassed.
Sanitizer, migration, UI, target budget, package checks and concurrent audio/MIDI
stress passed. Optional external-reference checks require their separate dependencies;
the test logs record those skips.

**Hardware acceptance remains pending.** The intermittent FM6 Bells distortion
and actual FM-1 CPU load have not been reproduced or measured on a physical unit.
Read **CONTROLS_AND_TESTING.md** for the new controls and staged hardware checks.
