# Studio FM-1 roadmap

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

Exploratory and deferred; not part of Performance 6.
Confirm whether patch/timbre morphing is wanted in addition to Tess's rhythm morphing. Engine-specific parameter interpolation, discrete settings, transition noise and CPU cost require separate design and validation. Not implemented by this merge.

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

Dual core remains primary; single core is diagnostic only. All unrelated UI
tweaks, MPE and exploratory patch/timbre morphing remain deferred. Feature pages
are added without implementing the deferred HOME/navigation redesign.

Compatibility: .5/.4 cannot read newly saved HMF2 projects. Preserve the full
pre-install backup; rollback needs both prior firmware and that backup. MIDI
note-off carries held duration; external instruments use their own release
envelopes. The internal FADE is not sent as an external synth envelope command.
