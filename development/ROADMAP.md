# Studio FM-1 roadmap

## Top priority for 21 October 2026 — navigation and page guide

By: Inputsignwave

User feedback, 20 October: Performance 7 is installed and **working perfectly**
in their testing. Treat it as the working baseline for the navigation work.
This records positive device feedback without claiming that every formal
endurance/acceptance scenario has been exercised.

Tomorrow's first development work is page/menu organisation and a function
reference, ahead of additional feature development.

- [ ] **First layout change: conditional generator pages.** Keep PATTERN MODE
  always accessible; show beat/generator-specific parameters only for the mode
  selected on the current track. Hide irrelevant generator pages from scrolling
  and page maps, preserve their saved settings, and retain applicable shared
  controls. See [the agreed visibility rule](docs/GENERATOR_VISIBILITY_DRAFT.md).
- [ ] Move the current **hold-OCT+ action sheets into the HOME menu**, as
  requested: sound tools, track actions, pattern/selected-note actions, song
  section/jam actions and shared-FX actions. **Create New Track** is the first
  directly selectable item (renamed from Add track). Move **Sound Designer**
  out of these actions into the **EDIT** menu. Preserve the target
  context, selections, undo and existing action checks. Inventory OCT+ tap
  actions separately so no functions disappear during remapping.
  See [the editable HOME actions draft](docs/HOME_ACTIONS_DRAFT.md).
- [ ] **FM6 EDIT: map the physical ALGORITHM knob to FM algorithm selection
  (1–32)** for the selected track, with immediate value/diagram and sound updates.
  Preserve the rest of the patch and existing save behavior. The current encoder
  selects tracks; revise that behavior in the FM6 editing context and document
  track selection in the new map. See [control mapping decisions](docs/CONTROL_MAPPING_DRAFT.md).
- [ ] Move **OUTPUT from SEQ to GLO (Global), as the second page after MIXER**.
  Keep MIDI output enable/channel
  scoped to the selected track, show its target clearly, and retain existing
  note-release and project-save behavior. This changes the page location, not
  the scope of its settings.
- [ ] **Colour/aesthetic identity:** include a UI colour change in this update
  as the first step toward our own Studio Tess visual identity. Review the
  current palette/track/status swatches before choosing replacements; keep
  contrast, selected/inactive states and track identities clear. See the
  [current colour reference](docs/UI_COLOUR_REFERENCE.md). **LO-FI is the provisional palette choice** (21 October); use it as the starting point for menu mockups and the new visual identity.
- [ ] Inventory the current pages, controls and navigation paths, including the
  new Performance 7 pages, and identify crowded or misplaced groups.
- [ ] **Layout correction, 21 October:** keep all existing sequencer pages in
  **SEQ**, including beat development, rhythm MORPH/EVOLVE/FORM/TWEAK, meters,
  phase, seeds, AWAKE/POLY, HARMONY and NOTE RANGE. Only OUTPUT leaves SEQ
  for GLO. Sound Designer is the only function being moved into EDIT; do not
  add a duplicate GENERATOR page there. Reduce SEQ scrolling through conditional
  generator visibility instead of relocating its controls.
- [ ] Keep core sequencing, pattern/transport and recording controls easy to find;
  group related generator, beat-development and editing controls coherently.
  Preserve quick access during playback and the existing hardware navigation model.
- [ ] Create a page guide with a quick navigation map and a reference for every
  page/function: where to find it, what it changes, scope (track/bank/project/global),
  defaults/bypass, applicable modes/engines, and save/recall behavior.
- [ ] Explain relationships and precedence with examples: MODE/generator versus
  the source grid; DIV versus meter/grouping/ratio; phase/drift/HOLD/ALIGN/reset;
  FORM/EVOLVE/MORPH and seeds/conditions; anchors/density/fills/hats; harmony/pitch;
  note length/release; capture destination and STEP replay. Link related pages
  and make inactive or mode-dependent controls clear.
- [ ] Validate the reorganised navigation in UI tests/screenshots and on the FM-1,
  checking page selection, labels/help and access to every existing control.
  Keep audio, timing, storage and established parameter behavior unchanged by
  the navigation-only patch. Update the guide to match the final layout.

Performance 8 implementation is in progress on `studio-tess-8`. The corrected
page families, generator visibility, HOME actions, FM6 algorithm encoder and
LO-FI default are implemented locally. The page/function reference is
[STUDIO8_PAGE_GUIDE.md](docs/STUDIO8_PAGE_GUIDE.md). Full target validation and
packaging are required before this candidate is delivered. The working
Performance 7 package remains preserved.

## Planned phase — character reverb and playable FX

By: Inputsignwave

Added 20 October 2026. Navigation reorganisation and the page/function guide
remain the next-session priority; this is additional development roadmap scope.

### Character reverb and stereo space

- [ ] Research and audition a more interesting reverb alongside the current one,
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
