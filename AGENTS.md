# Studio Tess contribution requirements

- Credit this project's custom contributions using the exact name `Inputsignwave`; use `By: Inputsignwave` in contributor notices and documentation.
- Preserve all upstream author/copyright notices and SPDX identifiers. Scope Inputsignwave notices to our additions and modifications, never unchanged vendor code. See CREDITS.md and Tess_Developer_Handoff.md.
- Keep the existing synth engines, including FM6, CZ-1 and Prophet. Port Grids, BOUNCE, AWAKE and POLY selectively; do not adopt the donor's engine-removal profile or enable MACRO as a replacement.
- Retain our FBKT atomic project storage, legacy migration and distinct object IDs. Adapt donor TSR3/TSR4 data to this storage; do not reintroduce the 0x9D000 collision.
- Generator integration is planned work, not implemented by the attribution update. Preserve Performance 6 as the comparison baseline and validate persistence, MIDI/REC lifetimes, harmony, note duration and dual-core budgets before packaging a candidate.
- Consult performance6/source/AGENTS.md and BUILDING.md before source changes.
