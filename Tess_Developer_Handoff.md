# Tess / Melodee Studio development handoff

**By: Inputsignwave** — Studio Tess custom features, integration, bug fixes and hardening. Original Melodee, Tess, Felucca and third-party credits remain applicable.

Attribution update, 17 October 2026: when porting the Studio Tess contributions described here into Tess/Tessera, retain the exact contributor name **Inputsignwave** in source comments and project credits. Scope this credit to the custom additions and fixes; preserve every existing author and licence notice. See [CREDITS.md](CREDITS.md). The Word companion is the original 16 October snapshot.

Developer summary and technical reference for Claude Code

16 October 2026 | M-VAVE FM-1 | Current baseline: v1.4.1-studio-tess.6 (Performance 6)

## 1. Summary for the Tess developer

This branch brings Tess's generative sequencing onto Melodee while retaining Melodee's synthesizers and sound-design facilities. Development began with Melodee 1.3.5 and Tess's merge-upstream-1.3.1 donor branch. It now runs on a pinned Melodee 1.4.1 base. Tess's Turing, Euclid, Markov, rhythm MORPH, EVOLVE, FORM, TWEAK and FOLLOW remain, alongside ordinary STEP sequencing. The substantial additions are weighted harmony and SONG progressions, sequencer USB MIDI output, one-pass capture, independent generated-note duration, release fades, and per-voice FM amplitude/tone controls.

The latest delivered build is Performance 6. The user has installed it and reports that it is working very well. Dual core remains the primary target; single core is supplied for controlled diagnosis. The software checks are extensive, but the earlier intermittent red-screen faults, Prophet preset-change freeze and FM6 Bells distortion have not been conclusively diagnosed or demonstrated fixed on hardware.

### What changed most materially

| Area | Result and significance |
| --- | --- |
| Generator safety | Live edits publish atomically against CPU0's audio interrupt; mutation and section recall are bounded; mode changes release owned notes, including MIDI note 0. Archive validation no longer destroys unsaved runtime phrases. |
| Harmony and composition | OFF/KEY/SONG harmony, note range, outside-note probability, chord bias and resolution; 16 SONG rows can carry chord, key/scale overrides and bar duration. |
| Output and capture | Final played notes reach USB MIDI with tracked note ownership and deferred note-offs. One generator pass records into an empty normal pattern, then disarms while generation continues. |
| Storage migration | Melodee 1.4's shared presets collided with Tess's old flash sectors and backup IDs. Custom state now commits atomically with the project in FBKT; legacy migration completes before presets can reuse the old sectors. |
| Confirmed audio defect | Rejected CPU0/serial-fallback audio jobs could merge stale PCM or unchecked lengths. Only validated, successfully executed jobs now merge; descriptor guards and submission bounds were strengthened. |
| Defensive audio handling | NaN FM6 sums are quarantined before integer conversion. This prevents undefined conversion; it is not a diagnosis of the reported Bells fault. |
| Performance 6 | STEP/FIXED/HIT duration, bounded four-key overlap, per-voice fade, FM AMP ADSR, FM TONE macros and bounded FM6 reuse correction. Actual held duration carries into MIDI and REC. |

The current full host/web suite passed 114 jobs. All 337 existing sound-render hashes remain unchanged in bypass, and the 28 established generator scenarios retain their current baselines. The enabled-feature stress test compares concurrent and serial rendering over 16,384 blocks, including MIDI, recording, effects and live edits. These results establish software regressions and invariants; they do not measure device CPU headroom or long-session stability. [E1-E6]

### Roadmap agreed next

1. Kit-aware generation around the user's own eight-lane DRUM patterns: protected anchor hits, per-lane density, coordinated kick/snare/hat/percussion relationships, choke rules, accents/ghost notes, repeatable seeds and editable capture.
2. Phrase variations and restrained fills that retain a recognisable base groove, return reliably to it, and connect to FORM/SONG.
3. Same-engine macro sound morphing: A/B snapshots, bounded parameter evolution and locks, repeatable trajectories, section-linked sound identities and smooth continuous transitions. Begin with FM AMP/TONE and suitable existing controls, using one engine instance per track.
4. Full native-patch morphing later, with engine-specific interpolation and explicit rules for discrete algorithm/waveform/FX changes. Cross-engine audio crossfading is not committed.

MPE and unrelated UI redesign remain deferred. These roadmap items are planned; none of this new drum/morphing phase is in Performance 6. Existing MORPH blends rhythmic hit patterns, not synthesizer patches. Fixing a reproduced regression takes priority. [E7]

### What to hand Claude Code

Provide this document's Markdown companion with the full Performance 6 source bundle. The document is an index and technical explanation, not a source-code substitute. Start from the pinned .6 snapshot and inspect the tests before porting. The .5-to-.6 patch covers only the latest increment, not the complete history. Preserve attribution, current engines, neutral sound identity, recording capacity, CPU1 ownership rules and migration safety. Sections 3-13 describe the implementation; sections 14-16 give validation, working instructions and source references.

## 2. Scope, provenance and version history

The distinction between donor code, upstream changes and studio additions matters. The original generators and their musical concepts came from Tess. Melodee supplies the synth engines, effects, transport, storage framework, hardware support and much of the recording/history system. Our branch integrates them and adds the features and hardening described below. This handoff concentrates on functional and safety changes; cosmetic layout revisions are not catalogued individually.

| Milestone | Base / identity | Main work |
| --- | --- | --- |
| Integration 1, 14 Oct | v1.3.5-studio-tess.1 | Merge Tess donor onto Melodee 1.3.5; preserve engines and boot/update protections; project/backup integration; truncated restore-header fix. |
| Stability 2, 15 Oct | v1.3.5-studio-tess.2 | Interrupt-safe edits, note ownership, bounded mutation/seed generation, safe section recall, pure archive validation. |
| Feature 3, 15 Oct | v1.3.5-studio-tess.3 | Harmony, SONG harmony/bar timing, USB sequence notes, one-pass generator REC, HMF1/TSR3 and failed-save retry. |
| Update 4, 16 Oct | v1.4.1-studio-tess.4 | Rebase/integrate onto pinned Melodee 1.4.1; resolve flash/backup collisions; FBKT atomic projects and legacy migration; adapt track and recording lifecycle. |
| Audio Hardening 5, 16 Oct | v1.4.1-studio-tess.5 | Rejected-job merge fix, descriptor validation, non-finite FM6 output guard, real STOP/tail tests and diagnostics. |
| Performance 6, 16 Oct | v1.4.1-studio-tess.6 | Duration/fade, FM AMP/TONE, actual-duration capture/MIDI, FM6 reuse continuity, HMF2 and FM sound extension. |

### Pinned references

- Melodee 1.3.5 input: `baeabc495c5ae0a6df48c6cf81d056fbc59abb6b`.
- Melodee 1.3.4 comparison: `f7cd3eafd593d9e585895bfaf2c9550a03662ac7`.
- Tess donor: `34d401c17b13cc4cb0089e4de1b48c55438789bf`; Tess main reference at retrieval: `a4533b5dff19cfd1a6a7b38f508ab10944ae7094`.
- Integration merge: `6d55c93`; documented .1 snapshot: `65fe2b7`.
- Stability implementation: `01b3799`; documented .2 baseline: `64054b3`.
- Feature implementation: `932cbff43f74cd27b574633781256d67096102e6`; subsequent feature-panel refinements: `defcafd4b7912ce3822a9fafb78cec18b937be5e`. Update 4's release notes identify its delivered .3 input as `a3d9f1d`.
- Melodee 1.4.1 input: `1cccd697`. Delivered .4 source reference: `1cdccb7add7679aa7af3b8c6a5231e51bd0b904b`, recovered locally as `1b1aa31`.
- Audio5 working baseline recorded in the .6 bundle: `c31eea3823960d2a0803d32a42cb0e49f7a1965f`.
- Performance 6: `72305f27c6b29a9c4a7d6f27541e62b3e8db93f4`.

These references come from release notes, retained git history and `SOURCE_COMMIT.txt`. Different documented implementation, packaging and recovered-snapshot IDs describe different points in the history; do not assume they are interchangeable commits available in the receiving repository. The source archive is a complete snapshot without its git database. [E1-E6]

Repositories: https://github.com/keremimo/melodee and https://github.com/Rastapopolix/melodee-tess.

## 3. Architecture and integration boundaries

Paths below are relative to the delivered `source/` root. Firmware implementation files share the `firmware/src/` prefix unless stated otherwise. Many functions are static because this firmware is largely built as one translation unit. `melodee.c` includes engines, parameters, voice/mix, USB, `tessera.h`, SONG chain, sequencer, `tessera.c`, `tess_features.c`, audio, UI and storage in an intentional order. `voice.c` includes `performance.c`; `core.h` brings in performance declarations. A port that compiles each new .c independently will need an explicit interface refactor.

Generator decisions and control preparation belong to CPU0. CPU1 renders submitted voice-sample jobs; it does not independently advance generators or edit their settings. Submitted descriptors own separate PCM scratch. The producer publishes complete jobs, joins before scratch reuse and merges in original voice order. Existing fault retirement drops failed worker PCM rather than replaying a job whose voice state may already have advanced.

The principal musical path is: generator event and existing FOLLOW/SHIFT/flourish/transpose processing, final harmony selection, note dispatch, internal synthesis and optional USB MIDI, with capture of the played event into ordinary timed recording. Performance 6 adds held-key ownership around dispatch. Audible envelope release remains a separate stage after key release, before track FX; effects retain their own tails.

### File map

| Files | Responsibility / integration point |
| --- | --- |
| `tessera.h`, `tessera.c` | Four generators, EVOLVE/FORM/TWEAK/FOLLOW, generator lifecycle, serialisation and pure validation. |
| `tess_features.h`, `tess_features.c` | Harmony, feature runtime, MIDI ownership, one-pass capture, HMF1/HMF2, independent duration. |
| `seq.c`, `recording.c`, `song_chain.c` | Transport stepping, timed-event recording/replay, recording gates and SONG row timing. |
| `performance.h`, `performance.c` | Performance configuration, per-voice gain state, ADSR/fade and FM sound-extension helpers. |
| `voice.c`, `eng_fm6.c`, `fm6_core.c` | Voice lifecycle, application of per-voice gain, derived FM tone values and reuse continuity. |
| `audio_batch.h` | Bounded audio descriptors, worker execution, validity/guard checking and safe merge. |
| `project.c`, `pattern_store.c`, `tess_project.c` | Project snapshot/import, FBKT packing, atomic save, tagged legacy migration. |
| `pattern.c`, `tracks.c` | Bank changes, track add/remove and custom-state ownership/reset. |
| `editor_backup.c`, `web/fm1backup.js` | Full backup objects, legacy namespace translation, validation and restore ordering. |
| `upreset.c`, `shared_presets.c` | FM user-sound macro persistence and preservation during native updates. |
| `ui_tess_features.c`, `ui_input.c`, `ui.c` | Functional controls, IRQ-safe feature edits, REC admission/history and sound undo. |
| `params.c`, `ui_pages.c`, `ui_graph.c`, `ui_draw.c`, `ui_patterns.c` | Feature page registration, values/status drawing and effective SONG timing display. |
| `melodee.c`, `core.h`, `usb.c` | Include/declaration integration, build/version identity and MIDI queue admission. |

The full custom firmware delta against pinned upstream 1.4.1 covers 35 firmware files. Appendix A lists each file and its purpose, including small support hooks. This is a cumulative comparison, not a claim that all changes originated in Performance 6.

## 4. Integration 1: importing Tess without losing Melodee

The initial merge used Melodee 1.3.5 rather than replacing it with Tess's older base. All current Melodee engines, effects, boot protection, loader and update handling were retained. Tess's four per-track generators, phrase evolution, remembered FORM sections, TWEAK controls and generator save/autosave/backup hooks were imported. Normal STEP remained available. Sound-patch morphing, MPE and Bluetooth were not added.

The donor integration touched `tessera.c/.h`, sequencer dispatch/release in `seq.c`, project/flash hooks in `project.c` and `storage.c`, editor backup, parameter/page registration, drawing/input and the quick-layer/menu controls. Console/host-simulator hooks and generator baselines made the imported behaviour observable. Installer naming and test-runner conflicts were reconciled while keeping current boot tests; stale donor precompiled site packages were removed from the custom tree.

A donor restore routine could read its header before confirming the input was long enough. The restore path was reordered to validate length first, with a one-byte archive regression in `tests/backup_test.c`. This is a concrete bounds fix, independent of the later state-purity fix.

Original generator metadata used a 180-byte block, four 45-byte track records, and a tagged TSR1 sidecar in the A/B sector pair at `0x9D000` / `0x9E000`. The tags tied settings to the matching saved project. Backup objects 28/29 carried saved/live generator state. These are historical formats; sections 8 and 12 describe the current layout.

Initial builds succeeded for dual and diagnostic single core, and generator baseline tests passed. The known CZ-1 determinism/dim-control failure also reproduced on unmodified Melodee 1.3.5; it was recorded rather than hidden by changing expected audio. Upstream corrected it later in the 1.4 integration. [E1]

## 5. Stability 2: generator safety and atomic validation

### 5.1 Live edit publication

`tessera_knob`, `tessera_evolve_knob`, `tessera_form_knob` and `tessera_tweak_knob` wrap bounded body functions with `TESS_IRQ_OFF/ON`, using the target's CPU0 interrupt controls. Playback cannot observe a partly changed generator configuration. Knob indexes are checked, zero edits return early, and `tess_knob_delta` saturates incoming deltas to -128..128 before arithmetic. Invalid/extreme encoder input therefore cannot overflow the parameter calculations.

This adopts the Tess interrupt-fix approach from `b89586ac4409303dbed21c17405328dd19237e4a`. Mutation safeguards were adapted from Tess commits `0e2059d71a0808783589383e16c06614a7d12f8c`, `c8211c06a81c0c264c6cf30c23e8173deed49333` and `9bbb7fc`. Preserve those provenance links when porting. Critical sections must remain bounded and contain no I/O or allocation.

### 5.2 Release ownership and bounded mutation

- Mode changes explicitly release the generator's own active note. Stress testing exposed stuck MIDI note 0 when ordinary sequencer release mistook the idle arpeggiator's zero sentinel for another owner. The explicit generator release fixes that ambiguity.
- `tess_new_seed` makes one bounded draw excluding the previous nonzero Euclid seed, replacing a random retry loop. ROLL remains immediate.
- Turing mutation repairs degenerate all-zero/all-one active registers for active lengths greater than one.
- Euclid pulse mutation reflects inside 1..STEPS-1; STEPS=1 remains one pulse. Manual pulse editing still permits zero/all hits. This distinction preserves intentional manual patterns.
- Markov LEAP/tension mutation reflects at 0 and 100 instead of sticking at a clamped edge.
- `tessera_mutate` with AMOUNT=0 returns without consuming RNG or resetting/rearming the playhead. Explicit Euclid ROLL is still allowed to change its seed.
- `tess_load_sec` clamps remembered FORM values at use. Shortening Euclid STEPS cannot reintroduce an oversized saved pulse count or rotation. Old stale section bytes remain readable.

### 5.3 Archive validation must be pure

Previously, saved-slot validation temporarily unpacked candidate settings into live Tess state and then restored only the packed settings. That reset playheads and lost unsaved LOCK phrase buffers even when an archive was rejected. `tess_pk_track(..., apply=0)` and `tessera_valid` now validate without applying. `tessera_unpack` validates all tracks before initialising or writing live state. The restore path uses the pure validator.

The current location of legacy-object validation is `tess_project.c:tess_blob_valid`; the original .2 implementation lived in `project.c`. Validation and application remain distinct in HMF2 as well. Do not validate by a load-and-restore round trip.

### 5.4 Evidence and intentional baseline changes

`tests/tessera_hardening_test.c` exercises 200,000 edits with pending playback, invalid tracks/knobs and extreme signed deltas; mutation/FORM bounds; amount-zero RNG preservation; and 46,080 archive byte mutations. Invalid loads are atomic and validation preserves complete runtime state. Target disassembly verified CLI/CSYNC/STI in all four handlers. Concurrent stress compares audio and full saved state against serial dispatch.

Exactly four Euclid pitch baselines changed in .2 because bounded seed selection changed random mapping: `evolve_euclid_every1_amount80`, `form_euclid_abcb_morph_aabc`, `division_gate_length`, `all_four_tracks`. Event times, velocities, durations and counts remained unchanged. All 28 current scenarios pass; do not describe the entire development history as having no golden changes. The full .2 suite was 96/97, with the already documented CZ-1 failure. These fixes did not establish the cause of physical crashes. [E2]

## 6. Feature 3: weighted harmony and SONG composition

`tess_features.c:features_pitch` implements OFF/KEY/SONG. OFF returns the proposed pitch unchanged and preserves the prior generator random stream. Harmony runs after FOLLOW, SHIFT, flourish and transpose, so later pitch controls do not bypass an enabled zero-OUTSIDE constraint.

KEY derives the effective context from existing scale/root/chord controls. SONG derives it from the running arrangement row, or previews the selected harmony row before arrangement playback. `harm_track_t` holds mode, low/high MIDI range, outside probability, chord bias, resolution, MIDI enable and channel. Track root remains track-specific; the existing scale control is shared.

Pitch selection weights eligible candidates within the chosen range instead of simply quantising every proposed note. OUTSIDE chooses the scale category probabilistically. CHORD favours chord tones within that category; an implicit diatonic triad is available when no explicit chord is selected. RESOLVE can force the next choice inside after an outside note. Context changes favour proximity to the preceding played pitch, without guaranteeing the closest candidate. Empty candidate categories fall back within range; chromatic scales have no outside category, and a single-note range cannot invent an unavailable note.

The supported harmony domain is the 16 conventional twelve-tone scales. Extended microtonal layouts retain existing HARM OFF generation; enabling harmony is refused, or it is disabled when changing to those layouts. This avoids treating degree indices as chromatic semitones. Tests span all 16 scales and 12 roots, category extremes, range fallback, chord weighting, resolution and context overrides.

`harm_song[16]` stores chord root/quality, optional key/scale override and explicit bar duration. Sentinels mean inherit KEY/TRACK/PAT. BARS=0 preserves pattern-derived duration; 1..16 supplies musical bars, multiplied by the existing row repeats. `song_chain.c` integrates this with arrangement timing; `ui_patterns.c` shows the effective duration. The controls guide melodic note selection; they do not turn a monophonic generator into polyphonic chord generation. [E3]

## 7. Feature 3: MIDI output and one-pass capture

### 7.1 USB note ownership

Per-track OUTPUT selects MIDI OFF/USB and channel 1..16. Older projects default OFF, with initial channels corresponding to tracks 1..4. Both final generator events and ordinary STEP/timed-recording notes use `features_midi_on/off`. Existing keyboard USB output remains independently routed; no MIDI clock output or Bluetooth was added, and the TRS connector remains input-only.

`feature_run.held` and `.off` use four 32-bit words per track to represent all 128 pitches, including note 0. `features_midi_on` records ownership only after `midi_try_out` accepts the packet. Off requests remain pending until queued. `features_midi_poll` retries releases with a bounded budget and gives them priority over new ons; congestion may drop a new note rather than lose a required release. A channel change first releases notes on the old channel. Pending releases survive project initialisation until USB can accept them.

When multiple sequence tracks share a channel and pitch, releasing one owner must not silence another. The shared-owner check suppresses premature note-offs. This is sequence ownership logic, not MPE or arbitrary DAW feedback management. `usb.c` provides nonblocking queue admission; `seq.c` and recorded replay route through the feature hooks.

### 7.2 One-pass generator recording

`features_capture_prepare` admits only an empty destination with capacity and no running SONG arrangement. REC while stopped arms for PLAY; during playback it waits for the track's next loop start. Capture covers one pattern-length pass, then automatically disarms while the generator continues. It records final pitch, onset, velocity and held duration after the musical transformations, using ordinary timed-event storage rather than MIDI loopback.

`features_capture_guard`, `features_capture_note` and `features_capture_end` coordinate lifecycle. Captured events remain silent while a generator is active, avoiding doubled playback; MODE STEP replays/edits them. REC again can cancel a waiting take or finish early. Stop, mode changes and bank changes seal or cancel safely. Full capacity or occupied patterns produce refusal without overwriting existing music.

`recording.c`, `seq.c`, `ui_tess_features.c` and `ui_input.c` connect capture to recording admission, history and transport. The timed-note release path also checks that an arpeggiator note is nonzero before treating it as an owner, so its idle sentinel cannot block MIDI note 0 release. Update 4 adapted this to upstream's continuous STEP REC and whole-take history: generator capture remains one pass; STEP continuous REC remains upstream behaviour. The current whole-take undo gesture is hold EDIT, with EDIT+OCT-/OCT+ for undo/redo; SAVE hold performs saving. Do not restore the old .3 SAVE-hold undo mapping during a port.

Queued empty-pattern shortcuts now validate/record the queued destination rather than the bank that is still sounding. Destructive edits and history handling respect upstream's deferred-edit boundary. Performance 6's open held notes and punch-out sealing are described in section 10.

### 7.3 Save retry

Feature 3 corrected a sidecar save path that marked state saved even when the flash write failed, preventing a later retry. Saved hashes advance only after successful commit. The current unified-project path retains the invariant; `tessera_store_save` in `tess_project.c` now records the live hash after the successful project commit rather than performing a separate normal sidecar write. [E3-E4]

## 8. Update 4: Melodee 1.4.1 integration and storage collision

### 8.1 Upstream changes retained

Update 4 imports the pinned 1.4.1 base, including the new fonts/layouts, mixer/filter/TONE and FX racks, SID models/kits, shared presets, one-to-four tracks, duplex USB audio, MIDI input filtering, scheduling/resource improvements and stock V15 supply startup. These are upstream changes, not studio inventions. The 1.4.1 comparison also included its installer restore-backup check correction. Upstream's CZ-1 sample-buffer type/bounds correction resolved the previously recorded host failure; the .4 suite passed all 109 jobs.

The upstream supply sequence runs before CPU1/peripherals and checks register readback. A failure keeps CPU1 held. Nominal source selections are SYSVDD 1.38 V and VDC14 1.60 V; these are not measurements of this device or a proven crash cure. The system PLL was not changed by our integration. Existing CPU1 retirement/boot protections remain.

### 8.2 Physical flash and protocol collision

The old TSR1/TSR3 generator sidecar occupied `0x9D000..0x9EFFF`. Melodee 1.4's final shared-preset A/B bank uses exactly that pair. Old custom backup object 28 also collides with upstream's final preset-bank object. Simply keeping both implementations, or initialising presets first, would risk destroying the only saved generator metadata.

The solution is not another flash allocation. Full projects now use `FBKT` within the existing 27,904-byte A/B project record. `pattern_store.c` keeps all 32 banks (eight per track), 64 steps per track and the 1,024-event recording pool. Timing entries shrink from four bytes to three using bounded bit fields; chain and motion bank tags become three-bit entries. These changes reclaim 112 bytes. The 180-byte Tess prefix fits the reserved FUN22 gap, and the feature extension fits the reclaimed bank tail. Marker, size, bounds and checksums are validated.

`bank_timing_get/put`, `bank_tag_get/put`, `bank_upgrade_layout`, `bank_tess_set`, `bank_pack`, `bank_valid` and `bank_restore` handle old/new layouts explicitly. `project.c` snapshots custom state, validates it, commits it with music, restores it after reset and includes it in dirty/autosave decisions. Tess settings and music can no longer disagree because of two separately committed normal saves.

### 8.3 Migration ordering and failure policy

`project.c` calls `tess_migrate_legacy` before shared-preset startup. `tess_project.c` recognises TSR1/TSR3, validates every tagged block before the first write, expands older blocks with defaults and compares tags against the corresponding saved project. Stale/nonmatching tags are ignored. Matching projects are repacked as FBKT with their custom state and committed through the existing A/B machinery.

If any conversion fails, preset startup is blocked and the last committed sidecar is retained. The next boot retries; already converted projects are skipped because the old tag no longer matches their new record. Only after every matching project commits is the legacy sector pair released/wiped for shared presets. A completed conversion is idempotent. Never reorder this as a cleanup convenience.

Forward imports cover old FBKK/FUN20 and upstream FBKK/FUN22 projects, TSR1 settings and full .3 TSR3 settings. Plain upstream projects default generators/harmony/sequence MIDI OFF. A normal new save uses no separate physical sidecar; a sidecar-shaped object is still synthesised for archive compatibility.

### 8.4 Backup namespace and lifecycle

Current full backup enumeration contains 31 objects: upstream IDs 0..28 plus custom IDs 30 and 31; ID 29 is unused in that layout. Object 30 is the synthesised tagged saved-project custom state, and 31 is live custom state. `editor_backup.c` and `web/fm1backup.js` translate old Tess 28/29 objects to 30/31. The web restore rejects incompatible upstream destinations before writing, and live custom state restores after the runtime project.

Track removal in `tracks.c` shifts generator, harmony, routing and later performance settings with surviving tracks, updates FOLLOW references and clears runtime ownership. New tracks reset their own custom settings. Rename preserves them. HMF2 track removal now decodes/shifts/re-encodes instead of relying on old HMF1 byte offsets.

Older firmware cannot read FBKT/unified preset banks. Flashing an older binary alone does not undo migration. Preserve and restore a complete pre-update backup for rollback. The same principle applies to HMF2 saves in section 12. [E4]

## 9. Audio Hardening 5: confirmed defect versus defensive protection

### 9.1 Reported symptom

The user reported FM6 distortion with Echo, then dry, artifacts in the tail after STOP, and displayed CPU above 60% with one sequence track. Later discussion isolated the intermittent sound to FM6 Bells; it did not recur after a power cycle. The exact causal chain was not reproduced in host simulation. One track can have several voices and long release tails; displayed CPU alone does not establish missed deadlines.

### 9.2 Rejected jobs merging stale PCM: reproduced and fixed

In `audio_batch.h`, the executor rejected corrupt descriptors, but the CPU0/serial merge still consumed the descriptor's old PCM and unchecked length. A rejected valid-length job could repeat stale samples. A corrupt length could overrun PCM/output bounds. This affected CPU0 jobs, serial fallback in dual core and the single-core diagnostic.

`audio_batch_worker` clears `valid` before validation and sets it only after successful execution. `audio_batch_merge` requires valid execution and independently rechecks nonzero `n <= CTL`, destination and guard before mixing. `audio_batch_guard` now includes the output pointer. `audio_batch_push` rejects null function/data/output pointers, zero or oversized lengths, invalid profiling IDs and payloads larger than `AUDIO_BATCH_BYTES` before copying.

The descriptor guard is an integrity check, not proof an arbitrary pointer is mapped safely. The important fixed path is that rejected descriptors never merge stale data or unchecked lengths. Worker failure still retires/drops without replaying advanced synth state. Scratch ownership, join and ordered merge remain intact.

`tests/audio_batch_safety_test.c` demonstrates that the old rejected-job path modified untouched output and the fixed path does not. It also covers damaged lengths, IDs, function/output pointer integrity, oversized submissions, fallback and CPU1 rejection.

### 9.3 Non-finite FM6 output

`eng_fm6.c:fm6_output_clip` maps NaN sums to zero before float-to-integer conversion. NaN previously bypassed ordinary comparisons and could reach undefined conversion. Infinities remain clipped to bounds; finite samples retain their previous clipping and rounding. This is defensive containment, not evidence that a normal preset produces NaN, and does not repair arbitrary corrupted operator state.

### 9.4 Tail coverage and scope

`tests/fm6_audio_tail_test.c` runs 18 scenarios: three FM modes, dry/Echo/Reverb and sparse/dense retriggers, eight seconds each with real transport STOP. It checks bounded output, voice release, dry settling, retained wet tails and exact serial/concurrent PCM. The tail and fault suites pass undefined-behaviour/float-cast checks; the complete .5 suite passed 111 jobs.

No polyphony reduction, new voice budget, Echo redesign, clock/supply/boot change or storage change was introduced in this hardening pass. Treat the scheduler defect as fixed in software and the hardware Bells cause as unresolved. [E5]

## 10. Performance 6: independent duration, release and capture

### 10.1 Held-key modes

`performance_t` adds per-track duration mode, fixed length and overlap policy. STEP retains the original DIV/GATE behaviour. FIXED selects 1/32, 1/16, 1/8, 1/4, 1/2 or one bar independently of generator DIV/density. HIT holds until the next actual generated hit; empty steps and dropped hits do not release it. HIT ignores LEN and KEYS.

FIXED defaults to OVER, with up to four independently held pitches per track. REPL ends previous held keys at each hit. Same-pitch repetition ends the old owner before retriggering. If all four owners are held, the oldest is released. This bounds key ownership; audible release tails remain governed by the existing voice allocator and budgets. A MONO/LEGATO synth patch keeps its existing voice mode.

`tess_features.c:duration_run[NTRK][4]`, allocated in `.pool`, stores remaining samples, elapsed held units, fractional timing, recording index, pitch and active flag. `duration_active` avoids scanning idle tracks. `features_duration_on/release/tick/stop` implement ownership; `tessera.c` dispatches modes; `seq.c` ticks and cleans up; `pattern.c` releases owners before bank changes.

FIXED duration is calculated at onset using the current tempo. A later tempo change does not retrospectively stretch its remaining sample timer. Resolution follows the existing 32-sample control blocks. HUMAN still varies velocity, but its gate-shortening only applies in STEP. Stop, mute, mode/bank/project/track changes and panic release ownership; external-clock loss follows the existing transport-stop path.

### 10.2 MIDI and capture follow held time

The owner release sends the actual note-off, so USB MIDI held lengths match the selected duration mode. External instruments use their own release envelopes: MIDI note-off does not transmit our internal FADE.

For FIXED/HIT capture, the record stays open while the key is held. `features_duration_tick` accumulates elapsed recording units with fractional carry; `features_duration_release` finalises the held length. Open captured events use the existing held/heard flag combination; captured replay stays silent under the active generator. `features_capture_close` seals any still-held recorded events at the one-pass endpoint and detaches their recording indexes while allowing live generator keys to continue. This produces a finite take and safe whole-take undo.

Recorded STEP replay uses the captured durations. Release/fade settings remain track/sound state applied during replay, not baked audio or transmitted envelope commands. Tests cover long holds, rests, overlap, repeated pitches, capacity, note 0, lifecycle changes, MIDI pairing and punch-out without stopping generation. [E6]

## 11. Performance 6: per-voice FM AMP, TONE and transition protection

### 11.1 Configuration and gain placement

`performance.h` defines 13 byte-sized fields in this order: `mode, length, fade, overlap, amp, attack, decay, sustain, release, brightness, contour, amount, tone_decay`. Defaults are `{0,2,0,1,0,0,40,127,40,64,64,64,40}`: STEP, length 1/8, NATIVE fade, overlap enabled, AMP bypass, neutral tone macros.

Time controls use `performance_ms(v)`: zero is 0 ms; otherwise `2 + v*v` ms, so larger values mean longer times. These are performance times, not DX7 rates. `performance_voice[NTRK][NVOICE]` stores independent amp/fade/tone values and stage/down state in `.pool`. `performance_start`, `performance_move` and `performance_gain` run bounded control-rate work. Existing engine ramps interpolate gain sample by sample.

`voice.c` applies the per-voice multiplier after synthesis contribution preparation and before track effects. FM AMP is an overall ADSR for each FM6 voice; editing ADSR enables it and FM AMP MODE can bypass it without discarding settings. Original six-operator envelopes remain available. With NATIVE fade, the multiplicative ADSR cannot outlive an already-ended native operator voice.

### 11.2 Explicit fade and native FM lifetime

FADE=0 retains native release. A nonzero value applies an outer gain fade to each released voice, including keyboard and captured STEP playback. It does not fade the whole track or erase FX buffers. Other engines keep native lifetime; the fade can shorten their release but cannot extend an engine that has ended.

FM6 is handled specially in `fm6_control_ram`: when explicit FADE is active, native operators remain keyed while the outer gain fades, so a fast native R4 cannot defeat the requested fade. At fade end the voice retires and native release bookkeeping completes. A patch that decays while held can still fall silent earlier. `voice.c` also prevents the generic ADSR from retiring a normal FM6 voice before its native lifecycle is checked; emergency stage-4 fades remain.

### 11.3 Reuse correction

`fm6_core.c:fm6_voice_t` adds `output_last`, `correction`, `correction_step` and `correction_pending`. On voice reuse, `eng_fm6.c` enables correction only when FADE or FM AMP is active and a previous voice existed. `fm6_output` makes the first new output sample meet the preceding output sample, then settles the additive correction within 128 samples. The output kernel and serial/pair/batch callers consistently receive the voice pointer.

This is bounded discontinuity protection, not a crossfade between two complete synth instances or a guarantee of globally click-free audio. It also is not proof of the cause/cure of the intermittent Bells fault. Bypass keeps existing rendering hashes unchanged.

### 11.4 Algorithm-aware FM TONE

- BRI offsets modulator level; negative softens, positive brightens.
- CNTR adjusts modulator envelope rates together; positive is slower. Carrier amplitude envelopes are excluded using the current algorithm's carrier map.
- ENV is a bipolar per-note decaying brightness-contour amount.
- DEC sets the contour time; it matters when ENV is nonzero.

BRI/CNTR/ENV display zero at raw 64. Temporary effective patch values are clamped and cached; contour participates in cache invalidation. Original native patch bytes are preserved. Algorithms with no modulators have limited/no response. These are FM macros, not a conventional resonant filter. `eng_fm6.c` derives effective controls and per-note tone decay; `fm6_core.c` retains the native engine representation. [E6]

## 12. Current persistence formats and compatibility contracts

### 12.1 HMF2 in the existing project allocation

The full custom block remains 296 bytes: original 180-byte Tess prefix plus a 116-byte feature extension. Feature 3/Update 4 used HMF1; Performance 6 writes HMF2. No flash sectors, project banks, note-event capacity or preset slots were taken for the new controls.

`features_encode/decode` use the `HMF` header with binary version byte 2. The following widths describe fields in declaration order:

| Record | Bit widths | Total |
| --- | --- | --- |
| Four harmony tracks | `2,7,7,7,7,7,1,4` | 42 bits each |
| Sixteen SONG rows | `4,4,4,5,5` | 22 bits each |
| Four performance tracks | `2,3,7,1,1,7,7,7,7,7,7,7,7` | 70 bits each |

The encoded payload occupies 104 bytes including the four-byte header; the remaining 12 bytes are zero. Decoder validation rejects nonzero reserved bits and out-of-range enums/rows, checks low<=high and imports HMF1 with performance bypass defaults. Preserve the actual bit arrays and order; the source's fit assertion is conservative and is not the wire layout specification.

`features_packed_remove` decodes the feature records, shifts surviving tracks, resets the vacated slot, updates FOLLOW references and re-encodes. Do not retain byte-offset copies from the HMF1 layout. `tess_full_valid/unpack` accepts legacy 180-byte or full 296-byte custom blocks through the appropriate reset/import lifecycle.

### 12.2 FM6 shared-sound extension

FM6 shared sound payloads retain `UP_VER_FM == 11` and 144-byte size. The previously unused 16-byte tail at offsets 128..143 now carries an SP version-1 extension: bytes 0/1 of the tail are `S/P`, byte 2 is version 1, byte 3 is FADE, bytes 4..12 are the nine fields from AMP through tone_decay. Remaining tail bytes are zero in newly packed payloads. Legacy payloads without the recognised marker load with sound controls reset.

`performance_sound_pack/valid/apply/reset`, `upreset.c` and `shared_presets.c` implement this. FM sound load preserves generator duration/length/overlap; factory sounds reset only the sound controls. Sound undo/redo includes the extension, rename retains it, and native patch updates preserve the existing 16 tail bytes. Plain DX7 export remains 128 native bytes and excludes these macros. Other engine sound presets do not store FADE; projects do.

### 12.3 Compatibility direction

| From / to | Supported behaviour |
| --- | --- |
| Plain upstream or .1/.2 saves into .6 | Forward imports with newer features defaulted off/bypassed as appropriate; tagged legacy migration runs before presets. |
| .3 TSR3/HMF1 into .6 | Existing generator/harmony/MIDI/SONG state retained; performance defaults bypassed. |
| .4/.5 HMF1 project into .6 | Readable; SAVE commits HMF2. Installing .6 does not rewrite every saved project merely to add HMF2. |
| .6 HMF2 project into .4/.5 | Unsupported. Restore the retained pre-install backup with the earlier firmware for rollback. |
| New FM sound on older firmware | Older firmware may ignore/drop the extension. Do not promise reverse macro compatibility. |

The included backup/editor code preserves complete project/archive payloads, but its external patch editor has no new FM macro controls. Format import compatibility does not imply reverse compatibility or that an old editor can expose the new parameters. [E3-E6]

## 13. Functional UI hooks and support changes

New feature pages are necessary controls for the added behaviour: HARMONY, NOTE RANGE, SONG CHORD, SONG KEY, OUTPUT, NOTE LENGTH, FM AMP, FM AMP MODE and FM TONE. `params.c` registers graph enums; `ui.c` selects feature pages, names/status, FM ENV-family pages, defaults and sound history; `ui_tess_features.c` publishes bounded edits under interrupts; `ui_graph.c`/`ui_draw.c` draw their values. Harmony panels invalidate on relevant content changes. `ui_input.c` routes controls and recording shortcuts; `ui_patterns.c` incorporates SONG bar durations.

Existing fonts, HOME, mixer/filter, circle of fifths, FX rack and broad navigation remain as inherited in .4. The separate HOME/menu/navigation redesign is deferred. Small cumulative hooks also exist in `ui_layer.c` (release generator keys immediately on quick-layer mute), `ui_pages.c` (generator preview and redraw signature), `ui_menu.c` (Tess attribution) and `console.c` (fork identity). These do not constitute implementation of the later UI backlog.

`tools/build.py` permits `MELODEE_DUAL_CORE=0` only with the explicit private `STUDIO_SINGLE_DIAGNOSTIC=1` gate. `tools/size_fns.py` tracks relevant custom functions for target reporting. Test-runner, hostsim, installer labels and backup web tests were updated with the corresponding firmware interfaces. The archive was later repackaged to address an extraction failure; the repacked ZIP and TAR carry the same Performance 6 source/binary, not a new firmware revision. [E1-E6]

## 14. Verification, budgets and remaining hardware work

### 14.1 Evidence progression

| Delivery | Recorded host/web result | Qualification |
| --- | --- | --- |
| Integration 1 | Generator/storage/backup/build checks passed after setup fixes | Pre-existing upstream CZ-1 failure documented. |
| Stability 2 | 96/97 jobs | Same CZ-1 failure; four reviewed Euclid pitch baselines updated. |
| Feature 3 | 98/99 jobs | Same CZ-1 failure; new features off preserve all 28 current generator baselines. |
| Update 4 | 109/109 jobs | Upstream CZ-1 correction imported; migration fixtures/failure injection passed. |
| Audio Hardening 5 | 111/111 jobs | Added descriptor fault and FM6 STOP/tail coverage. |
| Performance 6 | 114/114 jobs | Duration/FM controls, UI/sound persistence and enabled-feature stress added. |

Performance 6 preserves all 337 existing sound-render hashes in bypass. Default four-generator stress retains 694 MIDI packets; the enabled-performance variant yields 628, and each run's serial/concurrent PCM, timestamps and full saved state match exactly. Each stress run uses 16,384 blocks, four generators, two Prophet and two FM6 tracks, FX, capture and 512 live edits; the performance variant additionally exercises new controls. These are approximately 11.9 simulated seconds per dispatch mode, not hours of board operation.

ASan, UBSan and float-cast-overflow checks passed for the targeted suites. Leak detection was disabled for runtime compatibility. Storage checks cover real .2/.3 NOR fixtures, all 32 banks, timing/notes, legacy namespace translation, track lifecycle, rename, erase/page/header write failures, retry and idempotent migration. UI lint reports zero findings; the four new performance pages were visually reviewed. Optional external-reference checks remain conditional on their dependencies.

### 14.2 Exact current build facts

The archived manifest enables `MELODEE_RAM_AUDIO=1`, dual core and `MELODEE_CORE1_TEST=0`. Some older notes describe nonresident targets; use the current manifest/log, not those historical flags, to reproduce .6. The resident profile and its existing voice-budget policy are inherited; this feature pass did not introduce a new polyphony budget.

The final dual build log reports application image 545,356 bytes; `.data + .bss` 97,664 of 98,304 bytes; permanent pool 167,820 of 344,064 bytes; and 51,616 bytes of on-demand audio arena against a 49,152-byte enforced minimum. Ordinary data SRAM has only 640 bytes left. Future snapshots/state should not be casually added as static ordinary data. Respect linker region and audio-arena checks separately.

Both main and diagnostic packages are 610,534 bytes. The offline install/package checks passed. Entry/boot-sector bytes and the non-flash OTA loader entry are unchanged from Audio5. This is package evidence, not authorisation or a need to redesign recovery. The matching ELF/disassembly is required for crash PCs; optimised symbols provide function/offset information, not source-line debug.

### 14.3 Unresolved reports and next measurements

The earlier red-screen crashes and separate Prophet preset freeze remain unexplained. The confirmed audio descriptor defect was not demonstrated to cause the physical Bells distortion. Fade/reuse correction addresses transition continuity but does not establish an intermittent hardware cure. The user's positive .6 feedback should be retained as hands-on feedback, not upgraded to a completed acceptance matrix.

If artifacts recur, capture counters before a power cycle, along with patch, FX, duration/fade/FM settings, active voices, power source and exact actions:

```bash
python3 -u source/tools/usb_audio_stats.py --complete --seconds 30 --interval 1 > fm1-audio-report.jsonl
```

Run from the extracted bundle with installer dependencies available and other editor/MIDI connections closed. Play for roughly 15 seconds, then STOP and let the tail complete. Inspect counter deltas for deadline/DMA misses, CPU1 state, rejects/faults/timeouts, overload cuts and memory failures. Maxima are since boot and may predate the test. Compare the same project on USB/battery or diagnostic single core one condition at a time. Keep audio and a full crash-screen capture where available. [E2-E6]

## 15. Claude Code starting procedure and porting guidance

### 15.1 Establish the baseline

Read `AGENTS.md`, `BUILDING.md`, `LICENSING.md`, the six implementation notes in the evidence index, then this handoff. Preserve the Performance 6 snapshot unchanged as a comparison point. Work on a branch and record which donor/upstream revision receives each port. Inspect include order and actual interfaces before copying static functions.

Use the existing .6 bundle's `audio5-to-performance6.patch` only when the baseline is its exact delivered Audio5 source. The archived patch check passed against `rollback/source-audio5.tar.gz`. To review the complete fork, compare against pinned Melodee 1.4.1 or the original Tess donor; a diff against Audio5 omits earlier safety, MIDI, recording and migration changes.

### 15.2 Reproduce builds and tests

Configure the JieLi pi32v2 toolchain and AC79 SDK as in `BUILDING.md`; `JIELI_TOOLCHAIN` and `AC79_SDK` override their defaults. On Linux the toolchain runs natively; the documented macOS path uses Docker. Run from the source root:

```bash
MELODEE_RAM_AUDIO=1 ./build.sh --release 1.4.1-studio-tess.6
MELODEE_RAM_AUDIO=1 PARALLEL=4 bash tests/run_tests.sh
```

Diagnostic only:

```bash
MELODEE_RAM_AUDIO=1 MELODEE_DUAL_CORE=0 STUDIO_SINGLE_DIAGNOSTIC=1 ./build.sh --release 1.4.1-studio-tess.6-single
```

Use the suite's registered tests rather than guessing a standalone compilation recipe for the monolithic firmware. Keep sound/CPU/generator baselines unchanged unless a reviewed intended behaviour change explains a specific difference. Never use blanket `GOLDEN_UPDATE` or `BUDGET_UPDATE` to conceal a failure. Static target budgets are necessary but do not replace device measurements.

### 15.3 Suggested port order

1. Port pure validators, bounded edit/mutation/recall fixes and their tests first; adapt IRQ primitives to the receiving architecture.
2. Port MIDI ownership and capture against the receiver's current USB/recording/history interfaces. Preserve note 0, congestion, shared-pitch and lifecycle tests.
3. Port harmony and duration with features bypassed by default. Confirm identical baseline RNG/audio when disabled.
4. Design persistence against the receiver's actual flash map and backup namespace before enabling new writes. Tess's current upstream branch may differ from both historical and current studio layouts.
5. Port per-voice FM performance and audio hardening only with the receiving engine/batch interfaces checked. Preserve worker join/no-replay semantics and native FM lifetime.
6. Add drum/morphing work on a later branch after snapshot/seed storage, simultaneous drum events, discrete transitions and budget plans are explicit.

Pure generator safeguards are the most portable changes. FBKT migration, shared-preset SP extension, resident audio placement and FM6 sample correction are Melodee-version-specific. Avoid blindly cherry-picking all integration commits into Tess; the donor history and its preferred engine set may be different.

### 15.4 Invariants to retain

- Generator/control mutation stays on CPU0 with bounded critical sections; CPU1 renders only submitted/joined sample work.
- Rejected jobs never merge; failed advanced voices are not rerendered as fallback.
- Validation has no live-state side effects; all input is validated before destructive writes.
- Legacy Tess sectors remain intact until every matching migration commits; startup order is part of data safety.
- Held keys, audible release and recording boundaries remain separate. MIDI offs track actual accepted ons and drain before new notes.
- Project saves retain all banks, notes, timing and custom state atomically. New fields require versioned imports and explicit rollback rules.
- Neutral settings preserve existing sound/RNG behaviour. Synth patch bytes are not cumulatively rewritten by macros.
- Feature additions do not remove Melodee engines, original operator editing or ordinary STEP/continuous recording.

### 15.5 Attribution and licence

Follow the source's `AGENTS.md`: new Melodee code is credited to Kerem Kilic (Ellic Studio); retain Theo Dainis's Tess notices and all applicable third-party notices. Mixed files retain the relevant Felucca credit to Leo Kuroshita (@kurogedelic), Hügelton Instruments. Preserve GPL-3.0-only SPDX notices and the distinct licences of bundled dependencies/SDK components. Authorship must follow actual provenance, not a copied header or a shared licence.

## 16. Evidence index and recommended reading

The following references are shipped source/release records, not claims that tests were rerun while writing this document.

| Ref | Source record | Use |
| --- | --- | --- |
| E1 | `source/STUDIO_INTEGRATION.md` | Initial inputs, generator merge, restore-header fix, original validation/limits. |
| E2 | `source/STUDIO_STABILITY_UPDATE.md` | Exact hardening changes, Tess donor fix references, baseline exceptions, runtime validation. |
| E3 | `source/STUDIO_FEATURE_UPDATE.md` | Harmony/SONG/MIDI/REC behaviour, HMF1/TSR3, retry and feature validation. |
| E4 | `source/STUDIO_UPDATE_4.md` | Melodee 1.4.1 merge, FBKT, collision/migration, backup IDs and upstream changes. |
| E5 | `source/AUDIO_HARDENING_5.md` | Confirmed descriptor failure, defensive FM guard, tail tests and diagnostic method. |
| E6 | `source/STUDIO_PERFORMANCE_6.md`; bundle `SOURCE_COMMIT.txt`, `manifest.json`, `validation/` | Current behaviour, formats, exact .6 build and validation records. |
| E7 | Canonical `FM1_Studio_Tess_Roadmap.md`, version 3, 16 Oct | Current approved drum/morphing order and deferred MPE/UI status. Earlier dated unchecked requests are superseded by its Performance 6 implementation section. |

Within `validation/`, start with `summary.json`, `host-suite.log`, `dual-build.log`, `single-diagnostic-build.log`, `address-undefined-float-sanitizer.log`, `package-and-boot.json`, `offline-install-check.log`, `patch-check.txt`, `preset-native-update-and-rename.log`, `target-budget.log` and the individual `host-jobs/` logs. The bundle also contains `CONTROLS_AND_TESTING.md`, `INSTALL.md`, the Audio5 rollback source/binary and matching diagnostic symbols.

The Performance 6 TAR archive SHA-256 is `3b030b1feadffb45911cc5bbf41a7a2e782fd7668428f43a1c086fcc0d7873ec`. The repacked ZIP SHA-256 is `98a97b2e70f7f55b301863a2c2146a562b69f5bbbb5b2ffe3ee28604f3b9370f`. Use each archive's `SHA256SUMS` to verify its contents.

## Appendix A. Cumulative firmware file inventory

This inventory compares the delivered Performance 6 source bytes with pinned Melodee 1.4.1 (`1cccd697`). ADD means absent from that upstream snapshot; MOD means different. All paths in this table are under `firmware/src/`. Function locators in the technical sections are preferred over fixed line numbers when porting.

| File / status | Custom responsibility |
| --- | --- |
| `audio_batch.h` MOD | Descriptor execution validity, pointer/length guard and safe merge. |
| `console.c` MOD | Fork identity in console status/banner. |
| `core.h` MOD | Performance declarations included in shared firmware context. |
| `editor_backup.c` MOD | Custom saved/live state objects, validation, restore ordering and legacy handling. |
| `eng_fm6.c` MOD | Non-finite clipping, AMP/FADE-aware native lifecycle, derived TONE and reuse correction. |
| `fm6_core.c` MOD | Per-FM-voice output continuity fields. |
| `melodee.c` MOD | Fork/version identity and generator/feature include wiring. |
| `params.c` MOD | Generator/feature graph identifiers and page integration. |
| `pattern.c` MOD | Duration-owner release at bank transition. |
| `pattern_store.c` MOD | FBKT compact bank layout, feature block packing/import and checksums. |
| `performance.c` ADD | Per-voice control-rate amplitude/fade implementation. |
| `performance.h` ADD | Configuration/state, time curve and FM sound-extension helpers. |
| `project.c` MOD | Custom snapshot/load/dirty/hash handling; migration and save integration. |
| `recording.c` MOD | Sequence MIDI hooks for timed-note lifecycle/replay. |
| `seq.c` MOD | Generator dispatch, capture/REC lifecycle, MIDI, duration ticks and cleanup. |
| `shared_presets.c` MOD | Preserve FM extension during native patch updates. |
| `song_chain.c` MOD | Explicit harmonic row bar timing and carry. |
| `tess_features.c` ADD | Harmony, MIDI, capture, duration and feature wire formats. |
| `tess_features.h` ADD | Feature types, constants, state declarations and internal interfaces. |
| `tess_project.c` ADD | Tagged archive representation and protected legacy sidecar migration. |
| `tessera.c` ADD | Tess generators plus studio edit, mutation, validation and dispatch safeguards. |
| `tessera.h` ADD | Tess state/types/public generator interfaces. |
| `tracks.c` MOD | Shift/reset custom state and FOLLOW references on track lifecycle. |
| `ui.c` MOD | Feature page selection/status, FM pages, defaults and sound history. |
| `ui_draw.c` MOD | Generator/feature values and status drawing. |
| `ui_graph.c` MOD | Generator and feature graph rendering. |
| `ui_input.c` MOD | Generator controls, recording admission/shortcuts and lifecycle integration. |
| `ui_layer.c` MOD | Immediate generator key release on quick-layer mute. |
| `ui_menu.c` MOD | Tess/fork attribution in credits. |
| `ui_pages.c` MOD | Tess preview rendering and state-based redraw signature. |
| `ui_patterns.c` MOD | Effective harmonic SONG duration display. |
| `ui_tess_features.c` ADD | IRQ-safe harmony, routing, duration and FM control handlers; REC checks. |
| `upreset.c` MOD | FM shared-sound extension pack/load. |
| `usb.c` MOD | Nonblocking MIDI output admission. |
| `voice.c` MOD | Performance start/gain and normal FM6 lifetime handling. |

## Appendix B. Tests and supporting files to inspect with each port

| Files | Coverage / purpose |
| --- | --- |
| `tests/tessera_test.c`, `tests/tessera_baseline.txt` | 28 musical scenarios, transport/clock/mute, saved bytes and note release. |
| `tests/tessera_hardening_test.c` | Interrupt-edit model, delta bounds, mutation/FORM and validation purity. |
| `tests/tess_features_test.c`, `tests/tess_features_ui_test.c` | Harmony, SONG, MIDI lifecycle, capture, controls, undo and overwrite refusal. |
| `tests/tessera_dual_stress_test.c` | Serial/concurrent audio/state/MIDI equality, including enabled performance variant. |
| `tests/studio_performance_test.c`, `tests/performance_ui_test.c` | Duration/fade/FM controls, recording, sound/project persistence and functional controls. |
| `tests/audio_batch_safety_test.c`, `tests/fm6_audio_tail_test.c` | Descriptor fault injection, NaN/finite output and real STOP/FX tails. |
| `tests/studio_storage_test.c`, `tests/studio_upgrade_check.py`, `tests/fixtures/` | Real legacy NOR migration, failed-write retry, all banks, rename and track state. |
| `tests/backup_test.c`, `tests/storage_test.c`, `web/test_backup.mjs` | Archive validation, compatible import, namespace collision and save behaviour. |
| `tests/upgrade_test.c`, `tests/upgrade_check.sh` | Earlier upgrade-format fixtures/checks. |
| `tests/dual_core_test.c`, `tests/hostsim.c` | Worker contract and simulated generator/MIDI instrumentation. |
| `tests/ui_test.c`, `tests/ui_render.c`, `tests/editor_test.c`, `tests/motion_test.c` | Page integration, rendering and editor/history compatibility. |
| `tests/run_tests.sh` | Authoritative build flags and registered test jobs. |
| `web/fm1backup.js`, `web/index_pkg.html` | Backup/restore compatibility and custom package labels. |
| `tools/build.py`, `tools/size_fns.py` | Diagnostic single-core gate and compiled-function reporting. |

## Appendix C. Roadmap implementation boundaries

The next drum phase should extend the existing DRUM track's eight lanes, not consume all four tracks as separate percussion instruments. Preserve user anchor hits and normal editable patterns. Simultaneous lane events, distinct velocities, choke behaviour, one-pass capture and MIDI representation need explicit tests; the present monophonic melodic generator path is not already a complete kit generator.

The first sound-morphing phase should derive temporary effective values from protected snapshots. Locks must bound or exclude pitch, tuning, volume, algorithm and release where requested. Continuous values need smoothing and musical interpolation; envelope-time updates must not repeatedly restart sounding envelopes. Discrete topology, waveform and FX changes require explicit switch boundaries and tail policy. Returning to FORM A should restore its identity rather than accumulate random edits.

Before adding either phase, design versioned snapshot/seed persistence against the existing tightly allocated project. Retain Performance 6 as the comparison baseline, including neutral hashes, duration/MIDI/REC contracts and storage failure tests. MPE remains a later phase; the earlier generic Tess roadmap is not authorisation to remove engines or expand into unrelated work. [E7]
