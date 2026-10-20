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
