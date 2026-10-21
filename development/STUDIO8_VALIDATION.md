# Studio Tess 8 — navigation build and verification

By: Inputsignwave · 21 October 2026

## Developer summary

Performance 8 makes the working Performance 7 easier to navigate. Generator,
beat-development, rhythm MORPH/EVOLVE/FORM/TWEAK, meter, phase, seeds, HARMONY
and NOTE RANGE stay in SEQ. Visibility follows the selected track's PATTERN
MODE. Only OUTPUT leaves SEQ, becoming the second GLO page after MIXER.
Sound Designer moves into EDIT. No extra GENERATOR page is added to EDIT.

Hold HOME opens the Home menu: Create New Track first, then Sound tools, Track
actions, Pattern actions, Selected notes, Song actions and Shared FX, followed
by existing settings. The previous page/cursor and selected notes survive
opening and closing this menu. Existing hold-OCT+ shortcuts remain, with Create
New Track first on the overview's sheet. Sound Designer is removed from those
sheets and the former settings entry.

The physical ALGORITHM encoder selects 1–32 on ordinary FM6 EDIT pages using
the native patch/motion path; elsewhere it selects tracks. LO-FI is the default
for unset palette preferences; valid stored colour IDs remain unchanged.
See [the full page/function guide](docs/STUDIO8_PAGE_GUIDE.md).

No engine/DSP algorithm, generator algorithm, clock implementation, recording
format, FBK7 storage layout, NOR allocation, bootloader, CPU1 startup or voltage
sequence was changed. Existing engines, 32 banks and 1024 recorded events remain.
Performance 7 is the preserved working rollback. No device was flashed here.

The next feature phases remain character reverb/stereo movement and playable
repeat/slice/stutter FX; phrase development; bounded A/B sound morphing; musical
evolution with home/return; inter-track interaction; then MPE. This navigation
candidate does not implement those features or revive historical audio reports
as current faults. The user reports Performance 7 working perfectly.

## Runtime code areas

| Area | Files | Behaviour |
| --- | --- | --- |
| Page table | `firmware/src/params.c` | OUTPUT becomes FAM_GLO/SC_TRACK; new DESIGNER launcher is FAM_EDIT; all other existing page families retained |
| Visibility and navigation | `ui.c`, `ui_draw.c`, `ui_input.c` | Generator/mode filters; keep PATTERN/NOTE LENGTH reachable; move a hidden current page to a visible sibling after mode/track changes and before ordinary page input |
| Designer launcher | `params.c`, `ui.c`, `ui_draw.c`, `ui_graph.c`, `ui_input.c`, `ui_sections.c` | EDIT page/section, K1/OCT+ open, help and footer; existing designer editor unchanged |
| HOME actions | `ui_menu.c`, `ui_popup.c` | Reuse existing action handlers, guards, confirmations and undo; preserve page/cursor on close; identify track/pattern/song target; route pattern copy into PATTERNS |
| FM6 encoder | `ui_input.c` | Clamp algorithms 0–31 internally, retain native patch acceptance and automation capture; no track change in that editing context |
| Mode-specific control | `ui_studio7.c` | Hide/disable Grids CHAOS outside Grids without overwriting its stored value |
| Palette default | `tools/gen_ui_palettes.py`, `panel.c` comment | Generate default index for existing LO-FI palette; preserve palette IDs and colours |

Paths in the table without a directory prefix are under `firmware/src/`.
`tests/studio8_navigation_test.c` exercises all engine IDs and PATTERN modes,
EDIT section ownership, generator visibility, preserved values, hidden-page
fallback, GLO order, HOME context, selected notes, pattern copy, track guards,
song targeting and all 32 FM algorithms with extreme encoder deltas.
Existing UI/designer expectations now reflect the intended page order. Partial
protocol/persistence harnesses supply their omitted list-view stub because
track navigation now checks visibility. No test sound baselines were rewritten.
`tools/studio8_pageguide.c` produces the source-derived page/control inventory.

## Immutable build identity

- Compiled GitHub commit: `62d093ae2875173b34ccaa39ca1882a917fd83c1`.
- Equivalent local implementation commit: `d39b86291cfd30c2aca207fa602afd6bed1aa980`.
- Both complete repository trees: `2c5a4737936edf6386532cf2b9ee0080fe71c78d`.
- Candidate: `1.4.1-studio-tess.8-dev1`, `MELODEE_RAM_AUDIO=1`, dual core.
- Firmware: 610,534 bytes; SHA256 `e6d1abb18b0f535daeff26bbfdf16ce4f62a11d141729ba3d13eb44398fa756e`.
- Application: 568,412 bytes; SHA256 `f1f562018169b5325fbe441c1ca3a84ba9ccb6f3866b73dd3b6c8a1248218e3e`.
- Target ZIP artifact: `12351242179`, SHA256 `0cefd26bfa643aafdd0fd16dd222f7e77164b09faf4dcd1649949e59381f9954`.
- Final documentation may follow this compiled commit; firmware/runtime source
  must remain byte-identical to the compiled tree when packaging.

## Verification completed

[CI run 39612840717](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39612840717),
job `119265977201`, completed successfully on the compiled commit:

- Native Linux target compile, pinned SDK hashes, resident call graph and
  unavailable-runtime-helper checks passed. No fault-injection build was used.
- Working audio arena: **49,600 B**, minimum **49,152 B**. Ordinary data uses
  **97,760 of 98,304 B**, leaving **544 B**. Low/retained SRAM tails are 1,636 /
  784 B. These tight limits remain a constraint on future features.
- Strict pure timing/step/kit/state/math builds, generator contracts and
  AddressSanitizer/UndefinedBehaviorSanitizer checks passed, including actual
  sequencer/storage integration and new navigation coverage.
- Full Linux host/web suite passed in **298 s**. All **337** sound renders
  unchanged; zero health, voice/routing, CPU-budget failures or crashes.
- Complete local host/web suite passed in **165 s** on the equivalent sources.
  Navigation ASan/UBSan also passed locally.
- UI: **261 scenes × eight palettes** plus page/value sweeps; **zero layout
  lint findings**, zero MONO colour violations. Two allowed ellipsised free
  texts; 53,518 column values swept and 671 FM6 charts linted.
- Four focused LO-FI renders (HOME actions, Designer launcher, OUTPUT and pattern
  actions) passed layout lint and visual inspection.

## Exact-package chip emulator checks

Used the already pinned native `fm1-emu` v0.2.0:

1. Actual `.fwsc`, 3000 emulated ms: boot/note audio, 47 LCD frames, 123,904
   stereo frames, peak 5,481, zero clipped samples.
2. Extracted XIP from that same package, with the real encoder's synthetic
   FBK7 project: FM6/AWAKE 5/8 at 3:2, CZ/POLY 7/8 at 5:4, Prophet/BOUNCE and
   eight-lane DRUM/KIT. 3000 emulated ms, 155 LCD frames, 125,184 stereo frames,
   peak 3,759, zero clipped samples. Display showed the four tracks playing and
   the selected track's local 5/8 status.

These are bounded boot/playback smoke checks. CPU1 is offline in this emulator;
eight unsupported MMIO addresses remain. They establish serial-fallback
behaviour, not physical CPU percentages or dual-core hardware acceptance.
Native host tests separately exercise concurrent/serial PCM and MIDI parity.
UI/menu semantics are verified by the actual firmware UI harness, not claimed
from the emulator's named-button mapping.

## Delivery and physical acceptance

The local bundle contains the installable `.fwsc`, application, exact symbols,
source ZIP, patch from Performance 7, page guide, this report, checksums and
validation logs/screenshots. The earlier Word/Markdown developer handoff remains
an unchanged history of Performance 6; this report and the source ZIP describe
Performance 8. No hardware flash, PR merge or main-branch update was performed.

Remaining device checks: new HOME entry/return and selection context; SEQ page
visibility while changing modes/tracks; Sound Designer; FM6 encoder and track
selection; GLO OUTPUT; saved palette/project recall; cold starts, MIDI/USB,
battery/USB power and longer dense playback. Preserve project backups and the
working Performance 7 firmware until those checks are accepted.
