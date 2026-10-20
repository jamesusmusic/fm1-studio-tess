# Studio Tess Performance 7 validation and developer notes

By: Inputsignwave

Candidate: `1.4.1-studio-tess.7-dev1`. Date: 20 October 2026.
Validated compiled commit: `8057dbd53a28651f27d41ae22fc11454185fadf2`.
[Complete CI run: passed](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39449681801).
The full host/web suite passed in 434 seconds on Linux and 164 seconds locally.
Strict builds and AddressSanitizer/UndefinedBehaviorSanitizer checks passed.

The installed Performance 6 build has no active audio fault report. This patch
adds the approved generator, rhythmic independence and coordinated drum work.
It is a development candidate requiring physical acceptance; no device was flashed.

## Developer summary

The patch retains the complete studio synth profile and old generator defaults.
It selectively ports Grids, BOUNCE, AWAKE and POLY from Tess v1.3.3, adds an
original eight-lane KIT development mode, and connects local timing, seed controls,
capture and versioned project storage to the existing sequencer. Timing runs from
the existing internal/MIDI clock rather than a second tempo source.

The principal regression risks are sequencing at boundaries, note ownership,
backward project import and SRAM headroom. Tests cover these directly. The new
storage format fits the existing A/B allocation and object identities; it does
not change boot or flash sector placement. Saving in this build upgrades a project
to FBK7, which older firmware cannot read. Keep a pre-update Performance 6 backup.

The target builds with resident audio and all synth engines retained. The checked
working arena is 49,600 bytes against its unchanged 49,152-byte minimum, leaving
448 bytes above that guard. Four largest engine states total 30,448 bytes. This
is tight static headroom, not a claim of measured hardware CPU capacity. Target
link, resident-call and disassembly checks are mandatory for later edits.

Near-term roadmap: physical acceptance, measured performance/UI polish, then the
planned sound/timbre morphing and modulation work (BRANCH/LOGIC, SLOTH/OCHD, MPE).
The donor's engine-removal profile and MACRO replacement are excluded.

## Changes and code ownership

| Area | Files and main entry points | Behavior and hardening |
| --- | --- | --- |
| Generator ports | `gen.c`, `gen.h`, `gen_grids.h`; `studio7_emit`, `studio7_roll` in `studio7.c` | Pinned Tess algorithms; bounded seed selection; editable AWAKE arrays and POLY degrees. BOUNCE may emit three pitches. Grids maps roles to physical kit lanes. Upstream attribution retained. |
| Timing | `studio7_clock.h`: `s7_clock_start/take/realign`; `studio7.c`: `studio7_tick`, `studio7_meter`, `studio7_after_evolve`; `seq.c`, `tessera.c` | Exact rational carry, grouped meters, ratios, phase, drift/freeze and realignment. Event processing is bounded to 256 events per interrupt. Inactive local settings bypass rate/phase/drift. FORM boundaries avoid duplicate or prematurely phased notes. |
| Rhythmic conditions | `studio7_condition`, `studio7_bar_at`, `studio7_allow`; `tessera.c` | Conditions can reference local bars, master bars, FORM sections or pattern cycles. Conditions still work with LOCAL off; EVOLVE uses musical boundaries. |
| Drum development | `studio7_kit.h`: `s7_kit_generate`; `studio7_kit_plan`, `studio7_drum_tick/stop` | Eight coordinated roles, lane densities, variation/fill, accent/ghost velocity, hat exclusion and bounded human timing. Sixteen pending events per track provide early-hit lookahead. Existing anchors retain source hits and actual accent velocity 127. REST/TIE stale bits cannot become anchors. |
| Lifetimes and recording | `tess_features.c`, `studio7_note`, `studio7_tick`; `seq.c` | Four melodic owners and eight drum key owners preserve releases. BOUNCE HIT releases the prior batch once, then adds all pitches. Fractional capture retains performed timing; STEP replay bypasses the generator clock. Conservative preflight refuses takes exceeding existing recording capacity. |
| UI | `ui_studio7.c`, `params.c`, `ui.c`, `ui_draw.c`, `ui_graph.c`, `ui_tess_features.c` | Existing navigation exposes meter, grouping, ratio, phase, clock reset, seeds, drum controls, AWAKE and POLY edits. LOCAL/HOLD status follows the shared header. Encoder extremes clamp safely. |
| Project state | `project.c`, `studio7_state.h`, `pattern_store.c`, `studio7_step.h` | S7G1 stores 644 bytes, including inactive new-generator settings, AWAKE arrays and anchors. Full validation precedes flash commit. FBK7 uses a lossless 61-bit inactive-step codec to recover 672 bytes in the existing 27,904-byte allocation. |
| Migration and backup | `pattern_store.c`, `tracks.c`, `editor_backup.c` | FBKT/FBKK use their original offsets on import. Save/rename conversion preserves all 32 banks. Track moves/removal preserve corresponding state; full archives and legacy companion restore retain S7 settings. Torn writes leave the committed copy intact. |
| SRAM and target math | `gfx.c`, `studio7_math.h`, `melodee.c`, `tools/build.py` | Secondary canvas changes from 24 to 4 rows, recovering 9,600 bytes. Larger rectangles use the existing DMA-safe primary fallback. Bounded 64/32 division avoids unavailable compiler runtime helpers; explicit freestanding build avoids hosted-header assumptions. Link map emitted for inspection. |
| FM6 baseline cost | `eng_fm6.c` | A neutral block skips inactive correction bookkeeping while retaining the same sample math. All 337 golden sound hashes remain unchanged. |
| Build reproducibility | `tools/get_studio7_sdk.py`, `.github/workflows/studio7-validation.yml` | SHA-256-pinned vendor packaging and recovery dependencies; native Linux target build, full host/web suite and sanitizers. Recovery helper included after CI exposed its omission from the minimal SDK download. |

## Verification evidence

Final CI result and immutable artifact identity are recorded in the delivered
`BUILD_ID.json` and `validation/CI_SUMMARY.md`. Corresponding logs accompany the
candidate. Source changes after the runtime validation are limited to this report
and status/roadmap documentation.

- All 337 golden renders unchanged; no removed renders, health failures,
  voice/routing failures, CPU entries over budget or crashes.
- Original four generator note/state baseline unchanged when new features are off.
- Five new modes run repeatably through the real sequencer and release on STOP.
- Grouped 5/8 at 3:2 retains the master grid; freeze retains ratio. Fractional
  phase capture replays through STEP. Phased FORM boundaries emit once. A 5:4
  external-clock test follows MIDI ticks despite changing tempo estimates.
- Three BOUNCE HIT keys retain independent owners; eight anchored lanes capture
  to a different bank without changing their source. Anchor accents remain 127.
  Alignment retains existing drum release owners. LOCAL-off edits remain bypassed.
- Over-capacity capture is refused before recording; malformed/extreme knob deltas
  retain valid state. Full saves, track removal and add/reset retain correct ownership.
- Step codec: all 6,816 hit/gate states and 200,000 complete random round trips.
  Clock: four million rational events. Kit: 131,072 repeatability/anchor trials.
  Division: one million full-width comparisons plus edge cases. State: 644-byte
  round trips, all truncations and 1,868 invalid mutations rejected.
- Frozen Performance 6 fixture (27,904 bytes) imports and converts with all 32 banks'
  decoded bytes unchanged. Complete archive and legacy companion restore preserve
  S7 state; malformed extension/torn saves do not replace committed settings.
- AddressSanitizer/UndefinedBehaviorSanitizer cover pure foundations and real
  integration/storage paths. UI rendering/lint and LCD DMA ownership checks pass.
- Host serial/concurrent parity: 16,384 blocks with four generators, 512 live edits,
  harmony, two FM6/two Prophet tracks, FX and one-pass recording; PCM and all 694
  MIDI packets/timestamps match. Audio matrix tests 1,296 ordered engine assignments.

## Chip emulator evidence and limits

Official `joakimeriksson/fm1-emu` v0.2.0, macOS arm64, ran the packaged target and
its unpacked XIP application. The synthetic NOR fixture contains no user data:
FM6/AWAKE in 5/8 grouped 3+2 at 3:2; CZ/POLY in 7/8 grouped 2+2+3 at 5:4;
Prophet/BOUNCE with three balls; DRUM/KIT with anchors, fill and human timing.
Fixture source is `tests/studio7_emulator_fixture.c`.

Each run completed its requested 3,000 emulated milliseconds and exited normally.
The package drew 45 LCD frames and produced 2.810 seconds of stereo audio
(peak 5,481; RMS 1,409.22; zero clipped samples). The four-track project drew 148
frames and produced 2.839 seconds (peak 3,759; RMS 559.65; zero clipped samples).
These are short smoke checks, not a subjective listening or endurance certificate.

The emulator reports eight unsupported MMIO addresses. The inspected
`audio_worker_online` flag remains zero, so this exercises the serial fallback;
it does not validate actual CPU1 startup or physical dual-core timing. Host
concurrent parity is separate evidence. No hardware startup changes were inferred
from this emulator limitation. WAVs, screenshots, logs and audio statistics are
included under `validation/`.

Reproduce after building the target (create `work/` in the run directory):

```sh
FM1_FWSC_XIP=studio7.xip /path/to/fm1-emu \
  --fwsc /path/to/candidate.fwsc --ms 3000 \
  --keys '1100-1900:n7,2200:PLAY' --wav package.wav --snap 2400
# Build the synthetic NOR fixture from development/:
cc -O2 -ffp-contract=off -w -Ibuild/gen -Ifirmware/src \
  tests/studio7_emulator_fixture.c -lm -o build/host/studio7_emulator_fixture
build/host/studio7_emulator_fixture /path/to/demo.nor
# From a run directory containing work/:
/path/to/fm1-emu --bin /path/to/studio7.xip --entry 0x02001000 \
  --raw-flash /path/to/demo.nor --ms 3000 --keys 1200:PLAY \
  --wav demo.wav --snap 2000
```

## Physical acceptance still required

Before adopting this as the performance build, check repeated cold boots on USB
and battery, CPU1 operation, real MIDI/USB synchronization, maximum practical
voice/FX/kit load, capture/playback, saved-project power cycling and long sessions.
Keep a Performance 6 project backup and firmware available. Software validation
cannot establish those device-specific properties. Main remains unchanged; this
candidate is on the development branch/draft pull request.
