# Studio Tess update 4 — Melodee 1.4.1

Hardware test candidate, 16 October 2026. About: **v1.4.1-studio-tess.4**.
Main build: dual core. Single core is a diagnostic comparison only.

This merges pinned Melodee v1.4.1 (1cccd697) with our full .3 feature branch
(a3d9f1d). It retains upstream's new UI/fonts/palettes, FX racks and TONE, C64
SID models and kits, shared preset library, one-to-four tracks, duplex USB audio,
MIDI input channel filtering, audio/resource optimizations, and stock V15 supply
startup. It also retains all four Tessera generators, EVOLVE/FORM/TWEAK, FOLLOW,
our weighted harmony, SONG chord/key/duration controls, USB MIDI sequencing and
one-pass capture. This is a custom test build, not an upstream release.

The UI abbreviates OUTSIDE/CHORD/RESOLVE to OUT/CHRD/RES where needed. Extended
microtonal layouts appear as M1... on NOTE RANGE; use SCL to see their full names.
On OUTPUT, PASS is ONE for a generator; STEP shows LOOP for explicit continuous
REC or ONE for an armed one-pass pattern shortcut. Both are read-only status.

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
and edit the captured phrase. Current compact timing precision is retained. STEP recording retains upstream's continuous REC; generator REC is always one pass.

REC again cancels a waiting take or finishes an active one early. Stop ends a
take. Changing generator MODE seals a take; switching banks cancels it safely.
Hold EDIT to undo a whole take; EDIT+OCT-/OCT+ steps through undo/redo.
The take includes notes, timing and motion, using upstream's recording history. Occupied patterns are refused with `USE EMPTY PATTERN`, and insufficient
event capacity is refused without overwriting other work.

Capture currently uses normal pattern transport. Stop the SONG arrangement and
return to generator pages before recording. SONG mode can still preview the
selected harmony row in normal playback. Live key/harmony edits during a take
are recorded as heard. Recording across a running arrangement is a later extension.

## Atomic saves and migration

The 1.4 shared preset library uses 0x9D000/0x9E000. Our older TSR1/TSR3 object
used the same pair, so this build reads and validates it **before** preset startup.
Each matching saved project is converted with its generator/harmony state in the
same existing A/B commit as its music. Only after every matching project commits
is the old pair released to the shared preset library. Failed migration prevents
preset startup and retains the last committed sidecar for retry on reboot.

New full projects use FBKT, still exactly 27,904 bytes in the existing allocation.
No flash sector is added and the key_mac sector is untouched. Compact timing and
bank tags reclaim 112 bytes; 180 bytes of Tessera state use the FUN22 reserved
gap, and the HMF1 extension fits the reclaimed bank tail. All 32 banks, 64 steps
per track and the 1,024-event recording pool are retained. Static bounds and
checksums cover both parts. Notes and custom settings now commit atomically.

Older FBKK/FUN20 and upstream FBKK/FUN22 projects import; plain projects load
with generators/harmony/sequence MIDI OFF. TSR1 defaults the newer features OFF.
TSR3 preserves the full .3 settings. Stale sidecar tags are ignored as before.
A completed conversion is idempotent. Normal new saves do not use a sidecar.

Full backups now have 31 objects: upstream's 0..28, custom saved settings 30 and
live settings 31. The included backup code translates older Tess archives'
28/29 to 30/31 and rejects them on incompatible upstream firmware before writing.
It restores live custom state after the runtime project. Use the included editor
for backup/restore; do not use a generic upstream restore on a Tess archive.

Track removal shifts generator/harmony/routing settings and FOLLOW references
with the surviving tracks. Adding a track resets only that new track's custom
state. Project rename retains all custom settings. The empty-pattern shortcut
also targets the queued destination during playback, rather than the old bank.

**Back up before upgrading, and retain that backup unchanged.** Older firmware
cannot read FBKT or the unified preset banks. A rollback must restore the old
backup; flashing old firmware alone does not reverse migration. Leave power
connected during installation and the first-start migration.

## Power and diagnostic builds

The upstream stock V15 analog supply sequence now runs before CPU1/peripherals.
Its read-back checks keep CPU1 held after a transfer/check failure. It does not
change the system PLL. Nominal register selections are SYSVDD 1.38 V and VDC14
1.60 V; these are source values, not measurements of this FM-1.

Dual core is the normal build. A private STUDIO_SINGLE_DIAGNOSTIC=1 gate permits
MELODEE_DUAL_CORE=0 only for comparison. CORE1_TEST and RAM_AUDIO remain 0 in
both packages. Existing CPU1 fault retirement and boot guards remain.

## Validation and limits

- Full host/web suite: 109 jobs passed, including upstream's corrected CZ-1 test.
- All 28 existing Tessera note scenarios match their baseline. No existing sound, CPU or
  generator golden entry was rewritten; new SID-kit entries come from upstream.
- Real .2 and .3 NOR fixtures cold-boot and retain their generator settings.
  A four-project .3 fixture retains all harmony/range/MIDI/SONG settings.
- Injected erase and page/header write failures preserve the committed legacy
  sidecar; retry finishes conversion and subsequent boots do not repeat it.
- All 32 banks retain distinct notes, timing and SONG assignments; capture/STEP
  replay, protection, mode changes, undo/redo, track lifecycle and rename pass.
- Backup web tests cover new archives, legacy namespace translation and refusal
  on incompatible firmware before writes.
- Feature and migration suites pass ASan/UBSan, with leak detection disabled
  because this runtime does not support it.
- UI rendering covers 261 scenes across eight palettes and the page/value sweep,
  with zero lint findings and zero non-gray MONO pixels.
- Dual-core concurrent audio, MIDI lifecycle, installer, boot-stub and target
  budget checks accompany the bundle. Exact binary sizes/hashes are recorded.

These are software checks. No FM-1 was flashed here. They do not establish
physical CPU headroom, battery stability, rail behavior, USB timing or resolution
of the earlier red-screen crashes/Prophet preset freeze. Test the dual-core build
on hardware, using the single-core package only to compare a reproducible bug.

## Hardware test order

Back up and quick-save your .3 projects first. Install on stable USB power and
allow startup migration to complete. Confirm About shows .4. Load each saved
project and compare its notes and generator settings before editing. Verify
HARM/MIDI OFF playback, then harmony and SONG rows, MIDI output, and REC into an
empty pattern. Check automatic disarm, STEP replay, EDIT undo/redo, save/reload,
track add/remove and queued pattern changes. Test USB and battery sessions
separately. Record the complete crash numbers, power source, engine/generator,
harmony/MIDI/REC state and last action.
