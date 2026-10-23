# Inputsignwave Performance 10 RC4

By: Inputsignwave

## Track reverb behavior

REVERB (ID 10) is selectable in any of each track's three FX slots. The entry
represents a send to the one shared stereo tank after the track insert chain,
level, pan and solo processing. Its amount is labelled SEND. Each track keeps
its own amount; no extra reverb tank is allocated for a track entry.

Selection reuses a configured shared REVERB or creates it in the first empty
shared slot. It never replaces an occupied effect. SHARED FX FULL rejects the
selection without changing the previous track effect. Existing CHAIN/SPLIT
routing is retained: CHAIN feeds the shared chain, SPLIT feeds the reverb bus.
An explicit send and a reverb marker combine by maximum rather than summing.
Repeated REVERB entries on one track also use the maximum amount.

The track's detail page says REV SHARED. SIZE and DECAY edit the common tank,
so their changes affect every sending track. The reverb's tone, wet filters,
movement, wobble, Swell, Hold and slot LFO remain shared controls. Their existing
shared MIDI/editor IDs are retained; a track marker's inactive local RATE/CHAR
fields do not become independent reverb settings. The SEND amount retains its
normal track parameter, MIDI/editor identity and motion recording path.

Selecting OFF removes that entry's send. Explicit sends and the existing shared
tail remain. Track entries restored without a tank can bind an empty shared
slot before the audio worker is prepared; if all slots are occupied, existing
effects remain intact, the track header says REV NO BUS and common controls
are disabled. A replaced sounding insert follows its normal fade to dry.
Retired ROOM/REVERB IDs 7/8 remain hidden bypass aliases. No storage layout or
canonical preset/effect identity changed. All RC2/RC3 fixes and sounds remain.

## Source and exact package

Native source commit: `7c2ce0ac7b6eb9f70ada069550c6da20a44acfeb`.
Tree: `e304e531ba75fcb034851a2edc27a3a18c2fbff0`.
Branch: `INPUTSIGNWAVE-GEN`.
[Successful native and host build](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39951739830).

Artifact ID 12500338719, archive SHA256:
`85817b31e02545386403e35ebb10cfea75e16088067417c56e940efb1ecbd05e`.
Firmware `inputsignwave-1.4.1-performance10-rc4.fwsc`, identity FM-1_91041,
SHA256 `fd7c6b57a49094a8c03bddeed384f062277ff949683278045887b38c066f0ec9`.
The packaged application was compared byte-for-byte with the linked native
binary before emulator execution. The source archive verifies every one of
its 726 files against Git blob hashes from the native source commit.

## Code areas

- `firmware/src/params.c`: shared tank lookup/binding; track SEND label;
  SIZE/DECAY descriptors point at common bus parameters.
- `firmware/src/ui_input.c`: permit track selection, protect occupied buses,
  preserve shared duplicate-instance checks, guard track-only pointer handling.
- `firmware/src/fx.c`: virtual track entries bypass per-track tank allocation,
  preserve prior insert fade, combine sends without duplication, bind restored
  entries before shared worker preparation.
- `firmware/src/mix.c`: route virtual amounts through existing split send
  buffers; existing level/pan/solo mixing and worker ownership are retained.
- `firmware/src/ui_draw.c` and `ui_pages.c`: shared/no-bus context, correct
  common decay and tank allocation-failure preview state.
- `tests/reverb_track_test.c`, registered in `run_retirement_tests.sh`:
  knob/picker selection, routing, ownership, fade and save/recall contracts.
  `tests/ui_render.c` additionally sweeps valid and unavailable track reverb
  pages with both wavetable engines in every palette.

## Measurements

Application body **573,168 / 573,440 B**, **272 B spare**. Including the unchanged
3,808-byte boot stub the linked application is 576,976 B. This patch costs
704 B of flash versus RC3. Future feature additions need further space work.

Main audio arena **51,232 B**, above the unchanged 49,152-byte gate. Disjoint
managed tails are data 2,656 B, low 4,228 B and retained 784 B, total 58,900 B
excluding optional cache. These are working-bank capacities, not free RAM or
one contiguous buffer. Ordinary RAM through BSS remains 95,648 B. Static target
CPU gates pass without changing budgets; no physical CPU percentage or deadline
margin is inferred. The tank algorithm, delay capacity and sample rate did not
change in RC4.

## Validation

Linux native compile/link, foundational sanitizers and full registered host/web
checks passed: **666 seconds** for the complete suite. All **349** historical
sound renders are unchanged; zero health, voice/routing, CPU-budget or crash
failures. The production matrix checks **2,401** ordered engine assignments
with actual concurrent worker/serial PCM equality.

The new ASan/UBSan test exercises all four tracks and three FX positions,
knob and full-list selection, independent amounts, common controls, existing
bus reuse, full-bus rejection, unavailable-page controls, CHAIN/SPLIT routing
at all three bus positions, duplicate-send prevention, live mixed audio and
solo, exactly one tank, prior insert fade, project wire and template recall.
The complete UI sweep reports zero layout findings, two allowed free-text
ellipses and zero off-gray MONO pixels. No golden or budget expectation changed.

An initial local run used the default missing SDK path, causing four package/
recovery test failures and no firmware signal failures. The final local run
uses the pinned local SDK and bundled Python with Pillow and passes the
complete registered suite in **305 seconds**, including the exact RC4 target
disassembly gates. Package and recovery checks pass with no USB access.
Optional external DaisySP/Dexed comparison suites were not run because those
reference checkouts were not supplied; the registered native engine, envelope,
PCM and factory-identity tests passed.

The exact installable application ran in **fm1-emu v0.2.0** for **6,000 emulated
ms** using synthetic NOR only. Timed input opened the track FX rack, selected
REVERB from CHORUS, entered the detail page, started PLAY, held a note, raised
SEND and changed SIZE. The captured LCD shows **REV SHARED**, SEND 45%, SIZE
62% and DECAY 50%. The run produced **249,216 stereo frames at 44.1 kHz**, peak
490, 148,426 nonzero channel samples, zero clipped samples, 104 LCD frames and
no crash screen/invalid-instruction failure. Eight known unsupported MMIO
addresses remain. The emulator uses the serial worker fallback; CPU1 contracts
are covered by the concurrent host matrix and fault/timeout tests. Its ISR
instruction shares are diagnostics, not physical FM-1 CPU percentages.

Physical sound, playing feel and deadline margin remain device acceptance
checks. No connected FM-1 was flashed during this work. The existing NIGHT
palette, Prophet order, original retained sounds, independent warm Drive and
approved synthesis/reverb features remain in this candidate.
