# Studio Melodee + Tess integration candidate

Date: 14 October 2026. Device: M-VAVE FM-1.

## Sources

| Input | Pinned commit |
| --- | --- |
| Melodee v1.3.5, current stable at retrieval | baeabc495c5ae0a6df48c6cf81d056fbc59abb6b |
| Melodee v1.3.4, last stable published yesterday | f7cd3eafd593d9e585895bfaf2c9550a03662ac7 |
| Tess merge-upstream-1.3.1, integration donor | 34d401c17b13cc4cb0089e4de1b48c55438789bf |
| Tess main, fetched for reference | a4533b5dff19cfd1a6a7b38f508ab10944ae7094 |

Repositories: https://github.com/keremimo/melodee and https://github.com/Rastapopolix/melodee-tess.

The user's installed version is not established by installation date. Several stable and RC releases were published on 13 October. Original v1.3.4 and v1.3.5 standard firmware downloads are included and their full SHA-256 hashes matched the GitHub release metadata. These original packages are separate from the custom candidates.

## What changed

Merged the donor branch onto Melodee v1.3.5. Retained current Melodee synth engines, effects, boot protection, loader and update handling. Added Tess's Turing, Euclid, Markov, rhythm Morph, Evolve, Form, Tweak and per-project generator save/backup support. Normal STEP mode stays available. This merge does not add sound/patch morphing, MPE or Bluetooth support.

Resolved README, test runner and installer conflicts; retained current boot tests and upstream package naming. Removed stale precompiled donor site packages from the custom source tree. Kept attribution and licences. Fixed a donor restore routine to check the input length before reading its header, with a regression test for a one-byte archive.

Tess stores generator metadata in the sector pair 0x9D000..0x9EFFF. Storage tests check non-overlap and parent-project hash validation. Backup protocol adds objects 28 and 29, so use this candidate's editor/backup code when exporting generator state. A standard upstream backup does not include generator state.

## Validation

- Dual-core target builds successfully: application 503,132 bytes; audio arena 147,808 bytes, above the 142,336-byte minimum.
- Single-core target builds successfully: application 501,084 bytes; audio arena 156,000 bytes, above the same minimum.
- Both candidates use development package identity FM-1_900 and show v1.3.5-studio-tess.1; they are not official Melodee releases. Boot stub version/hash remained unchanged from upstream.
- Full host/web suite ran. Tess note-generation scenarios matched their existing golden baseline, including external clock, transport stop, mutation, pattern changes and saved-state validation. Baselines were not rewritten.
- The first run reported installer-label failures, missing recovery-loader dependency, and CZ-1 knob-test failures. Installer naming was corrected; the proper SDK dependency was supplied. Web tests and recovery tests were rerun and passed (recovery: 38 tests, one skipped).
- CZ-1 knob-test failures remain. The same test, with the donor's pointer-type correction, was run against unmodified Melodee v1.3.5 and reproduced the failure. It is documented in the donor's upstream issue report (Melodee issue #47). It is not treated as a passing test or hidden by changing the expected output.
- Backup tests passed under AddressSanitizer and UndefinedBehaviorSanitizer, including the new truncated-header check.
- Explicit single-core Tess generator-baseline and backup tests passed.
- Target RAM/flash, register-access and build checks passed. No physical FM-1, USB connection, audio audition or hardware timing measurement was used.

## Files and use

`firmware-candidates/` contains custom dual-core and single-core .fwsc packages. These are compiled development candidates, not hardware-qualified releases. `original-firmware/` contains unmodified reference firmware. `source/` contains the merged project; `original-sources/` contains pinned source archives; `validation/` records logs, and `integration.patch` describes the change from upstream v1.3.5.

To rebuild, follow source/BUILDING.md with the JieLi Linux toolchain and AC79 SDK. In source/, run ./build.sh for the standard candidate or MELODEE_DUAL_CORE=0 ./build.sh for single core. Run tests/run_tests.sh after building; the known CZ-1 issue is documented above. The custom candidates use upstream package tooling. No files were published to GitHub and no hardware was flashed.

Before hardware testing, retain a complete backup from the installed firmware. Establish the exact installed version and core variant, then check existing projects and presets, generator controls, save/reboot/load, backup/restore, Polyend MIDI clock and note release on Stop. MPE remains a later phase after this validation.
