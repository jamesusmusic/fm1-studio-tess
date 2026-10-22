# Inputsignwave Performance 9: wavetable prototype

By: Inputsignwave

22 October 2026. Development candidate; native target build and resource measurements completed.
Final Linux validation is in progress. Listening and physical hardware acceptance remain pending.
Prophet, FM6 and 808/909 remain. SID, CZ, SID kits, ROOM and REVERB remain retired.
Replacement reverb and resonator are separate next stages.

## Sound and controls

Select WAVETABLE in PRESETS or Sound Designer. Its stable engine ID is **21**;
20 remains the Prophet user-preset namespace. Existing IDs and disk layouts stay fixed.
Six factory sounds: INIT WAVE, HOLLOW KEYS, GLASS DRIFT, FOLDED BASS, SLOW SCAN,
BRIGHT PLUCK. Normal ADSR, LFO, modulation matrix, poly/mono/legato/unison and
recording paths apply. This engine generates sound, so it needs no sample imports.

| Page | Control | Behaviour |
| --- | --- | --- |
| WAVE SCAN | WAVE | Morph between original sine, hollow, glass and bright spectra. |
| WAVE SCAN | FOLD | Morph between three precomputed triangle-folded spectra. This is not live nonlinear wavefolding of incoming audio. |
| WAVE SCAN | RATE | Scan oscillator rate, using the existing LFO rate scale. |
| WAVE SCAN | DEPTH | Scan depth; zero disables scanning's audible effect. |
| WAVE TONE | TONE | Two-pole low-pass, transparent at full setting; responds to filter modulation. |
| WAVE TONE | SUB | Blend a sine one octave below, up to half the source. |
| WAVE TONE | SEMI | -24 to +24 semitones. |
| WAVE TONE | FINE | -50 to +50 cents. |

Front knobs map to WAVE, FOLD, TONE, DEPTH. MIDI CC74 adjusts TONE and CC71 FOLD.
Factory stars use a tagged four-byte area previously unused in the retired GRAIN
favorites row, preserving the old stars and FX key map. User presets retain the
normal packed format with explicit validation of ID21. Older firmware cannot
play the new engine and should not be used to resave these new sounds.

## Implementation and limits

`eng_wavetable.c` contains oscillator/control processing and six factory presets.
`gen_wavetable.py` reproducibly generates `wavetable_bank.h` from original mathematical
spectra: no third-party waveform assets or DSP were imported. Four frames, three
fold spectra, four harmonic limits (15/7/3/1) and 129 int16 points use **12,384 bytes**.
The bank lives in flash and is copied once to managed audio RAM when needed.
All tracks share it; per-voice phases and filter/ramp state fit existing voice fields.
The audio owner releases it when no wavetable voices remain. Allocation failure
produces silence and increments diagnostics, without overwriting other resources.

Pitch-dependent table crossfades reduce harmonics before Nyquist; linear phase,
wave, fold and band interpolation plus block control ramps reduce discontinuities.
This is not an absolute alias-free claim: table interpolation images, fast modulation
and pitch-jump transients remain subjects for listening. No hardware CPU percentage
can be inferred from host elapsed time or emulator throughput.

Host checks currently cover all 32,768 note/control corners, zero/short blocks and
output canaries; periodicity, DC, harmonic limits; shared allocation/idle release;
exhausted-arena failure; signed project/user-preset round trips and favorite settings;
six deterministic chord presets with complete note release. Final hardening also checks
measured 440/880/220 Hz pitches, +/-50-cent tuning and A4=432 Hz, MIDI CC71/74,
startup-template recall, poly/mono/legato/unison releases, 40 held-note engine
switches without leaked state, and bit-identical zero-depth scan at minimum and
maximum rates. Coherent full-fold oscillator
measurements include interpolation images: at 1,100.89 Hz unwanted energy is -59.93 dB,
and at 4,998.39 Hz -76.61 dB relative to intended harmonic energy. Compared with a
naive live fold of the same source, those are improvements of 47.51 and 64.57 dB.
They are two static fixtures, not a full-band musical quality verdict.

Four-track concurrent playback is expanded from 256 retained assignments to 625
ordered assignments including wavetable; serial and concurrent PCM must match.
Historical reference goldens run with the new engine disabled. Production retained
PCM comparisons remain independent. The production UI also checks all engine pages and control values across eight palettes.

## Exact target measurements

| Quantity | Removal baseline | Wave1 |
| --- | ---: | ---: |
| App including unchanged 3,808-byte boot stub | 533,616 B | 548,328 B |
| App body | 529,808 B | 544,520 B |
| Free app-slot flash | 43,632 B | **28,920 B** |
| Main contiguous working arena | 64,640 B | **62,912 B** |
| Data / low / retained working banks | 3,120 / 4,260 / 784 B | unchanged |
| Total disjoint working banks, cache excluded | 72,804 B | 71,076 B |

The linked addition costs **14,712 B of flash** and **1,728 B of working-arena
capacity**, including resident DSP code and metadata. The **12,384 B runtime bank**
is a further allocation *within* these working banks when WT is playing, not
another permanent deduction from the linker arena. Host dynamic state peaks at
**35,220 B** for one WT plus three FM6 tracks. All 625 four-track engine/kit choices
allocate successfully with banks constrained to the measured target capacities;
the host data bank is smaller than the target, and optional cache is disabled.
This does not prove every possible FX/history allocation combination fits.

The target control-flow cost for `wt_render` is **269** (483 instructions, 219 in
loops, four weighted divides, no loop calls), with a dedicated +10% regression
gate. This is a static compiled-code weight, **not cycles or CPU percent**.
FM6 remains 3,560, Prophet's sample kernel 1,065 and track rendering 558.
The complete resident-audio closure includes `wt_note_on` and `wt_render`.
No retired SID/CZ/SID-kit/reverb DSP is linked. Existing cost gates remain intact.

## Code areas touched

| Area | Files and purpose |
| --- | --- |
| New DSP and assets | `eng_wavetable.c`, `wavetable_bank.h`, `tools/gen_wavetable.py`: original source, fold/mip bank, oscillator, scan, tone and factory sounds. |
| Engine identity and lifecycle | `core.h`, `engines.c`, `resources.c`, `mix.c`: append ID21, preserve ID20 namespace, shared allocation and idle release. |
| Controls and UI | `params.c`, `midi_control.c`, `ui_graph.c`, `browse.c`: names, CC macros, abbreviated header, factory categories and visible order. |
| Persistence | `upreset.c`, `favorites.c`: explicitly admit the new generic preset engine and store tagged factory stars without enlarging settings. Project/template wire layouts remain unchanged. |
| Editor | `web/editor.html`: device order/filtering, extended historical mock; connected devices still supply their descriptors. |
| Validation/build | `tools/build.py`, `tests/wavetable_test.c`, `tests/wavetable_spectral.py`, `tests/wavetable_emulator_fixture.c`, `audio_matrix_test.c`, `target_budget.py`, `run_retirement_tests.sh`, reference fixtures and CI workflow. |

## Emulator evidence and practical limits

The final Wave1 `.fwsc` boots and plays in pinned fm1-emu v0.2.0 for 1,800 emulated
milliseconds: **44 LCD frames**, **70,912 stereo frames** at 44.1 kHz, peak **5,257**,
zero clipped samples. This exercises package startup and the retained default sound.
The decoded app from that exact package also boots a synthetic saved WAVETABLE /
GLASS DRIFT project at entry `0x02001000`, with FM6, Prophet and 808 on other tracks:
**31 LCD frames**, **72,320 stereo frames**, peak **529**, zero clipped samples.
The snapshot confirms the selected WT sound. Its short keyboard hold samples only
part of the slow attack. The package/stub path and the app-with-synthetic-NOR path
are separate tests; the latter bypasses the package stub. Both terminate at the
requested interpreter step limit, without an undecoded-instruction halt.

Eight MMIO addresses remain unsupported and CPU1 is offline in this pinned emulator.
Consequently these runs prove executable boot/audio/UI paths under emulation;
they do not establish physical dual-core timing, stack high-water use, first-note
bank-copy latency or actual device CPU headroom. Source, linker map/disassembly,
residency manifest, host logs, waveform statistics and emulator snapshots accompany
the local package. No hardware was flashed.

## Next decisions

1. Listen to the six factory auditions, particularly the transition from hollow to
   glass/bright and the maximum fold setting; refine character rather than add controls.
2. On-device acceptance: compare loudness, rapid wave/fold changes, held chords,
   mono/legato/unison, project/preset recall, engine switches and live CPU load.
3. Prototype the new stereo reverb in one shared send slot: dense plate/ambient tail,
   controllable wobble, tape colour and width, with freeze and performance controls
   developed after the core tail is stable. Start with a bounded buffer and measure
   it alongside WT + FM6 and recording; the 28,920 B flash margin is shared with all
   future work. ROOM and REVERB IDs stay retired.
4. Reassess a focused resonator only after the reverb's measured resource cost.

## Other open-source engines reviewed

The requested pre-finalization review is recorded in
[FM1_ENGINE_RESEARCH_2026-10-22.md](FM1_ENGINE_RESEARCH_2026-10-22.md).
It pins available engine sources, separates binary SDKs/installers from DSP source,
and records why this build retains our current WT prototype rather than adding
unmeasured ports. Physical/modal models and replacement reverb remain subsequent
work with their own budget and listening gates.
