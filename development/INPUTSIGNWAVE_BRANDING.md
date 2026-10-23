# Inputsignwave — firmware name and attribution

By: Inputsignwave · 21 October 2026

Firmware name: **Inputsignwave**. This supersedes Studio Tess as the current
product name; historical development records retain their original titles.

## Changes

- `firmware_identity.h` centralises the name, exact contributor credit and repo
  link. About displays Inputsignwave and **By: Inputsignwave**, with readable
  version/date and retained scrollable upstream credits.
- About retains Melodee/Tess/Felucca/component notices. The QR remains the
  upstream Melodee reference and is labelled as upstream in text.
- Application USB product, editor INFO and console banners use Inputsignwave.
  INFO retains its 24-character string bound and wire layout; the full candidate
  version appears in About.
- Browser editor/installer and installation/sample tools recognise Inputsignwave
  alongside legacy names. Audio tools accept both USB names; performance-tool
  defaults use Inputsignwave and retain `--port` overrides.
- Source notices scope our changes, including adapted test harnesses, to
  **By: Inputsignwave**. Original copyright/SPDX notices remain. CREDITS.md
  documents Performance 8 and branding scope.
- Candidate: `1.4.1-inputsignwave.8-dev2`; public package filename
  `inputsignwave-1.4.1-performance8-dev2.fwsc`.

Bootloader/update USB name and numeric package identity, MIDI protocol IDs,
FBK7 storage, flash layout, engines and DSP are unchanged. GitHub repository and
branch names, MELODEE build switches and legacy build output paths remain
stable. Archived Performance 6 and historical handoffs are not rewritten.

## Validation completed

Compiled GitHub commit: `2b68fd3d0badbe38c7c8d9abeb90d30c1a7256ad`.
Equivalent local implementation commit: `9d6a599a4616f7638a6ca87f8ad1e2768698bb95`.
Both repository trees: `30114178527937649df92df563446fec57d8358a`.

[CI run 39625591513](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39625591513),
job `119309846075`, passed on that compiled commit:

- Native dual-core resident-audio build and resource/call-graph checks passed.
  Audio arena remains 49,600 B against a 49,152 B minimum; ordinary SRAM tail
  remains 544 B. Image size: 568,604 B.
- Strict and AddressSanitizer/UndefinedBehaviorSanitizer checks passed, including
  real sequencer/storage integration and navigation/identity contracts.
- Full Linux host/web suite passed in **314 s**; complete local suite passed in
  **170 s**. All **337** sound renders unchanged; zero health, voice/routing,
  CPU-budget failures or crashes.
- Actual USB driver and descriptor tests verify the complete 28-byte USB string
  descriptor decodes to Inputsignwave in all three app layouts. New installer
  matching accepts Inputsignwave and legacy names while excluding unrelated gear.
  Browser editor/installer tests pass with the new name and legacy support.
- UI: 261 scenes across eight palettes plus page/value sweeps, zero layout lint
  findings and zero MONO violations. Separate About/credits renders use the full
  real candidate version and have zero lint findings; images were inspected.
- Binary inspection confirms the compiled app contains Inputsignwave,
  **By: Inputsignwave**, the full version and upstream credit strings.
- Exact branded `.fwsc` in pinned chip emulator v0.2.0: 3000 emulated ms, 47 LCD
  frames, 123,904 stereo audio frames, peak 5,481 and zero clipped samples.
  CPU1 is offline and eight unsupported MMIO addresses remain; this is bounded
  serial-fallback smoke evidence, not hardware/dual-core acceptance.
- Existing upstream copyright and SPDX lines are retained in all changed
  source/test/tool files. Archived Performance 6 has no changes.

Firmware SHA256: `7de48a56e4153b7537cc0796f0392b21af5a3ebb79d3c17690d56c027e3c1c75`.
Application SHA256: `8505cd80a47fbe1ce1c80bd4c38469262241719b69dc5f729cafce3c5948e65b`.
Target artifact `12355899441`, ZIP SHA256:
`485668a4837ecc064a50db0aaea198445adb78389ccf2f4bf2d590a47bb693f5`.
The public Inputsignwave package filename is byte-identical to the tested native
release package. Final documentation changes do not alter firmware/tool/test
sources from this compiled commit. Pre-branding evidence remains separately
recorded in STUDIO8_VALIDATION.md. No physical device was flashed or main merged.

Remaining physical checks: About and USB names, editor/MIDI/audio connection,
new navigation, project/palette recall and normal sustained operation. Retain
the working Performance 7 package and project backup for rollback.
