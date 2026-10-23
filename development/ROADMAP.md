# Inputsignwave FM-1 roadmap

## Firmware name and attribution — 21 October 2026

By: Inputsignwave

The firmware is now named **Inputsignwave**. Add explicit on-device attribution,
application USB/editor/console identity and compatible host discovery; preserve
upstream notices, protocol/storage/update identities and archived baselines.
See [branding and validation](INPUTSIGNWAVE_BRANDING.md).

## Performance 10 RC4 track reverb — 23 October 2026

By: Inputsignwave

User requested REVERB directly selectable on every track. Track entries now
represent independent sends to the one shared tank, with common SIZE/DECAY
accessible on the track page and a REV SHARED header. Reuse a configured tank
or an empty shared slot; preserve occupied effects and CHAIN/SPLIT routing.
Track sends are post-insert/level/pan/solo. Duplicate markers and explicit sends
combine by maximum, avoiding double feeding. Saved identities/layout are retained.
RC4 native build, Linux full suite (666 s), corrected local full suite (305 s),
ASan/UBSan routing/ownership tests, all-palette UI lint and a 6,000 ms chip
emulator run passed. The package is ready for device audition, with 272 B flash
spare and a 51,232 B main audio arena.
See [RC4 validation](docs/PERFORMANCE10_RC4_VALIDATION.md).

## Performance 10 RC2 hardware fixes — 23 October 2026

By: Inputsignwave

RC1 hardware testing found a repeatable red-screen fault while playing WAVE
LAYERS / Phase Prism with Warm Drive at about 30% mix. Fault vector 1, PC
`020233A0`, EMU `00000000`, DBG `00000010`, return `0202375E` map to
`text_fit` called by `cv_free_text`. A new sanitizer test reproduces a null hint:
the FX preview table had 12 entries allocated but only ten initialized strings.
Both Drive and Plate were affected. RC2 populates all hints and asserts table
coverage at compile time. DSP and saved effect IDs are unchanged.

Retired Room/Reverb entries are now empty aliases omitted by the shared picker
mapping; old loaded IDs remain dry bypass and display OFF. The warm drive label
is DRIVE to use the normal picker font, and its output knob is OUT without
clipping. New tests render complete preview panels during note playback, rather
than only sweeping columns; original WT and WAVE LAYERS get full Drive/Plate and
picker sweeps in every palette. RC2 native build and full offline validation are complete: local suite 247 s,
Linux suite 426 s, unchanged 349 golden renders, zero layout lint, sanitizer
reproduction/fix and native-binary hint-table verification. The install package
passes checks; its app boots and plays Phase Prism + Drive in the emulator.
Flash spare 1,008 B; main audio arena unchanged at 51,520 B. RC1 should be
superseded by RC2. Hardware re-test of the reported sequence remains. See
[RC2 fixes and validation](docs/PERFORMANCE10_RC2_HARDWARE_FIXES.md).

## Performance 10 integration — 23 October 2026

By: Inputsignwave

Combined candidate source is implemented for validation: NIGHT and the 12
additional Prophet patches; a new ID-22 dual-layer engine alongside unchanged
ID-21 sounds; eight compact spectral bank families, independent layer ADSRs,
filters/scans and clocked blending/morph; 25 appended FM6 voices with INIT retained
at index 24; ID-23 FM drum kits with immutable per-hit patches; shared Rings-derived
plate with size, separate tone/cuts, movement, wobble, Swell and lossless Hold;
warm drive and independent per-slot LFO settings. FBK8 saves 168 B losslessly and
adds SFX1 controls in the unchanged atomic allocation. TPLM migrates older templates.

Complete combined RC1 target build and offline checks are finished. Exact native
source: `7a74b612485b335a94203bff6a28207178be4feb`. Application body 572,352 /
573,440 B (1,088 B spare); main audio arena 51,520 B, excluding disjoint tails.
The final full local suite passed in 238 s: 349 unchanged golden renders, zero
health/routing/CPU failures, 2,401 production concurrency combinations and zero
UI lint findings. Linux CI also passed native compile, sanitizers and the full suite (431 s).
Exact-package boot and a combined song emulator test pass.
See [final validation and limits](docs/PERFORMANCE10_FINAL_VALIDATION_2026-10-23.md).

Next acceptance work is physical FM-1 audition/timing: note edges and rich WT
presets, mixed polyphony with plate/drive/modulation, long Hold/Swell changes,
record/reload and control feel. New feature growth now requires flash optimization;
only 1,088 B remains. Optional record undo is deliberately lower priority than
sound/recording and can be unavailable under heavy allocations. Physical CPU
percent, stack high-water and deadline margins are not inferred from host tests.
The later roadmap entries below remain the backlog; this candidate does not
silently incorporate additional unapproved engine or Pro-800 conversions.

## Performance 9 removal baseline — built and offline validated

By: Inputsignwave

SID, CZ, SID drums, ROOM and REVERB are removed from the production DSP.
Prophet, FM6, 808/909 and other FX remain. Flash recovered: **34,988 B**;
audio-working SRAM recovered: **20,240 B** across disjoint banks. Main arena:
**64,640 B**. No replacement engines or reverb are added yet. Production PCM,
legacy storage/bypass, sanitizer and concurrency tests pass; the complete local
suite passed in 210 s; Linux CI passed in 360 s. The exact-package emulator
smoke and production UI checks also pass. No hardware was flashed.
See [measurements and validation](docs/PERFORMANCE9_REMOVAL_REPORT.md).

## Next update — engine choices and reverb auditions (21 October 2026)

By: Inputsignwave

**22 October audition decisions for the restarted update:** accept all eight richer
WT concept directions; sloopDX melodic **808 SUB only**; DX7 percussion and all
previewed Eno/more-synth-colour sounds. Reject the other sloopDX melodic imports
and all sloopDX drum sets; keep our existing 808/909. Package accepted DX7
percussion **and existing FM6 percussion as playable eight-lane FM kits**, while
retaining the individual sounds. These are pending integrations, not shipped
features. See [approved sound direction and actual build status](docs/APPROVED_SOUND_DIRECTION_2026-10-22.md).

**Preset preservation clarification, 22 October:** all approved new sound presets
are **additions**. Retain existing FM6/Prophet presets, native 808/909 kits and WT
preset identities/sounds alongside the new banks/kits. Do not replace old tones
or repurpose their IDs to make room; validate old-project recall and account for
retained assets in the combined budget. This does not undo the explicitly
approved SID/CZ/SID-kit and ROOM/REVERB retirement. Pro-800 conversions remain
separate work. See the additive rule in the approved-sound document above.

**Additional confirmed scope, 22 October:** return to **NIGHT** UI; two wavetable
layers with independent banks/parameters; frame scanning, cross-table A/B blend
and sound-parameter morph with beat sync; warm drive/distortion; FX LFO modulation
with SYNC and a time-division dial; **a new reverb implementation in this build**.
Each WT layer also requires its own **amplitude ADSR and filter** (cutoff/resonance,
with envelope amount); the reverb requires **separate tone and wet filter controls**
(warm/bright shaping and low/high cuts). These are distinct from scan envelopes
and internal tail damping. Preserve the dry signal and smooth automated changes.
Profile these together with the accepted FM6 sounds and FM kits. Individual feature
feasibility is not a combined resource-fit claim. See
[dual-wavetable and FX scope](docs/DUAL_WAVETABLE_AND_FX_SCOPE_2026-10-22.md).

**22 October replacement reverb prototype:** built a standalone fixed-point C
adaptation of Mutable Instruments Rings' modulated Griesinger/Dattorro topology
(Emilie Gillet, MIT; adaptations By: Inputsignwave). Separate tone, wet low/high
cuts, damping, decay, width, movement and predelay controls. 22.05 kHz delay storage
24,274 B; smaller 11.025 kHz variant 12,136 B, before state/resampler/allocator.
Both pass sanitizer/stability tests; the complete existing host/web suite also
passes. Fourteen effect renders / 35 listening WAVs
cover three settings, equal-send/RMS-matched/wet-only references and isolated
controls, with zero audition saturation/clipping. Ready to listen and begin
integration; **not yet connected to shipping firmware, saved controls or clock**.
See [prototype and audition report](docs/REVERB_PROTOTYPE_AUDITION_2026-10-22.md).

**22 October reverb listening approval:** user likes the first plate audition;
retain it as the replacement-reverb direction. Add independent **SIZE** and
optional zero/off **tape WOBBLE**. Include **SWELL** (wet rise) and **INFINITE/HOLD**
(closed-input held tail, dry keyboard remains playable) in the next prototype
plan. These additions are not implemented yet. Size must remain distinct from
decay; validate stable hold and its interaction with size/wobble rather than
claiming that maximum feedback already provides infinity. Core rate and combined
hardware fit remain open. See [controls and performance behaviour](docs/REVERB_PERFORMANCE_CONTROLS_2026-10-22.md).

**22 October Prophet sound auditions:** prepared 12 new native artist-/classic-
inspired designs plus eight curated, byte-exact existing Sequential references.
20 dry previews and eight approved-reverb examples use the actual Prophet DSP;
no artist-authenticated dump or new firmware integration is claimed. Reviewed the
user's five Desktop Pro-800 collections (23 version/bank SysEx files) privately;
conversion needs calibration and is not represented by these new patches. All
20 native audition scenarios pass sanitizers; 28 WAV files are unclipped; the
complete existing host/web suite passes in 188 seconds. Keep
paid data and the private inventory out of public Git. **All 12 new Prophet
patches are now user-approved additions**; the eight existing references remain
unchanged and are not duplicated. See [Prophet auditions and provenance](docs/PROPHET_SOUND_AUDITIONS_2026-10-22.md).

**22 October hardware feedback:** Wave1 has audible note-on/release pops and some
presets sound too similar. Prioritize Wave2 correction before reverb: the WT
renderer omitted the shared ADSR/velocity/performance amplitude ramp and reset
filter/control state during sounding retriggers. Restore amplitude application,
preserve sounding state, and test rendered attack, release, velocity, pluck decay
and edge/retrigger continuity. Wave1 acceptance is pending these fixes; do not
describe it as fault-free based on earlier host tests. Wave2 software validation
is complete: local suite 286 s, Linux build/sanitizers/host/web success; native and
saved-WT emulator checks pass. No Wave2 hardware flash or standalone delivery is
assumed. Richer wave banks and FM kit routing remain separate work.

The user reports that Inputsignwave Performance 8 Dev2 installs and works well
on their FM-1. No audio fault is currently reported; this is feature development.

- [x] Keep Prophet, FM6 and DRUM; implement the first wavetable prototype at fresh ID 21.
  Original morphing/fold spectra, scan, tone, sub and tuning; 28,920 B flash headroom,
  62,912 B main working arena, shared WT bank 12,384 B. See the Wave1 report.
- [x] Review available FM-1 engine sources before finalizing Wave1 (22 October).
  See FM1_ENGINE_RESEARCH_2026-10-22.md in development/docs for pinned references.
  Keep this patch focused on WT; new donor engines require separate profiling.
- [x] Finish Wave1 software validation and package the candidate (22 October):
  full local suite 232 s; Linux host/web suite 534 s; native target and sanitizer
  checks; exact-package and saved-WT emulator checks; source/licences and listening
  samples bundled. Hardware CPU headroom is not inferred from these tools.
- [ ] Listen and accept the WT prototype on hardware before final adoption.
  Fold morphs precomputed bandwidth-limited spectra. A focused modal/resonator engine
  follows the replacement reverb only if measured resources permit.
- [x] Recheck Wave1 budgets and other wavetable banks; acquire pinned reverb source
  references and licences (22 October). See
  [space and acquired sources](docs/WAVE1_SPACE_AND_REVERB_SOURCES_2026-10-22.md).
  Research snapshots are separate from firmware; no replacement reverb is in Wave1.
  Full donor tanks do not fit unchanged. First prototype: one shared, reduced-rate
  modulated plate, roughly 16–24 KiB delay target subject to combined-load profiling.
- [x] **Confirmed: remove SID and CZ entirely from the next build**, including
  SID drum kits and their shared SID synthesis code. Retain Prophet, FM6 and
  the non-SID DRUM models. The user approved this on 21 October 2026.
- [x] Preserve retired SID/CZ engine IDs and the SID drum-kit value; never reuse
  or silently remap them. Old retired-engine tracks must remain silent, retain
  their saved data and be identifiable as unavailable. Apply the same explicit
  unavailable behaviour to old SID drum kits rather than substituting 808.
- [x] Replace the default third track's SID sound with a deliberate FM6 default.
  Remove SID/CZ from browsing, Sound Designer, editor capabilities and active
  factory assets/code. Keep historical source/build archives and upstream credits.
- [x] Validate old-project round trips, unavailable-engine handling, default
  tracks, engine selection and retained-engine sound regressions. Compare target
  flash, SRAM and audio-arena usage before allocating the savings to new work.
- [x] **Confirmed: retire both ROOM and REVERB from the next build**, replacing
  them entirely with our own reverb offering. Remove their old DSP and
  selectable entries; reserve both stored FX IDs and retain saved parameters.
  Legacy ROOM/REVERB slots must bypass transparently without allocating a tank,
  muting the dry signal or silently selecting a replacement. Identify the
  unavailable effects in the UI/editor. Give replacement algorithms new IDs.
  Keep the installed Dev2 and archived host renders as comparison references.
- [ ] Audition different reverbs as part of this update: compact plate, modulated
  ambient space, tight room/chamber, and tape-coloured space. Compare against
  archived ROOM and REVERB renders before selecting what ships. Tape character is a
  processing option, not necessarily another reverb algorithm.
- [ ] Produce matched host audio comparisons, then alternate target candidates
  after memory/CPU gates pass. Start with one candidate on the shared send;
  decide whether multiple types can coexist only after profiling.
- [ ] Budget engine and FX changes together, including future repeats/stutter
  buffers. Do not assume engine removal releases equivalent working RAM.

See [the reverb audition plan](docs/REVERB_AUDITION_PLAN.md) for candidates, source references,
resource constraints, code areas and acceptance checks. Research is complete
for this shortlist; implementation, listening comparisons and profiling remain.

## Top priority for 21 October 2026 — navigation and page guide

By: Inputsignwave

User feedback, 20 October: Performance 7 is installed and **working perfectly**
in their testing. Treat it as the working baseline for the navigation work.
This records positive device feedback without claiming that every formal
endurance/acceptance scenario has been exercised.

Tomorrow's first development work is page/menu organisation and a function
reference, ahead of additional feature development.

- [x] **First layout change: conditional generator pages.** Keep PATTERN MODE
  always accessible; show beat/generator-specific parameters only for the mode
  selected on the current track. Hide irrelevant generator pages from scrolling
  and page maps, preserve their saved settings, and retain applicable shared
  controls. See [the agreed visibility rule](docs/GENERATOR_VISIBILITY_DRAFT.md).
- [x] Move the current **hold-OCT+ action sheets into the HOME menu**, as
  requested: sound tools, track actions, pattern/selected-note actions, song
  section/jam actions and shared-FX actions. **Create New Track** is the first
  directly selectable item (renamed from Add track). Move **Sound Designer**
  out of these actions into the **EDIT** menu. Preserve the target
  context, selections, undo and existing action checks. Inventory OCT+ tap
  actions separately so no functions disappear during remapping.
  See [the editable HOME actions draft](docs/HOME_ACTIONS_DRAFT.md).
- [x] **FM6 EDIT: map the physical ALGORITHM knob to FM algorithm selection
  (1–32)** for the selected track, with immediate value/diagram and sound updates.
  Preserve the rest of the patch and existing save behavior. The current encoder
  selects tracks; revise that behavior in the FM6 editing context and document
  track selection in the new map. See [control mapping decisions](docs/CONTROL_MAPPING_DRAFT.md).
- [x] Move **OUTPUT from SEQ to GLO (Global), as the second page after MIXER**.
  Keep MIDI output enable/channel
  scoped to the selected track, show its target clearly, and retain existing
  note-release and project-save behavior. This changes the page location, not
  the scope of its settings.
- [x] **Colour/aesthetic identity:** include a UI colour change in this update
  as the first step toward our own Studio Tess visual identity. Review the
  current palette/track/status swatches before choosing replacements; keep
  contrast, selected/inactive states and track identities clear. See the
  [current colour reference](docs/UI_COLOUR_REFERENCE.md). **LO-FI is the provisional palette choice** (21 October); use it as the starting point for menu mockups and the new visual identity.
- [x] Inventory the current pages, controls and navigation paths, including the
  new Performance 7 pages, and identify crowded or misplaced groups.
- [x] **Layout correction, 21 October:** keep all existing sequencer pages in
  **SEQ**, including beat development, rhythm MORPH/EVOLVE/FORM/TWEAK, meters,
  phase, seeds, AWAKE/POLY, HARMONY and NOTE RANGE. Only OUTPUT leaves SEQ
  for GLO. Sound Designer is the only function being moved into EDIT; do not
  add a duplicate GENERATOR page there. Reduce SEQ scrolling through conditional
  generator visibility instead of relocating its controls.
- [x] Keep core sequencing, pattern/transport and recording controls easy to find;
  group related generator, beat-development and editing controls coherently.
  Preserve quick access during playback and the existing hardware navigation model.
- [x] Create a page guide with a quick navigation map and a reference for every
  page/function: where to find it, what it changes, scope (track/bank/project/global),
  defaults/bypass, applicable modes/engines, and save/recall behavior.
- [x] Explain relationships and precedence with examples: MODE/generator versus
  the source grid; DIV versus meter/grouping/ratio; phase/drift/HOLD/ALIGN/reset;
  FORM/EVOLVE/MORPH and seeds/conditions; anchors/density/fills/hats; harmony/pitch;
  note length/release; capture destination and STEP replay. Link related pages
  and make inactive or mode-dependent controls clear.
- [x] Validate navigation through host tests/screenshots and full regression checks.
  All 337 sound baselines remain unchanged. Complete Linux CI, sanitizers,
  resource gates and exact-package emulator smoke checks passed.
- [ ] Hardware acceptance on the FM-1: menu/track/mode changes, controls,
  MIDI/USB, saved projects/palettes and long sessions. No automated flash.

Performance 8 software validation is complete on `studio-tess-8`:
[build/validation record](STUDIO8_VALIDATION.md) and
[page/function guide](docs/STUDIO8_PAGE_GUIDE.md). CI run 39612840717 passed;
local/full Linux suites completed in 165/298 s. The corrected page families,
conditional pages, HOME actions, FM6 encoder and LO-FI default are included.
The working Performance 7 package remains preserved for rollback.

## Planned phase — character reverb and playable FX

By: Inputsignwave

Added 20 October; expanded 21 October 2026. The navigation build is installed
and reported working. Reverb auditions now accompany the next engine update;
keyboard performance FX remain a subsequent implementation stage.

### Character reverb and stereo space

- [ ] Research and audition a replacement for the retired ROOM and REVERB effects,
  using the Walrus Audio stereo Slöer pedal as a loose creative reference for
  atmosphere and playability. Develop an original design; choose specific
  behaviours by listening and testing rather than assuming the pedal's internals.
- [ ] Explore adjustable wobble/modulation, tape-like movement and coloration,
  diffusion/texture, and stereo width/imaging. Separate useful performance
  controls from deeper editing, with predictable neutral settings.
- [ ] Compare candidate algorithms on short percussion, sustained chords, FM
  transients and evolving textures. Evaluate tail smoothness, stereo motion,
  mono compatibility and useful ranges at both subtle and extreme settings.
- [ ] Measure CPU, delay-buffer memory and worst-case four-track/voice/FX load
  before choosing an implementation. The current resident-audio build has tight
  SRAM headroom; prototype bounded resource use before committing to features.
- [ ] Preserve dry bypass, existing project/FX compatibility, stable gain and
  clean parameter changes. Define tail handling for switching, bypass and STOP.

### Keyboard-triggered performance FX

- [ ] Add an explicit performance-FX keyboard mode for beat repeats, slices and
  stutter, with a clear way to enter/leave it and see the active action.
- [ ] Explore keys selecting repeat divisions, slice positions and stutter
  gestures; define momentary press/release behaviour first, then evaluate latch
  and quick variation controls. Choose mappings during UI design.
- [ ] Decide which actions process buffered audio, which retrigger sequencer/MIDI
  events, and whether both are useful. Document the difference: audio processing
  alone does not create repeated notes at an external MIDI instrument.
- [ ] Define track/bus targeting, buffer capture/freeze and return to normal
  playback. Preserve the source pattern and distinguish temporary performance
  gestures from destructive edits or recorded results.
- [ ] Synchronise rhythmic gestures to internal/external clock and define how
  they follow local meter, ratios and phase. Evaluate quantised starts/releases
  and immediate response as explicit options.
- [ ] Keep transitions click-controlled and output bounded; release reliably on
  key-up, mode/track changes, STOP and clock loss where applicable. Ensure FX
  keys cannot leave stuck MIDI notes or silently alter normal keyboard playing.
- [ ] Define whether/how performances are captured into audio, sequencer events
  or automation, according to the chosen implementation and available capacity.
- [ ] Include FX signal flow, keyboard mappings, scope, timing, capture and
  relationships with other controls in the page/function guide. Validate both
  sound and navigation on the FM-1 before adopting the new FX build.

## Goal
Keep Melodee's synth engines and sound-design facilities, adding Tessera generative sequencing and rhythm morphing. The synth FM-1 may be played from a Seaboard RISE or sequenced by the Polyend Tracker. A second FM-1 with a groovebox focus remains optional. zp12 is excluded.

## Integration candidate
- [x] Fetch upstream Melodee v1.3.4 (last stable published 13 October 2026) and v1.3.5 (14 October 2026).
- [x] Fetch Tess main and its merge-upstream-1.3.1 branch.
- [x] Merge Tess's existing generators into v1.3.5, preserving upstream boot/update changes.
- [x] Build the dual-core target and check RAM/flash limits.
- [x] Complete available host/web verification and record the remaining upstream CZ-1 failure in STUDIO_INTEGRATION.md.
- [ ] Hardware validation: controls, audio, MIDI clock, existing projects, generator saves and restore.

The installed firmware version remains unconfirmed: installation yesterday does not distinguish stable 1.3.1/1.3.2/1.3.3/1.3.4, an RC, or a nightly.

## Generators
Evaluate Turing, Euclid, Markov, rhythm Morph, Evolve, Form and Tweak on this integration. Keep normal step sequencing available. Decide which performance controls should be exposed after hands-on testing.

## Sound morphing
Confirm whether patch/timbre morphing is wanted in addition to Tess's rhythm morphing. Engine-specific parameter interpolation, discrete settings, transition noise and CPU cost require separate design and validation. Not implemented by this merge.

## Later MPE phase for the synth unit
After firmware and engine testing, implement per-note pitch bend, pressure and slide (CC74), voice/channel ownership, appropriate expression mappings, bend ranges, sustain and voice stealing. Verify wired MIDI first. Bluetooth host support and direct Seaboard RISE interoperability are later work; neither is added here.

## Scope
TESS_UPSTREAM_ROADMAP.md is the donor project's own future plan, retained for reference. Its proposal to remove Melodee synth engines is not adopted for this project. No firmware has been flashed or published during this integration.

## Melodee 1.4.1 / Studio Tess update 4
- [x] Merge pinned upstream v1.4.1, retaining the new UI, FX, SID, USB audio and stock supply startup.
- [x] Preserve Turing, Euclid, Markov, Morph, EVOLVE, FORM, TWEAK/FOLLOW, harmony, SONG durations and USB MIDI.
- [x] Keep generator REC one-pass; integrate whole-take undo and the queued-empty-pattern shortcut.
- [x] Store generator settings with project music in the existing A/B allocation; preserve 1024 events and all 32 banks.
- [x] Migrate TSR1/TSR3 before preset startup; exercise failure/retry and keep stock preset object 28 distinct from custom 30/31.
- [x] Shift custom state on track removal and reset only the newly added track.
- [x] Pass the full 109-job host/web suite and all 28 generator baselines; no golden baselines rewritten.
- [x] Complete final target/diagnostic package checks; prepare the hardware bundle with exact symbols and rollback source.
- [ ] Hardware validation: cold starts, USB/battery audio, CPU1 stability, migration, MIDI and recording.

Dual core remains the main build. Single core is permitted only with the private
STUDIO_SINGLE_DIAGNOSTIC switch for comparison; fault injection stays off.
Custom state uses the FBKT project container, with no new flash sectors and no
bootloader changes. Preserve pre-update backups for rollback; old firmware does
not read these projects or the unified preset banks.

## Audio investigation / hardening 5
- [x] Recover delivered `.4` source as baseline `1b1aa31`.
- [x] Record FM6 artifacts with/without FX, STOP tail and high single-track CPU report.
- [x] Reproduce/fix rejected audio jobs merging stale PCM and unchecked lengths on CPU0 or serial fallback.
- [x] Guard output destinations and submission bounds; keep NaN FM6 sums out of integer conversion.
- [x] Add 18 one-track FM6 STOP/tail scenarios with serial/dual equality and sanitizer coverage.
- [x] All 111 host/web jobs pass; sanitizer fault/tail tests and dual/single builds pass, within target budgets.
- [ ] Capture hardware counters to identify the reported distortion; cause remains unconfirmed.
- [ ] Hardware acceptance: FM6/Echo/dry/tails on dual core, USB and battery separately.

All UI proposals, including the HOME menu move, are deferred in UI_BACKLOG.md.
Preserve the present UI and custom features. The confirmed scheduler defect is
not yet established as the cause of the hardware distortion.

## FM performance controls — requested 16 October 2026

Planned after the audio investigation. Keep native operator pages for detailed
editing; add a predictable performance layer similar in accessibility to Prophet.
These are feature plans, not changes in the delivered Audio5 firmware.

- [ ] Add **FM AMP** in the ENV family: Attack, Decay, Sustain, Release.
  Controls are per FM track and govern an envelope for each sounding FM voice,
  preserving polyphony and independent note releases. Apply after that voice's
  native FM synthesis, before track FX, without rewriting operator envelopes.
- [ ] Provide neutral/bypass behaviour reproducing the original patch exactly.
  A multiplicative envelope cannot extend a voice beyond its native carrier
  release; document this and evaluate a separate opt-in release macro if needed.
- [ ] Add **FM TONE**: proposed Brightness, Contour, Tone Envelope Amount and Tone
  Decay. Shape several operator parameters together with algorithm-aware carrier
  and modulator mapping. This is filter-like timbre control, not yet a proposed
  conventional resonant filter or a commitment to final knob names/ranges.
- [ ] Retain all operator, native rate/level, algorithm and deep sound-design pages.
  Make operator selection and level-versus-rate naming clearer when UI work resumes.
- [ ] Preserve the underlying patch, clamp temporary derived values, avoid cumulative
  edits, and serialize the new controls with sounds/projects using a versioned,
  backward-compatible plan before changing any storage allocation.
- [ ] Check slow-attack pads and Bells, algorithms/carrier counts, live changes,
  polyphony, releases/voice stealing, DSP cost and neutral-state sample identity.

## Generator note length / slow attacks — reported 16 October 2026

Current source uses track **FEEL > GATE** (P_SGATE) as a fraction of the generator
DIV step, up to 127/128 of one step. It also unconditionally releases the previous
generated note at the next step, including rests. Lower pulse density therefore
does not extend an existing note across the intervening empty steps. HUMAN can
shorten the gate further. Slow-attack patches may reach note-off before blooming.
The user's report that the GATE knob seems ineffective still needs a direct
control-path check; reading the parameter does not prove physical knob operation.

- [ ] Add generator duration independent of trigger density: consider **step gate**,
  **fixed musical length** (e.g. eighth, quarter, half, bar) and **hold until next hit**.
- [ ] Preserve present step-gate behaviour by default; explicitly define retrigger,
  same-note, tie/legato and overlapping-note policies rather than permitting
  unbounded overlapping voices.
- [ ] Carry actual held lengths through one-pass REC, whole-take undo and MIDI
  note-offs. Release correctly on STOP, mute, bank/track/mode changes and clock loss.
- [ ] Verify FEEL GATE/DIV with hardware; test attack-time versus held-time, rests,
  HUMAN, swing, external clock and all four generators. Measure dual-core voice
  pressure with long notes and long-release sounds before setting upper bounds.

Current workaround: on the affected track use FEEL > DIV 1/4 or 1/2, GATE near
100%, HUMAN 0. Slower DIV also slows the rhythm; this is not independent note
length. Increasing the patch's release alone does not let the attack complete
while its key is held. Audio bug fixing remains the immediate priority.

## Expanded Studio Tess 7 execution (20 October 2026)

By: Inputsignwave

The approved expanded patch now has runtime, control, recording and FBK7 storage
integration in `studio-tess-7`. See `STUDIO7_BUILD_STATUS.md` for the implemented
paths, precise compatibility contract, validation evidence and remaining checks.
This integrates the updated handoff with our existing plans; current installed
firmware has no active audio fault report. Preserve Performance 6 as the baseline.

- [x] Pin and selectively adapt Tess v1.3.3 Grids, BOUNCE, AWAKE and POLY.
- [x] Implement/test pure local clock, deterministic kit, state and lossless step codecs.
- [x] Connect the new modes, controls, fractional capture and project state.
- [x] Pass focused real-sequencer, capture, track-removal and control-extreme checks.
- [x] Complete the expanded full host/web, target resource and disassembly checks.
- [x] Run the actual .7 firmware package in the chip emulator and prepare the candidate.
- [ ] Hardware acceptance on the FM-1 (no automated flash).

Final software validation: CI run 39449681801 passed on compiled commit `8057dbd`.
Full Linux host/web suite: 434 s; local suite: 164 s; 337 unchanged sound renders.
Strict/sanitizer, target resident-call/resource checks and exact-byte package
emulator smoke checks pass. CPU1 remains offline in the emulator; physical
acceptance is still required. No hardware flash or main merge was performed.

## Official FM1 Open tools review — 21 October 2026

By: Inputsignwave

Reviewed official source/SDK commit `4c7496d`; all 65 manifest file hashes match.
Useful reference areas: RGB565 dirty-band rendering, custom glyphs, audio/MIDI
diagnostics, bounded I/O contracts and reproducible build guards. The workbench
is a separate application preview; our chip emulator remains complementary.
The public FX code is basic browser reverb/delay, not a ready-made character
reverb or keyboard-stutter implementation.

- [ ] Apply relevant findings during UI/resource profiling and later FX design.
- [ ] Evaluate any binary-SDK/platform adoption separately, preserving our engines,
  FBK7 atomic storage and working baseline. This does not change current priorities.

See [the source-based assessment](docs/OFFICIAL_FM1_OPEN_REVIEW.md) for exact files, limits and proposed order. No firmware code or toolchain was changed.

## Performance 10 RC3 — Reverb naming and custom Prophet order

By: Inputsignwave

Display the new ID-10 plate as REVERB; retired IDs 7/8 remain hidden dry aliases.
Browse Prophet additions 201–212 first, followed by INIT 0 and original 1–200;
new Prophet engine selection starts at 201, explicit Init remains 0. Stored IDs,
patch bytes, projects and favorites are preserved. Development continues on
INPUTSIGNWAVE-GEN (Git branch names cannot contain spaces); studio-tess-8 is
retained as the prior development/archive reference. RC3 is built and offline validated: local full suite 262 s, Linux full suite
653 s, 349 unchanged golden renders, 2,401 concurrency combinations, zero UI
layout findings, 32,768 added randomized ASan/UBSan audio/preview blocks and two
6,000 ms chip-emulator scenarios. Native REVERB naming was verified in the
actual binary. Flash spare 976 B; main audio arena remains 51,520 B. See
[RC3 validation and limits](docs/PERFORMANCE10_RC3_VALIDATION.md). Hardware
audition remains; no device was flashed.
