# Studio Tess contribution requirements

- Credit this project's custom contributions using the exact name `Inputsignwave`; use `By: Inputsignwave` in contributor notices and documentation.
- Preserve all upstream author/copyright notices and SPDX identifiers. Scope Inputsignwave notices to our additions and modifications, never unchanged vendor code. See CREDITS.md and Tess_Developer_Handoff.md.
- User decision, 21 October 2026: remove SID and CZ-1 entirely in the next update, including SID drum kits and the shared SID core. Keep Prophet, FM6 and non-SID DRUM models; preserve retired stored IDs and historical archives. This supersedes the earlier requirement to retain CZ-1. Port Grids, BOUNCE, AWAKE and POLY selectively; do not enable MACRO as a replacement.
- User decision, 21 October 2026: retire both ROOM and REVERB in the next update and replace them entirely with our own reverb offering. Reserve both retired FX IDs and saved data, transparently bypass legacy slots, and assign new IDs to replacements. Preserve historical reference builds.
- Retain our FBKT atomic project storage, legacy migration and distinct object IDs. Adapt donor TSR3/TSR4 data to this storage; do not reintroduce the 0x9D000 collision.
- Generator integration is planned work, not implemented by the attribution update. Preserve Performance 6 as the comparison baseline and validate persistence, MIDI/REC lifetimes, harmony, note duration and dual-core budgets before packaging a candidate.
- Consult performance6/source/AGENTS.md and BUILDING.md before source changes.
