# FM-1 Studio Tess custom firmware

**By: Inputsignwave** — Studio Tess custom features, integration, bug fixes and hardening. Original Melodee, Tess, Felucca and third-party credits remain applicable.

Development snapshot for the M-VAVE FM-1, combining Melodee synthesizers and sound design with Tess generative sequencing and studio additions.

**Current baseline: v1.4.1-studio-tess.6 (Performance 6).** Dual core is primary; single core is diagnostic only. The user reports successful use, while the full hardware acceptance matrix and intermittent FM6 Bells fault remain unresolved.

## Developer handoff

- [Complete handoff for Tess and Claude Code](Tess_Developer_Handoff.md): human-readable summary, all six development stages, detailed fixes/hardening, affected code, persistence, tests and porting guidance.
- [Word handoff](Tess_Developer_Handoff.docx)
- [Current studio roadmap](FM1_Studio_Tess_Roadmap.md)

Give Claude Code the Markdown handoff together with this repository. Firmware source is in `performance6/source/`; follow its `AGENTS.md` and `BUILDING.md`. Source paths in the handoff are relative to that source directory.

## Performance 6 snapshot

`performance6/` preserves the delivered package, including source/tests, main firmware, diagnostic symbols, validation logs, installation instructions, checksums and Audio5 rollback files. Its source reference is `72305f27c6b29a9c4a7d6f27541e62b3e8db93f4`. This repository starts from that complete snapshot and does not reconstruct historical Git commits.

Read [installation instructions](performance6/INSTALL.md) and [controls and hardware testing](performance6/CONTROLS_AND_TESTING.md). New HMF2 project saves cannot be read by .4/.5; rollback needs the earlier firmware and the full pre-install backup.

Recorded software validation: 114/114 host/web jobs, 337 unchanged bypass sound renders and 28 generator scenarios. These are archived results, not a fresh test run or completed hardware acceptance.

## Next phase

Kit-aware drum generation around user patterns; phrase variations and fills; macro sound morphing; full native-patch morphing later. MPE and unrelated UI redesign remain deferred. Keep Performance 6 as the comparison baseline.

## Provenance and licensing

Based on [Melodee](https://github.com/keremimo/melodee) and [Tess](https://github.com/Rastapopolix/melodee-tess). Preserve all original notices and actual contributor attribution. See [licensing](performance6/LICENSING.md), [project license](LICENSE) and `performance6/LICENSES/`. Bundled third-party components retain their respective terms.
