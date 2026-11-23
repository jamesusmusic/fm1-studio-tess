# Performance 12 Cassette Dev2 — new UI only

By: Inputsignwave

This supersedes Dev1's selectable older themes. The production firmware contains
only the approved cassette-futurist NIGHT design and its instrument colours.
The old HOME Stage rows and their caches are excluded from the target build;
the seven alternative colour palettes and COLOR menu selector are removed.

Shared functionality remains: mixer geometry, HOME's chord view and its knob
overlay, both fonts, sound controls, menus, recording and MIDI. No DSP, effects,
presets, generator or project-format changes are introduced.

All eight former tagged theme IDs remain valid in settings and backups. Import
maps their theme to NIGHT, preserving other settings, calibration and favourites.
New exports use NIGHT's existing tagged ID (96). Invalid IDs stay invalid.
The editor advertises one palette; cached requests for other choices are rejected.

Historical themes and Stage renderers remain available only to the existing
host-reference regression fixtures. They are not included in the installable
firmware. This preserves comparison history without carrying the old UI on-device.
The native binary and memory map must confirm the removed renderer is absent.

Required validation: production migration tests for all old tagged IDs and
unrelated preferences; six-engine/two-font new UI coverage; full registered
host/web/audio suite; exact native CPU/SRAM/flash guards; exact-package boot,
audio and LCD emulator checks. Retain Dev1 as rollback and measure actual savings.
