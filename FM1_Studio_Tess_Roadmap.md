# Studio FM-1 roadmap

## Current execution — 20 October 2026

By: Inputsignwave

Performance 7 is implemented on `studio-tess-7`: selective Grids/BOUNCE/AWAKE/POLY,
independent meters and accent groups, ratios, phase/drift/freeze/realignment,
fractional capture and coordinated eight-lane KIT development with anchors,
densities, accents, ghost notes, hats, seeds, variations and fills. All synth
engines are retained. FBK7 fits the same atomic A/B allocation and imports FBKT/FBKK.
See `development/STUDIO7_BUILD_STATUS.md` and `development/STUDIO7_VALIDATION.md`
for exact software evidence and remaining physical acceptance. No device was
flashed. The user reports no current audio faults in installed Performance 6.

The dated checklists below retain the original planning/hardware-report context.
They do not indicate that the features just listed are still unimplemented.
Hardware acceptance and measured polish come next; broader phrase/arrangement
tools, sound/timbre morphing, BRANCH/LOGIC, SLOTH/OCHD and MPE remain later work.


## Generator integration and attribution — agreed 17 October 2026

By: Inputsignwave

- [x] Selectively integrate Grids, BOUNCE, AWAKE and POLY, retaining the full existing synth engine set, including FM6, CZ-1 and Prophet. Do not adopt the donor MACRO/reduced-engine build profile.
- [x] Preserve atomic project storage, object identities and legacy migration; adapt the donor fields into FBK7 without overlapping flash objects, importing original FBKT/FBKK layouts.
- [x] Verify harmony/SONG, durations, MIDI ownership, capture, FORM/EVOLVE, seeds and software budgets; hardware acceptance remains pending.
- [x] Retain upstream generator credits and mark our original work, ports, fixes and hardening as Inputsignwave in source and handoff material.

The user reports that the Tess developer has received our build and has adopted or intends to adopt our features and bug fixes. This is collaboration context, not independent verification of which patches have landed upstream. Preserve Inputsignwave credit with ported contributions. The generator work above is implemented in Performance 7; the earlier attribution-only update did not implement it.

## Goal
Keep Melodee's synth engines and sound-design facilities, adding Tessera generative sequencing and rhythm morphing. The synth FM-1 may be played from a Seaboard RISE or sequenced by the Polyend Tracker. A second FM-1 with a groovebox focus remains optional. zp12 is excluded.

## Next development phase — approved 16 October 2026

Current hardware baseline: **v1.4.1-studio-tess.6**. The user reports that it is
working very well. This is positive hands-on feedback, not a claim that every
hardware acceptance check or the intermittent Bells fault is resolved.

Agreed implementation order:

1. **Kit-aware drum generation built around the user's own patterns.**
2. **Phrase variations and fills**, retaining a recognisable core groove.
3. **Macro sound morphing** with snapshots and bounded evolution.
4. **Full native-patch morphing later**, after engine-specific design and testing.

MPE and unrelated UI tweaks remain deferred. This phase is planned work; it is
not included in the delivered Performance 6 firmware.

### 1. Coherent kit-aware drum generation

- [ ] Operate the existing eight lanes of one DRUM track as a coordinated kit,
  leaving the other tracks available for melodic parts.
- [ ] Generate supporting material around an existing user pattern, with explicit
  anchor locks protecting selected kick, snare or other hits. Do not force one
  backbeat or genre on every pattern.
- [ ] Provide independent per-lane density for kick, snare, hats and percussion.
- [ ] Model lane relationships: percussion responses to the kick, space around
  strong snares, and coordinated open/closed hats with the kit's choke rules.
- [ ] Add a shared groove/timing feel with bounded instrument-specific variation.
- [ ] Generate coherent velocity accents and ghost notes, not only hit placement.
- [ ] Keep density, variation amount and fill intensity separate, allowing sparse
  evolving patterns or dense steady patterns.
- [ ] Add repeatable seeds, regeneration, freeze/recall of a good result, and
  non-destructive capture into the normal editable sequencer pattern.
- [ ] Extend one-pass REC/undo and USB MIDI to preserve simultaneous drum lanes,
  velocities and accents within the recording and voice budgets.

### 2. Phrase variations, fills and arrangement

- [ ] Repeat a recognisable core groove over a chosen phrase, with controlled
  variations and fills at phrase endings rather than unrelated random changes.
- [ ] Connect drum sections to FORM and SONG where appropriate, with a reliable
  return to the original groove and protected anchor hits.
- [ ] Allow a restrained fill independently of overall pattern density.
- [ ] Preserve the base pattern, section identity and seed through save/load.

Example four-bar behaviour, as an illustration rather than a fixed style:

| Bar | Behaviour |
|---|---|
| 1 | Establish the core groove |
| 2 | Repeat with a small hat variation |
| 3 | Add a percussion response or ghost note |
| 4 | Introduce a restrained fill and return cleanly to bar 1 |

### 3. Macro sound morphing and bounded timbre evolution

This is separate from the existing MORPH generator, which blends rhythmic hit
patterns. Start with one engine instance per track; do not require two complete
synth engines to render simultaneously.

- [ ] Capture A/B versions of the same sound and move between them manually or
  over a selected number of bars.
- [ ] Begin with FM AMP, FM TONE and suitable existing engine/effect controls.
  Preserve the underlying patch and derive temporary effective values, avoiding
  cumulative edits.
- [ ] Add bounded parameter wandering with independently selectable parameters
  and limits. Protect pitch, tuning, volume, algorithm or release as desired.
- [ ] Provide proposed POSITION, DEPTH, SPEED and LOCKS controls; final layout and
  ranges remain to be designed.
- [ ] Add repeatable seeds and a way to freeze, recall or capture a good result.
- [ ] Give FORM sections A/B/C recognisable sound characters; link transitions
  to phrases and restore section identity when returning to A.
- [ ] Smooth continuous changes. Apply envelope-time changes at suitable musical
  boundaries when necessary, without repeatedly restarting sounding envelopes.
- [ ] Define explicit switching points for discrete settings such as algorithm,
  waveform selection and FX type; preserve effect tails appropriately.
- [ ] Keep these trajectories coordinated with the rhythm where wanted: section
  B can become busier and brighter, while A returns to its original groove/sound.

### 4. Later full native-patch morphing

- [ ] Design interpolation separately for each engine and compatible snapshot
  pair, initially within the same engine.
- [ ] Distinguish continuous quantities from discrete topology/settings and use
  musically appropriate curves for levels, frequencies and envelope times.
- [ ] Assess transition noise, carrier/modulator roles, voice lifetime, memory,
  DSP cost and snapshot storage before extending beyond the macro layer.
- [ ] Cross-engine audio crossfading is not committed as part of the first phase.

### Integration and acceptance

- [ ] Plan snapshot/seed/generator persistence and versioned imports before
  changing the existing tightly allocated project format.
- [ ] Retain the current generators, harmony, independent note lengths, release
  protection, MIDI lifecycle and one-pass recording behaviour.
- [ ] Validate anchor preservation, seed repeatability, phrase returns, drum
  choke/velocity behaviour, capture replay and undo, neutral sound identity,
  transition continuity, save/load and dual-core CPU/voice limits.
- [ ] Package a separate hardware-test candidate when implemented; retain
  Performance 6 unchanged as the comparison baseline.


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

Approved for the next phase described above: macro sound morphing first, full
native-patch morphing later. Separate from Tess's existing rhythm morphing and
not implemented in Performance 6. Engine-specific interpolation, discrete
settings, transition noise and CPU cost require design and validation.

## Later MPE phase for the synth unit

**Explicitly deferred by the user on 16 October 2026. Not part of Performance 6.**
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

- [ ] Add generator duration independent of trigger density: provide **step gate**,
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


## Generator duration, release and transition clicks — confirmed request 16 October 2026

The user confirms DIV changes note length as described, but requests independent
note duration and release/fade controls. They report audible pops/artifacts when
the next generated hit cuts the previous note. Treat this as an audio defect to
investigate; an abrupt waveform discontinuity or voice reuse is a hypothesis,
not a demonstrated cause or proof that it explains the earlier FM6 Bells fault.

- [ ] Add per-track **DURATION MODE**: existing step gate, fixed musical duration,
  and hold until the next actual generated hit. A rest must not end a held note
  in hold-until-next-hit mode. Duration must not require changing DIV or density.
- [ ] Add **NOTE LENGTH** for fixed duration (proposed eighth, quarter, half and
  whole-bar values; final range to be validated). Define fixed notes crossing
  later hits explicitly: permit bounded overlap where supported, with a clear
  retrigger/legato policy and safe same-pitch note ownership.
- [ ] Add **RELEASE / FADE** independent of held duration: control how the internal
  voice ends after note-off or replacement. Keep native patch release available;
  define a neutral/native setting and a user-adjustable fade time. Do not describe
  a gain fade as capable of extending a native voice that has already ended.
- [ ] Prevent hard truncation at next-hit transitions and voice stealing: retain
  the outgoing release or use a short bounded fade/crossfade before voice reuse.
  Never fade the entire track to end one voice, or erase other notes/FX tails.
  Smooth live changes to fade/envelope parameters and measure dual-core cost.
- [ ] Preserve held duration in USB MIDI note-offs and one-pass REC/undo. Capture
  note-off timing separately from audible release. MIDI note-off alone cannot
  impose the internal fade on external synths; external release follows their
  envelopes unless explicit supported control mapping is added.
- [ ] Make captured STEP playback use the intended release/fade settings and save
  them with versioned project state. Define capture-boundary handling for notes
  held past the one-pass endpoint without hanging notes or losing undo integrity.
- [ ] Reproduce next-hit clicks with slow attacks and FM6 Bells; test dry first,
  then Echo/Reverb and STOP tails. Cover repeated pitches, rests, overlapping
  releases, all four generators, long holds, voice exhaustion, live duration
  changes, tempo/clock changes, STOP/mute and track/bank/mode changes.
- [ ] Validate transitions for discontinuities without smoothing intentional
  percussive attacks; check neutral-mode regressions, MIDI event pairing, REC
  replay, sustained CPU load and serial/dual agreement before hardware delivery.

Priority: investigate transition clicks alongside audio bug fixing; implement
independent duration and release/fade as a coordinated improvement. All unrelated
UI proposals remain deferred. This is roadmap scope, not a delivered firmware fix.

## Performance 6 — implementation on Audio Hardening 5

Hardware-test candidate **v1.4.1-studio-tess.6**. This section records the
implemented status of the earlier FM performance and generator-duration requests.

- [x] Add per-track STEP / FIXED / HIT duration modes. HIT holds across rests until
  the next actual generated hit. Fixed values: 1/32, 1/16, 1/8, 1/4, 1/2, one bar.
- [x] Keep DIV/rhythm independent of fixed held length. Offer OVER (up to four held
  pitches; default for FIXED) and REPL, with explicit same-pitch retrigger and oldest
  owner release when the held-key table fills. Preserve the synth's voice mode.
- [x] Add per-voice RELEASE/FADE independent of held time, with a NATIVE bypass.
  FM6 keeps its native operators keyed during an explicit outer fade, so fast R4
  cannot truncate that fade. An already decaying held sound can still become silent.
- [x] Add bounded FM6 voice-reuse sample correction when FADE or FM AMP is active.
  Preserve emergency fades and keep normal FM6 lifetime under its native envelopes.
  This is not proof of the cause or cure of the reported hardware clicks/Bells fault.
- [x] Add overall per-voice FM AMP ADSR; editing enables it, with separate bypass.
- [x] Add algorithm-aware FM TONE brightness, modulator contour, bipolar tone-envelope
  amount and decay. Keep the native operator pages and original patch bytes.
- [x] Extend actual note lifetimes through MIDI and one-pass REC. Seal held recorded
  notes at punch-out without stopping the generator. Keep whole-take undo/redo.
- [x] Save timing and performance controls in existing project allocation (HMF2,
  same 116-byte extension). Read HMF1 with bypass defaults; retain .2/.3 migration.
- [x] Save FADE and FM AMP/TONE in FM6 shared sounds, including sound undo/redo,
  renaming and native patch updates. Preserve generator timing on sound loads.
- [x] Keep existing engine voice limits, shared voice budget, boot stub and loader;
  retain Audio5's scheduler and non-finite-output hardening.
- [ ] Hardware validation: slow attacks, sparse Euclid holds, overlap, repeated
  pitches, FM6 Bells, dry/FX/STOP tails, MIDI, capture/undo and save/reload.
- [ ] Capture device counters if distortion returns; actual CPU load and the
  intermittent Bells fault remain unresolved by host simulation.

At Performance 6 delivery, dual core remains primary and single core diagnostic
only. That build does not include the now-planned morphing/drum phase. Unrelated
UI tweaks and MPE remain deferred. Feature pages
are added without implementing the deferred HOME/navigation redesign.

Compatibility: .5/.4 cannot read newly saved HMF2 projects. Preserve the full
pre-install backup; rollback needs both prior firmware and that backup. MIDI
note-off carries held duration; external instruments use their own release
envelopes. The internal FADE is not sent as an external synth envelope command.