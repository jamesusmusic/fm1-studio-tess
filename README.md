# Inputsignwave — custom firmware for the M-VAVE FM-1

**By: Inputsignwave**

Inputsignwave is the name of our firmware, previously called Studio Tess. It
combines Melodee synthesizers, Tess/Tessera generators and our original features,
integration, fixes and hardening. All upstream and third-party copyright and
licence notices remain intact; see [CREDITS.md](CREDITS.md).

## Current development build

The `studio-tess-8` branch contains Inputsignwave Performance 8: conditional
generator pages retained in SEQ, HOME actions, Sound Designer in EDIT, OUTPUT
second in GLO, the FM6 algorithm encoder and provisional LO-FI colours.
About displays **Inputsignwave** and **By: Inputsignwave**. Application USB,
editor identification and console use Inputsignwave. Bundled tools recognise
the new name and legacy device names. Update/recovery identities, protocol IDs
and FBK7 storage remain compatible. Repository and branch names remain stable.

The branded candidate is `1.4.1-inputsignwave.8-dev2`. Fresh validation is tracked
in [the branding record](development/INPUTSIGNWAVE_BRANDING.md). The preceding
navigation candidate passed [these checks](development/STUDIO8_VALIDATION.md).

- [Page/function guide](development/docs/STUDIO8_PAGE_GUIDE.md)
- [Development source](development/)
- [Roadmap](FM1_Studio_Tess_Roadmap.md)
- [Current draft pull request](https://github.com/jamesusmusic/fm1-studio-tess/pull/2)

Performance 7 is the user's working device baseline, reported as working
perfectly. Hardware acceptance of later builds remains separate from software
validation. No device is automatically flashed. Performance 6 remains preserved.

## Developer handoff

- [Complete handoff for Tess and Claude Code](Tess_Developer_Handoff.md): human-readable summary, all six development stages, detailed fixes/hardening, affected code, persistence, tests and porting guidance.
- [Word handoff](Tess_Developer_Handoff.docx)
- [Current studio roadmap](FM1_Studio_Tess_Roadmap.md)

Give Claude Code the Markdown handoff together with this repository. Performance 6 source is preserved in `performance6/source/`; active Inputsignwave source is in `development/`. Read the source folder's `AGENTS.md` and `BUILDING.md` before editing. Paths in the historical handoff are relative to `performance6/source/`.

## Performance 6 snapshot

`performance6/` preserves the delivered package, including source/tests, main firmware, diagnostic symbols, validation logs, installation instructions, checksums and Audio5 rollback files. Its source reference is `72305f27c6b29a9c4a7d6f27541e62b3e8db93f4`. This repository starts from that complete snapshot and does not reconstruct historical Git commits.

Read [installation instructions](performance6/INSTALL.md) and [controls and hardware testing](performance6/CONTROLS_AND_TESTING.md). New HMF2 project saves cannot be read by .4/.5; rollback needs the earlier firmware and the full pre-install backup.

Recorded software validation: 114/114 host/web jobs, 337 unchanged bypass sound renders and 28 generator scenarios. These are archived results, not a fresh test run or completed hardware acceptance.

## Next phase

Hardware acceptance and measured performance/UI polish for the new generators and kit development; broader phrase/arrangement tools, macro sound morphing and full native-patch morphing later. MPE and unrelated UI redesign remain deferred. Keep Performance 6 as the comparison baseline.

## Provenance and licensing

Based on [Melodee](https://github.com/keremimo/melodee) and [Tess](https://github.com/Rastapopolix/melodee-tess). Preserve all original notices and actual contributor attribution. See [licensing](performance6/LICENSING.md), [project license](LICENSE) and `performance6/LICENSES/`. Bundled third-party components retain their respective terms.
