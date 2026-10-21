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

## Validation

Fresh full local/Linux host/web, native target/resource, sanitizer and
actual-version About rendering checks are in progress. The `studio-tess-8`
workflow builds this candidate. Pre-branding evidence is recorded separately in
STUDIO8_VALIDATION.md; it must not be claimed for newly compiled bytes.
No device has been flashed by this update.
