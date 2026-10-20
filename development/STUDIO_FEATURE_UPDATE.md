# Studio Tess feature update 3

FM-1 hardware test candidate, 15 October 2026.
About: **v1.3.5-studio-tess.3**. Main build: dual core.

Based on our `.2` stability build (`64054b3`), including its interrupt protection,
note-release fixes and mutation safeguards. The feature implementation is
`932cbff`; final UI refinements are `defcafd`. This is a custom test build,
not an official Melodee or Tess release. No firmware was flashed here.

## Added behaviour

### Harmony

SEQ gains **HARMONY** and **NOTE RANGE** pages. HARMONY's four knobs are:

| Knob | Control | Behaviour |
| --- | --- | --- |
| 1 | HARM | OFF / KEY / SONG |
| 2 | OUTSIDE | 0–100% probability of choosing a note outside the scale |
| 3 | CHORD | 0–100% preference for current chord tones |
| 4 | RESOLVE | 0–100% chance that an outside note is followed by an inside note |

OFF retains `.2` generation and its random sequence. KEY uses existing SCL ROOT,
SCALE and CHORD selection. SONG uses the active arrangement row's harmony; before
starting the arrangement it previews the selected harmony row.

NOTE RANGE's knobs are LOW, HIGH, ROOT and SCALE. LOW/HIGH constrain final pitches
when harmony is enabled. ROOT remains track-specific; SCALE is shared across
tracks, following Melodee's existing behaviour. Pitch selection happens after
FOLLOW, SHIFT, flourish and transpose, so those controls cannot bypass an enabled
zero-OUTSIDE constraint.

Generation chooses weighted notes in the requested range rather than quantizing
every outside choice back into the scale. CHORD bias selects chord tones within
the chosen inside/outside category. If CHORD is OFF, a diatonic triad of ROOT is
used for the bias. Harmony changes favour notes close to the previously played
pitch. This is weighted voice leading, not a guarantee of the nearest note.

RESOLVE can override OUTSIDE on the following note. A range with no candidates in
the requested category falls back to available notes within the range; e.g. a
single C cannot supply an outside note in C major. Chromatic has no outside notes.
Short phrases will not contain an exact percentage of outside notes.

This first build supports the 16 conventional 12-tone scales for harmony.
Extended microtonal layouts retain their existing generation with HARM OFF.
Selecting such a layout disables harmony and reports `HARM OFF: 12T ONLY`;
trying to enable it requests `USE 12T SCALE`. Their chromatic alternatives need
separate pitch handling. This is an explicit limitation of the test build.

### SONG harmony and durations

Create rows using the existing SONG page. New pages:

| Page | Knob 1 | Knob 2 | Knob 3 | Knob 4 |
| --- | --- | --- | --- | --- |
| SONG CHORD | ROW | chord ROOT (or KEY) | CHORD quality (or TRACK) | BARS |
| SONG KEY | ROW | KEY root (or TRACK) | SCALE (or TRACK) | SONG reminder, read-only |

Up to 16 existing song rows have their own chord root/quality, optional key/scale
overrides and duration. BARS PAT (zero) retains original pattern-based timing.
1–16 sets an explicit musical bar duration. Existing row repeats multiply that
duration. Rows advance at the selected tempo and the arrangement stops at its end.
Edit rows while stopped. Return to SONG and press PLAY to start the arrangement;
PLAY on a generator page remains normal pattern playback.

Each track independently chooses OFF, KEY or SONG. Chord preferences can change
without changing key. Explicit key/scale overrides permit modulation. The key
diagram shows the effective scale and outlines chord tones. This guides a
monophonic line; it does not generate polyphonic chord events.

### USB MIDI sequencing

SEQ > OUTPUT: MIDI OFF / USB, CHAN 1–16. OFF is the default for older projects;
channels initially correspond to tracks 1–4. USB enables generator and normal
STEP/timed-recording notes alongside the internal sound. Final pitches, velocities
and releases are sent, including DROP, HUMAN, FOLLOW and harmony results.

Existing keyboard USB MIDI output remains on channels 1–4 independently of this
new switch. Sequence channels can be set to 5–8 if a separate keyboard route is
desired. The 3.5 mm connector remains input-only. USB requires a host: a computer
and routing/interface, or a standalone USB MIDI host. No Bluetooth or MIDI clock
output is added in this build.

The output tracks notes actually queued. On queue exhaustion, new notes may be
dropped while required note-offs remain pending and are retried with priority.
Channel changes, output-off, Stop and project changes release old notes. Shared
channel/pitch ownership across sequencer tracks is respected. This does not
replace a host's MIDI routing or prevent a DAW feedback loop.

### One-pass generator REC

Select an **empty pattern** on the selected generator track, then press REC.
While stopped, REC arms the take and PLAY starts it. During playback, it waits
for that track's next loop start. It records one complete pattern-length pass,
then automatically stops recording while generator playback continues.

Capture stores the final played notes, onset positions, velocities and gates,
after harmony, FOLLOW, SHIFT, DROP and HUMAN, using the existing timed-event
sequencer store. No MIDI loopback is involved. Captured events stay silent while
the generator is active, avoiding doubled playback. Choose MODE STEP to replay
and edit the captured phrase. Current compact timing precision is retained.

REC again cancels a waiting take or finishes an active one early. Stop ends a
take. Changing generator MODE seals a take; switching banks cancels it safely.
SAVE held undoes capture; the existing redo restores it while its history remains
available. Occupied patterns are refused with `USE EMPTY PATTERN`, and insufficient
event capacity is refused without overwriting other work.

Capture currently uses normal pattern transport. Stop the SONG arrangement and
return to generator pages before recording. SONG mode can still preview the
selected harmony row in normal playback. Live key/harmony edits during a take
are recorded as heard. Recording across a running arrangement is a later extension.

## Persistence and compatibility

- Existing project and 32-bank sequencer formats are unchanged.
- Original generator settings remain the first 180 bytes of the sidecar block.
- A 116-byte HMF1 extension stores four harmony/range/MIDI configurations and
  16 harmonic rows. Combined live block: 296 bytes.
- Saved object TSR3: four tagged slots, total 1,204 bytes, in the existing A/B
  sector pair at 0x9D000/0x9E000. No flash layout or bootloader changes.
- TSR1 objects and 180-byte live backups import with harmony and sequence MIDI
  OFF. Migration is in memory until a save; archive validation does not mutate
  the live generator state.
- Backup objects 28 and 29 carry full new state. Use the included Tess-aware
  backup/editor source. Invalid lengths and values are rejected before writes.
- Failed sidecar saves remain dirty so a subsequent save can retry.

**Keep a `.2` backup before installing.** `.2` cannot read TSR3 harmony/generator
sidecars. It can still read the normal project notes, including captured notes,
but it will not restore the new settings. Restore the old backup for rollback;
do not save over `.3` projects using older firmware if you need those settings.

## Verification

- Full host/web suite: 99 jobs, 98 passed. The sole failure is the pre-existing
  CZ-1 determinism/dim-control job also present in `.2`. No sound-engine golden,
  CPU golden or Tess note baseline was rewritten.
- All 28 existing generator scenarios pass with new features OFF.
- Feature tests cover all 16 conventional scales and 12 roots, inside/outside
  extremes, resolution, chord bias, range fallback, key overrides, explicit
  SONG bar changes, MIDI queue/release/channel behaviour and one-pass capture
  through all four generators. Stop, bank changes, note 0 and full capacity are
  covered. Capture replays through STEP without stacking repeated passes.
- UI tests cover the five new pages, recording protection and capture undo/redo.
- Feature and backup suites pass AddressSanitizer and UndefinedBehaviorSanitizer;
  leak detection is disabled because it is unsupported in this execution environment.
- A `.2` NOR save fixture loads with its generator settings intact and new
  features off. Same-build save/autosave/backup tests cover full new settings,
  malformed archives, legacy import and failed-write retry.
- Concurrent audio test: 16,384 blocks (about 11.9 simulated seconds) with four
  harmony-enabled generators, two Prophet and two FM6 tracks, FX, one-pass REC
  and 512 live edits. PCM, 694 MIDI packets/timestamps and complete settings
  match serial dispatch exactly. Actual concurrent worker jobs run and all
  notes release after Stop.
- Final page rendering: 215 scenes across three palettes plus all page/value
  sweeps, zero layout findings and zero non-gray MONO pixels. New panels were
  also inspected visually. Final UI refinements have targeted rechecks.
- Dual-core and diagnostic single-core targets compile within the enforced
  RAM/flash budgets. Exact sizes and hashes are in the bundle manifest/logs.
- Final package, installer, boot-stub and target-cost checks accompany the bundle.

These are software/host checks, not measurements of FM-1 voltage, CPU headroom,
USB timing or long-session stability. The earlier red-screen crashes and separate
Prophet preset freeze are not established as resolved. Automatic CPU1 retirement
and boot guards remain; dual-core firmware can fall back after a fault.

## Hardware test order

Back up, install on stable USB power, and check About for `.3`. Verify an existing
project first with HARM and MIDI OFF. Test KEY with OUTSIDE 0, then outside/bias/
resolution and range. Test a four-row SONG progression, external MIDI, and finally
REC into an empty pattern, STEP replay, undo, save and power-cycle reload. Compare
USB and battery sessions under the camera. Record full crash numbers and active
engine, generator, harmony, MIDI, recording state, last control and power source.

Matching ELF/disassembly/functions travel with this exact `.3` dual-core binary.
They provide function/offset lookup, not source-line debugging. Older symbols
must not be used for these crash addresses. Single core is for comparison only.
