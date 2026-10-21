# Studio Tess 8 — page and function guide

By: Inputsignwave · 21 October 2026

This guide describes the navigation candidate built from the working Performance 7.
All existing sound engines and sequencing features remain. Generator settings
are hidden when irrelevant, not erased or moved into sound editing.

## Quick navigation

```text
HOME tap: track overview / existing HOME views
HOME hold: Home menu
  Create New Track                 first item
  Sound tools                      Init / Save as user / Favourite / Undo last load
  Track actions                    Remove selected track
  Pattern actions                  Quantize / Copy to / Delete / Clear / Clear motion
  Selected notes                   Move / Copy / Quantize / Delete / Clear selection
  Song actions                     Insert / Duplicate / Delete row / Take jam / Clear
  Shared FX                        Chain or Split / Clear shared FX
  Existing settings                Colour, font, hold, lights, USB, click, preferences,
                                   calibration, About and Back
EDIT: selected engine's sound pages and sections
  Sound Designer                   DESIGNER page; turn K1 or tap OCT+ to open
  FM6: ALGORITHM encoder selects algorithms 1–32 in ordinary EDIT pages
SEQ: notes, patterns, generators, rhythm, harmony, note range, duration and song
  PATTERN: K1 chooses MODE; other knobs follow that mode
  Generator-only pages appear when applicable (table below)
GLO taps: MIXER → OUTPUT → GLOBAL → SYSTEM → MIXER
ENV / LFO / SCL / ARP / FX / SAVE: existing families and functions
```

SELECT scrolls pages; in engine EDIT sections PRESETS selects sections. Tapping
EDIT while editing opens the section map. In the Home menu K2 or PRESETS selects
a row, K1 edits a setting, OCT+ opens/confirms, OCT- closes. Closing the menu
returns to the original page and note cursor. Home actions use the selected
track/pattern and the existing song cursor. Their sheet subtitle identifies the
target. To act on selected notes, select them on SEQ > NOTES, then hold HOME.
Entering the overview with a HOME tap clears that selection as before.

The ALGORITHM encoder continues selecting tracks on HOME and outside ordinary
FM6 EDIT pages (including SEQ). Use those paths or the existing track-selection
layer to change tracks while working on FM6. Sound Designer is a separate modal
editor and keeps its own controls.

Hold-OCT+ context sheets remain available as shortcuts, with Create New Track
first on the overview's sheet. Sound Designer has been removed from those
sheets and from the old settings entry. OCT+ taps for individual notes/hits,
preset/project slot actions, import, motion lanes and load/save confirmations
retain their existing contextual functions. HOME adds access to the hold sheets;
it does not turn all OCT+ tap actions into global commands.

## SEQ page visibility

| Pages | Visible for |
| --- | --- |
| NOTES, PATTERN, NOTE LENGTH, HARMONY, NOTE RANGE, FEEL, TEMPO, PATTERNS, SONG, SONG CHORD, SONG KEY | All modes; existing engine/list conditions still apply |
| EVOLVE, FORM, TWEAK, METER, ACCENT GROUP, RHYTHM RATIO, PHASE, CLOCK RESET, GEN SEED | Any active generator; hidden in STEP |
| AWAKE EDIT | AWAKE only |
| POLY PITCHES | POLY only |
| DRUM DEVELOP, DRUM LANES, DRUM FEEL, DRUM ANCHORS | KIT only |
| MOTION | Existing motion lanes, or while currently editing that page |
| DRUM HIT | Existing drum-engine conditions |

NOTE LENGTH stays accessible in STEP: captured and ordinary playback can use
its duration controls. PATTERN MODE is always accessible. Track/mode changes
that hide the current page return to a visible page in the same family.
Hidden settings survive mode changes and project saves. GEN SEED's CHAOS knob
is displayed and editable only in Grids; the other seed controls are shared.

## How the musical controls relate

- **MODE and sound engine:** MODE chooses the sequencer/generator. FM6, CZ,
  Prophet, SID, DRUM and SLICE choose the sound. Selecting KIT does not silently
  replace a melodic sound engine; its enable control asks for a DRUM engine.
- **DIV, meter and ratio:** DIV sets the generator's base step duration. LOCAL
  enables the saved meter/grouping/ratio/phase settings. BEATS is 1–16 and UNIT
  is 4/8/16. Group counts are 1–8, with positive groups summing to the numerator.
  RHYTHM RATIO's NUM and DEN are 1–8; 3:2 means three local pulses against two
  reference pulses. Pattern length remains separate from meter.
- **Accents:** ACCENT GROUP divides the local bar into groups such as 2+3 in
  5/8. A five-step pattern and a 5/8 meter are independent choices.
- **Phase:** PHASE wraps over one local bar (4096 internal positions). DRIFT
  changes alignment gradually, from −100 to +100 basis points. HOLD freezes
  drift; ALIGN requests immediate or next-bar alignment. CLOCK RESET chooses
  PLAY, BANK, FORM or KEEP. These controls affect generator timing, not the
  master tempo or outgoing MIDI-clock rate. Saved local offsets remain inert
  with LOCAL off.
- **Conditions and structure:** RHYTHM RATIO's COND filters eligible events;
  REF chooses local bars, master bars, FORM sections or pattern cycles.
  EVOLVE mutates generator material; FORM supplies section structure; TWEAK
  supplies shift/drop/human/follow controls. Rhythm MORPH is distinct from
  future sound/timbre morphing.
- **Seeds:** RNG enables repeatable operation; SEED is 1–128; ROLL selects a
  new bounded seed. AWAKE EDIT edits its A/B sequences and probability. POLY
  PITCHES edits four pitches; its divider controls remain on PATTERN.
- **KIT:** DRUM DEVELOP chooses source pattern, variation and fill. DRUM LANES
  selects one of eight lanes and adjusts density/role. DRUM FEEL controls human
  timing, ghost notes and accents. DRUM ANCHORS protects selected source hits.
  Source edits/deletions remain meaningful; anchor masks do not create hits.
  Keep source and capture destination separate when recording a developed kit.
- **Pitch:** HARMONY applies OFF/KEY/SONG weighting, chord bias and resolution.
  NOTE RANGE bounds output and supplies root/scale access. SONG CHORD/KEY
  supplies per-row harmonic context. Existing microtonal harmony limitations
  remain. SCL tuning, note quantization and chord construction retain their
  previous behaviour; this update changes navigation, not pitch mapping.
- **Duration:** STEP gate follows DIV/GATE; FIXED supplies musical length;
  HIT holds to the next hit. Overlap/replacement and release/fade settings
  govern note lifetimes. Sparse triggers can therefore sustain independently
  of density. FM AMP ADSR and FM TONE remain per-track performance controls;
  native operator envelopes and patch parameters are still available.
- **Capture:** one-pass generator REC targets an empty pattern and preserves
  performed onset, velocity and held length within the existing event format.
  Whole-take undo/redo remains. STEP replays the captured events without running
  the generator clock a second time. STOP/mute/track/mode transitions retain
  existing note-release protection.
- **FX:** track inserts process the selected track; Shared FX is the shared
  send/return chain. Chain/Split changes shared routing. OUTPUT controls final
  note MIDI output for the selected track, not shared FX or a device-wide channel.

## Scope, defaults and recall

Track/pattern data—including generators, meter/phase, seeds, anchors, harmony,
recorded notes, automation and sound controls—continues using Performance 7's
FBK7 atomic project format and existing capacity. Sound presets retain their
existing native patch and supported performance-control fields; they are not
complete project snapshots. Saving a sound does not save the whole arrangement.

GLOBAL contains device reference/budget and existing clock/tuning controls;
SYSTEM contains existing MIDI input/routing controls. A page labelled GLOBAL
can include device preferences alongside project data. OUTPUT remains **per
selected track**, despite its new location in GLO. Mixer controls remain
per-track within a project. HOME preferences use the existing settings record.
No NOR allocation, object ID, bootloader or project-format change is introduced.

Existing projects/settings retain their values. LO-FI becomes the default for
unset/legacy-invalid palette preferences; a valid saved palette is preserved.
To select it explicitly: hold HOME → Color → LO-FI. RUBIK/PIXEL fonts remain.
Existing generator modes, local clocks and sound macros retain their previous
off/bypass defaults; this build does not enable musical processing just because
a page becomes visible. Clearing/removing/overwriting keeps existing guards,
confirmation dialogs and undo support where already provided.

Older firmware cannot read FBK7 projects. Keep the Performance 7 package and
backups for rollback; do not expect a Performance 6 downgrade to read new saves.
Character reverb, performance repeats/slices/stutter, sound morphing and MPE
are future roadmap work and are not included in this navigation patch.

## Source-derived page inventory

Generated from the actual firmware page table and knob drawing code. Each engine table is a union of its reachable pages across PATTERN modes; conditional pages are not all visible together. Native operator rows use the selected operator. Shared pages repeat deliberately so each engine has a complete reference. `—` means an unused knob or a page whose controls are rendered as a list/grid. Values are examples from a fresh host session, not replacements for saved project values.


### PROPHET

| Family | Page | Scope | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- | --- | --- |
| FX | FX | Selected track/sound | FX: Off | Slot: 1 | — | Mix: 0 % |
| FX | SHARED | Project/device (see scope notes) | FX: Off | Slot: A | Route: Chain | Mix: 100 % |
| LFO | LFO | Selected track/sound | Rate: 1.18 Hz | Wave: Sin | Phs: 0 | Fade: 1.0 ms |
| LFO | LFO 2 | Selected track/sound | Sync: Off | Trigger: Note | Polarity: Bi | — |
| LFO | LFO DEST | Selected track/sound | Pitch: 0 % | Filter: 0 % | Shape: 0 % | Level: 0 % |
| LFO | MOD | Selected track/sound | —: Off | Slot: 1 /4 | —: Off | —: 0  % |
| SCL | SCALES | Selected track/sound | — | Scale: Chr | Root: C | Quantize: Off |
| SCL | SCL | Selected track/sound | Root: C | Favourite: Off | Quantize: Off | Transpose: 0 st |
| SCL | CHORD | Selected track/sound | Chord: Off | Voicing: Close | — | — |
| EDIT | EDIT 1 | Selected track/sound | Cutoff: 0 | Resonance: 0 | Tuna: 0 st | Tunb: 0 st |
| EDIT | EDIT 2 | Selected track/sound | Pwa: 0 | Pwb: 0 | Mixa: 0 | Mixb: 0 |
| EDIT | P5 STORE | Selected track/sound | Slot: 1 | Store: 0 | Send: 0 | Init: 0 |
| EDIT | P5 OSC A | Selected track/sound | Freq A: 0 st | Saw A: On | Pulse A: Off | Width A: 63 |
| EDIT | P5 OSC B | Selected track/sound | Freq B: 0 st | Fine B: 24 | Width B: 65 | Key B: On |
| EDIT | P5 B WAVES | Selected track/sound | Saw B: On | Tri B: Off | Pulse B: Off | Low B: Off |
| EDIT | P5 MIXER | Selected track/sound | Osc A: 127 | Osc B: 127 | Noise: 0 | Sync: Off |
| EDIT | P5 FILTER | Selected track/sound | Cutoff: 41 | Resonance: 1 | Key: Full | Model: Ssi |
| EDIT | P5 FLT ENV | Selected track/sound | Atk: 21 | Dec: 87 | Sus: 29 | Rel: 94 |
| EDIT | P5 AMP ENV | Selected track/sound | Atk: 20 | Dec: 81 | Sus: 114 | Rel: 84 |
| EDIT | P5 ENV MOD | Selected track/sound | Env amt: 78 | Vel flt: On | Vel amp: Off | Release: On |
| EDIT | P5 POLYMOD | Selected track/sound | Env amt: 0 | Osc B: 0 | Freq A: Off | PW A: Off |
| EDIT | P5 MOD FLT | Selected track/sound | Filter: Off | Vintage: 64 | At flt: On | At LFO: Off |
| EDIT | P5 LFO | Selected track/sound | Rate: 80 | Saw: Off | Tri: On | Square: Off |
| EDIT | P5 WHEEL | Selected track/sound | Initial: 18 | N mix: 0 | Freq A: On | Freq B: Off |
| EDIT | P5 WHEEL 2 | Selected track/sound | PW A: Off | PW B: On | Filter: Off | Glide: 0 |
| EDIT | P5 UNISON | Selected track/sound | Unison: Off | Voices: 5 | Detune: 0 | Priority: Low |
| EDIT | P5 BEND | Selected track/sound | Bend: 7 | — | — | — |
| EDIT | VOICE | Selected track/sound | Voice: Poly | Glide: 1.0 ms | Glide mode: Rate | Priority: Low |
| GLO | OUTPUT | Selected track/sound | MIDI: Off | Chan: 1 | Rec: Off | Pass: Loop |
| GLO | GLOBAL | Project/device (see scope notes) | A4: 440 Hz | Budget: 16 pts | Clk: Int | Tune: 0 |
| GLO | SYSTEM | Project/device (see scope notes) | MIDI: USB OFF | Drum: 10 | Rout: Ch1-4 | Cpu: 0 % |
| SAVE | PRESETS | Project/device (see scope notes) | — | — | — | — |
| SAVE | USER | Project/device (see scope notes) | — | — | — | — |
| SAVE | PROJECT | Project/device (see scope notes) | Slot: 1 | Boot: Off | Load: -- | Save: -- |
| ARP | ARP | Selected track/sound | Mode: Off | Rate: 1/16 | Oct: 1 | Gate: 50 % |
| ARP | ARP 2 | Selected track/sound | Swing: 0 % | Probability: 100 % | Hold: Off | Order: Note |
| SEQ | NOTES | Pattern / step | Step: 1 /16 | Pitch: C4 | Length: -- | Velocity: -- |
| SEQ | PATTERN | Selected track/sound | Mode: Step | Div: 1/16 | Swing: 0 % | Gate: 50 % |
| SEQ | EVOLVE | Selected track/sound | Lock: 100 % | Every: Off | Amount: 30 % | Roll: Turn |
| SEQ | FORM | Selected track/sound | Form: Off | Contr: 60 % | Clear: Turn | Flrsh: Off |
| SEQ | TWEAK | Selected track/sound | Shift: +0 st | Drop: 0 % | Human: 0 % | Follow: Off |
| SEQ | METER | Selected track/sound | Local: 0 | Beats: 4 | Unit: 4 | Grps: 4 |
| SEQ | ACCENT GROUP | Selected track/sound | Count: 4 | Group: 1 | Beats: 1 | Total: 4 |
| SEQ | RHYTHM RATIO | Selected track/sound | Num: 1 | Den: 1 | Cond: Off | Ref: Local |
| SEQ | PHASE | Selected track/sound | Phase: 0 | Drift: 0 | Hold: 0 | Align: Turn |
| SEQ | CLOCK RESET | Selected track/sound | Reset: Play | Local: Off | Meter: 4/4 | Rate: 1:1 |
| SEQ | GEN SEED | Selected track/sound | Rng: 0 | Seed: 1 | Roll: Roll | — |
| SEQ | DRUM DEVELOP | Selected track/sound | Kit: 0 | Src: 1 | Vary: 0 | Fill: 0 |
| SEQ | DRUM LANES | Selected track/sound | Lane: 1 | Dens: 50 | Role: Kick | Src: 1 |
| SEQ | DRUM FEEL | Selected track/sound | Human: 0 | Ghost: 40 | Acc: 115 | Vary: 0 |
| SEQ | DRUM ANCHORS | Selected track/sound | Step: 1 | Mask: 0 | Lock: Hits | Clear: All |
| SEQ | AWAKE EDIT | Selected track/sound | Step: 1 | A: 1 | B: 0 | Probability: 100 |
| SEQ | POLY PITCHES | Selected track/sound | P1: 0 | P2: 2 | P3: 4 | P4: 6 |
| EDIT | DESIGNER | Selected track/sound | Open: Turn | — | — | — |
| SEQ | NOTE LENGTH | Selected track/sound | Mode: Step | Length: 1/8 | Fade: Native | Keys: Over |
| SEQ | HARMONY | Selected track/sound | Harm: Off | Outside: 0 % | Chord: 0 % | Resolve: 0 % |
| SEQ | NOTE RANGE | Selected track/sound | Low: C2 | High: C6 | Root: C | Scale: Chr |
| SEQ | FEEL | Selected track/sound | Div: 1/16 | Swing: 0 % | Gate: 50 % | — |
| SEQ | TEMPO | Project/device (see scope notes) | Bpm: 120 BPM | Swing: 0 % | Quantize: Off | — |
| GLO / MIXER | MIXER | Selected track/sound | Level: -4.0 dB | Pan: 0 % | — | Mute: Off |
| SEQ | PATTERNS | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG CHORD | Project/device (see scope notes) | Row: None | Root: Key | Chord: Track | Bars: Pat |
| SEQ | SONG KEY | Project/device (see scope notes) | Row: None | Key: Track | Scale: Track | Harm: Song |

### FM6

| Family | Page | Scope | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- | --- | --- |
| FX | FX | Selected track/sound | FX: Off | Slot: 1 | — | Mix: 0 % |
| FX | SHARED | Project/device (see scope notes) | FX: Off | Slot: A | Route: Chain | Mix: 100 % |
| LFO | LFO | Selected track/sound | Rate: 1.18 Hz | Wave: Sin | Phs: 0 | Fade: 1.0 ms |
| LFO | LFO 2 | Selected track/sound | Sync: Off | Trigger: Note | Polarity: Bi | — |
| LFO | LFO DEST | Selected track/sound | Pitch: 0 % | Filter: 0 % | Shape: 0 % | Level: 0 % |
| LFO | MOD | Selected track/sound | —: Off | Slot: 1 /4 | —: Off | —: 0  % |
| SCL | SCALES | Selected track/sound | — | Scale: Chr | Root: C | Quantize: Off |
| SCL | SCL | Selected track/sound | Root: C | Favourite: Off | Quantize: Off | Transpose: 0 st |
| SCL | CHORD | Selected track/sound | Chord: Off | Voicing: Close | — | — |
| EDIT | EDIT 1 | Selected track/sound | Alg: Pat | Fb: 0 | Mlvl: 0 % | Mrat: 0 |
| EDIT | EDIT 2 | Selected track/sound | Meg: 0 % | Vmod: 0 | Dtun: 0 % | — |
| EDIT | STORE | Selected track/sound | Slot: 1 | Store: 0 | Send: 0 | Init: 0 |
| EDIT | ALGO | Selected track/sound | Alg: 5 | Fb: 3 | Sync: On | Trns: 0 st |
| EDIT | FREQ | Selected track/sound | Crs: 1 | Fine: 1.00 | Dtn: 0 | Mode: Ratio |
| EDIT | OUT | Selected track/sound | Out: 98 | Velocity: 2 | Ams: 0 | On: On |
| EDIT | EG RATE | Selected track/sound | R1: 96 | R2: 25 | R3: 25 | R4: 67 |
| EDIT | EG LVL | Selected track/sound | L1: 99 | L2: 75 | L3: 0 | L4: 0 |
| EDIT | SCALE | Selected track/sound | Brk: C4 | Ldep: 0 | Rdep: 0 | Rscl: 3 |
| EDIT | CURVE | Selected track/sound | Lcrv: -lin | Rcrv: -lin | — | — |
| EDIT | PITCH EG | Selected track/sound | R1: 99 | R2: 99 | R3: 99 | R4: 99 |
| EDIT | PITCH LV | Selected track/sound | L1: 50 | L2: 50 | L3: 50 | L4: 50 |
| EDIT | FM LFO | Selected track/sound | Wave: Sin | Speed: 34 | Delay: 33 | Ksync: On |
| EDIT | FM BEND | Selected track/sound | Bend+: 0 st | Bend-: 0 st | Step: 0 | Dxvel: Off |
| EDIT | VOICE | Selected track/sound | Voice: Poly | Glide: 1.0 ms | Glide mode: Rate | Priority: Last |
| GLO | OUTPUT | Selected track/sound | MIDI: Off | Chan: 1 | Rec: Off | Pass: Loop |
| GLO | GLOBAL | Project/device (see scope notes) | A4: 440 Hz | Budget: 16 pts | Clk: Int | Tune: 0 |
| GLO | SYSTEM | Project/device (see scope notes) | MIDI: USB OFF | Drum: 10 | Rout: Ch1-4 | Cpu: 0 % |
| SAVE | PRESETS | Project/device (see scope notes) | — | — | — | — |
| SAVE | USER | Project/device (see scope notes) | — | — | — | — |
| SAVE | PROJECT | Project/device (see scope notes) | Slot: 1 | Boot: Off | Load: -- | Save: -- |
| ARP | ARP | Selected track/sound | Mode: Off | Rate: 1/16 | Oct: 1 | Gate: 50 % |
| ARP | ARP 2 | Selected track/sound | Swing: 0 % | Probability: 100 % | Hold: Off | Order: Note |
| SEQ | NOTES | Pattern / step | Step: 1 /16 | Pitch: C4 | Length: -- | Velocity: -- |
| SEQ | PATTERN | Selected track/sound | Mode: Step | Div: 1/16 | Swing: 0 % | Gate: 50 % |
| SEQ | EVOLVE | Selected track/sound | Lock: 100 % | Every: Off | Amount: 30 % | Roll: Turn |
| SEQ | FORM | Selected track/sound | Form: Off | Contr: 60 % | Clear: Turn | Flrsh: Off |
| SEQ | TWEAK | Selected track/sound | Shift: +0 st | Drop: 0 % | Human: 0 % | Follow: Off |
| SEQ | METER | Selected track/sound | Local: 0 | Beats: 4 | Unit: 4 | Grps: 4 |
| SEQ | ACCENT GROUP | Selected track/sound | Count: 4 | Group: 1 | Beats: 1 | Total: 4 |
| SEQ | RHYTHM RATIO | Selected track/sound | Num: 1 | Den: 1 | Cond: Off | Ref: Local |
| SEQ | PHASE | Selected track/sound | Phase: 0 | Drift: 0 | Hold: 0 | Align: Turn |
| SEQ | CLOCK RESET | Selected track/sound | Reset: Play | Local: Off | Meter: 4/4 | Rate: 1:1 |
| SEQ | GEN SEED | Selected track/sound | Rng: 0 | Seed: 1 | Roll: Roll | — |
| SEQ | DRUM DEVELOP | Selected track/sound | Kit: 0 | Src: 1 | Vary: 0 | Fill: 0 |
| SEQ | DRUM LANES | Selected track/sound | Lane: 1 | Dens: 50 | Role: Kick | Src: 1 |
| SEQ | DRUM FEEL | Selected track/sound | Human: 0 | Ghost: 40 | Acc: 115 | Vary: 0 |
| SEQ | DRUM ANCHORS | Selected track/sound | Step: 1 | Mask: 0 | Lock: Hits | Clear: All |
| SEQ | AWAKE EDIT | Selected track/sound | Step: 1 | A: 1 | B: 0 | Probability: 100 |
| SEQ | POLY PITCHES | Selected track/sound | P1: 0 | P2: 2 | P3: 4 | P4: 6 |
| EDIT | DESIGNER | Selected track/sound | Open: Turn | — | — | — |
| SEQ | NOTE LENGTH | Selected track/sound | Mode: Step | Length: 1/8 | Fade: Native | Keys: Over |
| EDIT | FM AMP | Selected track/sound | Atk: 0 MS | Dec: 1602 MS | Sus: 100 % | Rel: 1602 MS |
| EDIT | FM TONE | Selected track/sound | Bri: 0 | Cntr: 0 | Envelope: 0 | Dec: 1602 MS |
| EDIT | FM AMP MODE | Selected track/sound | Level: Off | Fade: Native | Envelope: Voice | Patch: Kept |
| SEQ | HARMONY | Selected track/sound | Harm: Off | Outside: 0 % | Chord: 0 % | Resolve: 0 % |
| SEQ | NOTE RANGE | Selected track/sound | Low: C2 | High: C6 | Root: C | Scale: Chr |
| SEQ | FEEL | Selected track/sound | Div: 1/16 | Swing: 0 % | Gate: 50 % | — |
| SEQ | TEMPO | Project/device (see scope notes) | Bpm: 120 BPM | Swing: 0 % | Quantize: Off | — |
| GLO / MIXER | MIXER | Selected track/sound | Level: -4.0 dB | Pan: 0 % | — | Mute: Off |
| SEQ | PATTERNS | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG CHORD | Project/device (see scope notes) | Row: None | Root: Key | Chord: Track | Bars: Pat |
| SEQ | SONG KEY | Project/device (see scope notes) | Row: None | Key: Track | Scale: Track | Harm: Song |

### CZ-1

| Family | Page | Scope | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- | --- | --- |
| FX | FX | Selected track/sound | FX: Off | Slot: 1 | — | Mix: 0 % |
| FX | SHARED | Project/device (see scope notes) | FX: Off | Slot: A | Route: Chain | Mix: 100 % |
| LFO | LFO | Selected track/sound | Rate: 1.18 Hz | Wave: Sin | Phs: 0 | Fade: 1.0 ms |
| LFO | LFO 2 | Selected track/sound | Sync: Off | Trigger: Note | Polarity: Bi | — |
| LFO | LFO DEST | Selected track/sound | Pitch: 0 % | Filter: 0 % | Shape: 0 % | Level: 0 % |
| LFO | MOD | Selected track/sound | —: Off | Slot: 1 /4 | —: Off | —: 0  % |
| SCL | SCALES | Selected track/sound | — | Scale: Chr | Root: C | Quantize: Off |
| SCL | SCL | Selected track/sound | Root: C | Favourite: Off | Quantize: Off | Transpose: 0 st |
| SCL | CHORD | Selected track/sound | Chord: Off | Voicing: Close | — | — |
| EDIT | CZ TOOLS | Selected track/sound | Name: -- | 1 > 2: -- | 2 > 1: -- | Comp: -- |
| EDIT | CZ LINE | Selected track/sound | Line: Line1 | Mod: Off | Oct: 1 | — |
| EDIT | CZ DETUNE | Selected track/sound | Sign: Off | Oct: 0 | Note: 0 | Fine: 0 |
| EDIT | CZ VIBRATO | Selected track/sound | Wave: Tri | Rate: 0 | Depth: 0 | Delay: 0 |
| EDIT | CZ WINDOW | Selected track/sound | Line1: Off | Line2: Off | — | — |
| EDIT | CZ1 WAVE | Selected track/sound | Wave: Saw | Wave2: Off | W.key: 0 | A.key: 0 |
| EDIT | CZ1 TOUCH | Selected track/sound | Level: 15 | V.pit: 0 | V.wav: 0 | V.amp: 0 |
| EDIT | C1 PIT R1-4 | Selected track/sound | R1: 99 | R2: 0 | R3: 0 | R4: 0 |
| EDIT | C1 PIT R5-8 | Selected track/sound | R5: 0 | R6: 0 | R7: 0 | R8: 0 |
| EDIT | C1 PIT L1-4 | Selected track/sound | L1: 0 | L2: 0 | L3: 0 | L4: 0 |
| EDIT | C1 PIT L5-8 | Selected track/sound | L5: 0 | L6: 0 | L7: 0 | L8: 0 |
| EDIT | C1 PIT POINT | Selected track/sound | Sus: - | End: 1 | — | — |
| EDIT | C1 WAV R1-4 | Selected track/sound | R1: 99 | R2: 99 | R3: 0 | R4: 0 |
| EDIT | C1 WAV R5-8 | Selected track/sound | R5: 0 | R6: 0 | R7: 0 | R8: 0 |
| EDIT | C1 WAV L1-4 | Selected track/sound | L1: 99 | L2: 0 | L3: 0 | L4: 0 |
| EDIT | C1 WAV L5-8 | Selected track/sound | L5: 0 | L6: 0 | L7: 0 | L8: 0 |
| EDIT | C1 WAV POINT | Selected track/sound | Sus: 1 | End: 2 | — | — |
| EDIT | C1 AMP R1-4 | Selected track/sound | R1: 99 | R2: 99 | R3: 0 | R4: 0 |
| EDIT | C1 AMP R5-8 | Selected track/sound | R5: 0 | R6: 0 | R7: 0 | R8: 0 |
| EDIT | C1 AMP L1-4 | Selected track/sound | L1: 99 | L2: 0 | L3: 0 | L4: 0 |
| EDIT | C1 AMP L5-8 | Selected track/sound | L5: 0 | L6: 0 | L7: 0 | L8: 0 |
| EDIT | C1 AMP POINT | Selected track/sound | Sus: 1 | End: 2 | — | — |
| EDIT | CZ2 WAVE | Selected track/sound | Wave: Saw | Wave2: Off | W.key: 0 | A.key: 0 |
| EDIT | CZ2 TOUCH | Selected track/sound | Level: 15 | V.pit: 0 | V.wav: 0 | V.amp: 0 |
| EDIT | C2 PIT R1-4 | Selected track/sound | R1: 99 | R2: 0 | R3: 0 | R4: 0 |
| EDIT | C2 PIT R5-8 | Selected track/sound | R5: 0 | R6: 0 | R7: 0 | R8: 0 |
| EDIT | C2 PIT L1-4 | Selected track/sound | L1: 0 | L2: 0 | L3: 0 | L4: 0 |
| EDIT | C2 PIT L5-8 | Selected track/sound | L5: 0 | L6: 0 | L7: 0 | L8: 0 |
| EDIT | C2 PIT POINT | Selected track/sound | Sus: - | End: 1 | — | — |
| EDIT | C2 WAV R1-4 | Selected track/sound | R1: 99 | R2: 99 | R3: 0 | R4: 0 |
| EDIT | C2 WAV R5-8 | Selected track/sound | R5: 0 | R6: 0 | R7: 0 | R8: 0 |
| EDIT | C2 WAV L1-4 | Selected track/sound | L1: 99 | L2: 0 | L3: 0 | L4: 0 |
| EDIT | C2 WAV L5-8 | Selected track/sound | L5: 0 | L6: 0 | L7: 0 | L8: 0 |
| EDIT | C2 WAV POINT | Selected track/sound | Sus: 1 | End: 2 | — | — |
| EDIT | C2 AMP R1-4 | Selected track/sound | R1: 99 | R2: 99 | R3: 0 | R4: 0 |
| EDIT | C2 AMP R5-8 | Selected track/sound | R5: 0 | R6: 0 | R7: 0 | R8: 0 |
| EDIT | C2 AMP L1-4 | Selected track/sound | L1: 99 | L2: 0 | L3: 0 | L4: 0 |
| EDIT | C2 AMP L5-8 | Selected track/sound | L5: 0 | L6: 0 | L7: 0 | L8: 0 |
| EDIT | C2 AMP POINT | Selected track/sound | Sus: 1 | End: 2 | — | — |
| EDIT | VOICE | Selected track/sound | Voice: Poly | Glide: 1.0 ms | Glide mode: Rate | Priority: Last |
| GLO | OUTPUT | Selected track/sound | MIDI: Off | Chan: 1 | Rec: Off | Pass: Loop |
| GLO | GLOBAL | Project/device (see scope notes) | A4: 440 Hz | Budget: 16 pts | Clk: Int | Tune: 0 |
| GLO | SYSTEM | Project/device (see scope notes) | MIDI: USB OFF | Drum: 10 | Rout: Ch1-4 | Cpu: 0 % |
| SAVE | PRESETS | Project/device (see scope notes) | — | — | — | — |
| SAVE | USER | Project/device (see scope notes) | — | — | — | — |
| SAVE | PROJECT | Project/device (see scope notes) | Slot: 1 | Boot: Off | Load: -- | Save: -- |
| ARP | ARP | Selected track/sound | Mode: Off | Rate: 1/16 | Oct: 1 | Gate: 50 % |
| ARP | ARP 2 | Selected track/sound | Swing: 0 % | Probability: 100 % | Hold: Off | Order: Note |
| SEQ | NOTES | Pattern / step | Step: 1 /16 | Pitch: C4 | Length: -- | Velocity: -- |
| SEQ | PATTERN | Selected track/sound | Mode: Step | Div: 1/16 | Swing: 0 % | Gate: 50 % |
| SEQ | EVOLVE | Selected track/sound | Lock: 100 % | Every: Off | Amount: 30 % | Roll: Turn |
| SEQ | FORM | Selected track/sound | Form: Off | Contr: 60 % | Clear: Turn | Flrsh: Off |
| SEQ | TWEAK | Selected track/sound | Shift: +0 st | Drop: 0 % | Human: 0 % | Follow: Off |
| SEQ | METER | Selected track/sound | Local: 0 | Beats: 4 | Unit: 4 | Grps: 4 |
| SEQ | ACCENT GROUP | Selected track/sound | Count: 4 | Group: 1 | Beats: 1 | Total: 4 |
| SEQ | RHYTHM RATIO | Selected track/sound | Num: 1 | Den: 1 | Cond: Off | Ref: Local |
| SEQ | PHASE | Selected track/sound | Phase: 0 | Drift: 0 | Hold: 0 | Align: Turn |
| SEQ | CLOCK RESET | Selected track/sound | Reset: Play | Local: Off | Meter: 4/4 | Rate: 1:1 |
| SEQ | GEN SEED | Selected track/sound | Rng: 0 | Seed: 1 | Roll: Roll | — |
| SEQ | DRUM DEVELOP | Selected track/sound | Kit: 0 | Src: 1 | Vary: 0 | Fill: 0 |
| SEQ | DRUM LANES | Selected track/sound | Lane: 1 | Dens: 50 | Role: Kick | Src: 1 |
| SEQ | DRUM FEEL | Selected track/sound | Human: 0 | Ghost: 40 | Acc: 115 | Vary: 0 |
| SEQ | DRUM ANCHORS | Selected track/sound | Step: 1 | Mask: 0 | Lock: Hits | Clear: All |
| SEQ | AWAKE EDIT | Selected track/sound | Step: 1 | A: 1 | B: 0 | Probability: 100 |
| SEQ | POLY PITCHES | Selected track/sound | P1: 0 | P2: 2 | P3: 4 | P4: 6 |
| EDIT | DESIGNER | Selected track/sound | Open: Turn | — | — | — |
| SEQ | NOTE LENGTH | Selected track/sound | Mode: Step | Length: 1/8 | Fade: Native | Keys: Over |
| SEQ | HARMONY | Selected track/sound | Harm: Off | Outside: 0 % | Chord: 0 % | Resolve: 0 % |
| SEQ | NOTE RANGE | Selected track/sound | Low: C2 | High: C6 | Root: C | Scale: Chr |
| SEQ | FEEL | Selected track/sound | Div: 1/16 | Swing: 0 % | Gate: 50 % | — |
| SEQ | TEMPO | Project/device (see scope notes) | Bpm: 120 BPM | Swing: 0 % | Quantize: Off | — |
| GLO / MIXER | MIXER | Selected track/sound | Level: -4.0 dB | Pan: 0 % | — | Mute: Off |
| SEQ | PATTERNS | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG CHORD | Project/device (see scope notes) | Row: None | Root: Key | Chord: Track | Bars: Pat |
| SEQ | SONG KEY | Project/device (see scope notes) | Row: None | Key: Track | Scale: Track | Harm: Song |

### SID

| Family | Page | Scope | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- | --- | --- |
| FX | FX | Selected track/sound | FX: Off | Slot: 1 | — | Mix: 0 % |
| FX | SHARED | Project/device (see scope notes) | FX: Off | Slot: A | Route: Chain | Mix: 100 % |
| ENV | ENV | Selected track/sound | Atk: 1.0 ms | Dec: 14 ms | Sus: 100 % | Rel: 14 ms |
| ENV | ENV DEST | Selected track/sound | Filter: 0 % | Pitch: 0 % | Shape: 0 % | — |
| LFO | LFO | Selected track/sound | Rate: 1.18 Hz | Wave: Sin | Phs: 0 | Fade: 1.0 ms |
| LFO | LFO 2 | Selected track/sound | Sync: Off | Trigger: Note | Polarity: Bi | — |
| LFO | LFO DEST | Selected track/sound | Pitch: 0 % | Filter: 0 % | Shape: 0 % | Level: 0 % |
| LFO | MOD | Selected track/sound | —: Off | Slot: 1 /4 | —: Off | —: 0  % |
| SCL | SCALES | Selected track/sound | — | Scale: Chr | Root: C | Quantize: Off |
| SCL | SCL | Selected track/sound | Root: C | Favourite: Off | Quantize: Off | Transpose: 0 st |
| SCL | CHORD | Selected track/sound | Chord: Off | Voicing: Close | — | — |
| EDIT | EDIT 1 | Selected track/sound | Model: 6581 | Wave: Saw | Stack: One | PW: 50 % |
| EDIT | EDIT 2 | Selected track/sound | Cutoff: 100 % | Resonance: 0 | Mode: Lp | Drive: 0 % |
| EDIT | VOICE | Selected track/sound | Voice: Poly | Glide: 1.0 ms | Glide mode: Rate | Priority: Last |
| GLO | OUTPUT | Selected track/sound | MIDI: Off | Chan: 1 | Rec: Off | Pass: Loop |
| GLO | GLOBAL | Project/device (see scope notes) | A4: 440 Hz | Budget: 16 pts | Clk: Int | Tune: 0 |
| GLO | SYSTEM | Project/device (see scope notes) | MIDI: USB OFF | Drum: 10 | Rout: Ch1-4 | Cpu: 0 % |
| SAVE | PRESETS | Project/device (see scope notes) | — | — | — | — |
| SAVE | USER | Project/device (see scope notes) | — | — | — | — |
| SAVE | PROJECT | Project/device (see scope notes) | Slot: 1 | Boot: Off | Load: -- | Save: -- |
| ARP | ARP | Selected track/sound | Mode: Off | Rate: 1/16 | Oct: 1 | Gate: 50 % |
| ARP | ARP 2 | Selected track/sound | Swing: 0 % | Probability: 100 % | Hold: Off | Order: Note |
| SEQ | NOTES | Pattern / step | Step: 1 /16 | Pitch: C4 | Length: -- | Velocity: -- |
| SEQ | PATTERN | Selected track/sound | Mode: Step | Div: 1/16 | Swing: 0 % | Gate: 50 % |
| SEQ | EVOLVE | Selected track/sound | Lock: 100 % | Every: Off | Amount: 30 % | Roll: Turn |
| SEQ | FORM | Selected track/sound | Form: Off | Contr: 60 % | Clear: Turn | Flrsh: Off |
| SEQ | TWEAK | Selected track/sound | Shift: +0 st | Drop: 0 % | Human: 0 % | Follow: Off |
| SEQ | METER | Selected track/sound | Local: 0 | Beats: 4 | Unit: 4 | Grps: 4 |
| SEQ | ACCENT GROUP | Selected track/sound | Count: 4 | Group: 1 | Beats: 1 | Total: 4 |
| SEQ | RHYTHM RATIO | Selected track/sound | Num: 1 | Den: 1 | Cond: Off | Ref: Local |
| SEQ | PHASE | Selected track/sound | Phase: 0 | Drift: 0 | Hold: 0 | Align: Turn |
| SEQ | CLOCK RESET | Selected track/sound | Reset: Play | Local: Off | Meter: 4/4 | Rate: 1:1 |
| SEQ | GEN SEED | Selected track/sound | Rng: 0 | Seed: 1 | Roll: Roll | — |
| SEQ | DRUM DEVELOP | Selected track/sound | Kit: 0 | Src: 1 | Vary: 0 | Fill: 0 |
| SEQ | DRUM LANES | Selected track/sound | Lane: 1 | Dens: 50 | Role: Kick | Src: 1 |
| SEQ | DRUM FEEL | Selected track/sound | Human: 0 | Ghost: 40 | Acc: 115 | Vary: 0 |
| SEQ | DRUM ANCHORS | Selected track/sound | Step: 1 | Mask: 0 | Lock: Hits | Clear: All |
| SEQ | AWAKE EDIT | Selected track/sound | Step: 1 | A: 1 | B: 0 | Probability: 100 |
| SEQ | POLY PITCHES | Selected track/sound | P1: 0 | P2: 2 | P3: 4 | P4: 6 |
| EDIT | DESIGNER | Selected track/sound | Open: Turn | — | — | — |
| SEQ | NOTE LENGTH | Selected track/sound | Mode: Step | Length: 1/8 | Fade: Native | Keys: Over |
| SEQ | HARMONY | Selected track/sound | Harm: Off | Outside: 0 % | Chord: 0 % | Resolve: 0 % |
| SEQ | NOTE RANGE | Selected track/sound | Low: C2 | High: C6 | Root: C | Scale: Chr |
| SEQ | FEEL | Selected track/sound | Div: 1/16 | Swing: 0 % | Gate: 50 % | — |
| SEQ | TEMPO | Project/device (see scope notes) | Bpm: 120 BPM | Swing: 0 % | Quantize: Off | — |
| GLO / MIXER | MIXER | Selected track/sound | Level: -4.0 dB | Pan: 0 % | — | Mute: Off |
| SEQ | PATTERNS | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG CHORD | Project/device (see scope notes) | Row: None | Root: Key | Chord: Track | Bars: Pat |
| SEQ | SONG KEY | Project/device (see scope notes) | Row: None | Key: Track | Scale: Track | Harm: Song |

### DRUM

| Family | Page | Scope | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- | --- | --- |
| FX | FX | Selected track/sound | FX: Off | Slot: 1 | — | Mix: 0 % |
| FX | SHARED | Project/device (see scope notes) | FX: Off | Slot: A | Route: Chain | Mix: 100 % |
| ENV | ENV | Selected track/sound | Atk: 1.0 ms | Dec: 1.41 s | Sus: 100 % | Rel: 1.41 s |
| ENV | ENV DEST | Selected track/sound | Filter: 0 % | Pitch: 0 % | Shape: 0 % | — |
| LFO | LFO | Selected track/sound | Rate: 1.18 Hz | Wave: Sin | Phs: 0 | Fade: 1.0 ms |
| LFO | LFO 2 | Selected track/sound | Sync: Off | Trigger: Note | Polarity: Bi | — |
| LFO | LFO DEST | Selected track/sound | Pitch: 0 % | Filter: 0 % | Shape: 0 % | Level: 0 % |
| LFO | MOD | Selected track/sound | —: Off | Slot: 1 /4 | —: Off | —: 0  % |
| SCL | SCALES | Selected track/sound | — | Scale: Chr | Root: C | Quantize: Off |
| SCL | SCL | Selected track/sound | Root: C | Favourite: Off | Quantize: Off | Transpose: 0 st |
| SCL | CHORD | Selected track/sound | Chord: Off | Voicing: Close | — | — |
| EDIT | EDIT 1 | Selected track/sound | Kit: 808 | Tune: 50 % | Tone: 50 % | Decy: 50 % |
| EDIT | EDIT 2 | Selected track/sound | Snap: 50 % | Acc: 79 % | Kick: Punch | Drive: 0 % |
| EDIT | DRUM SOUND | Selected track/sound | Drum: Kick | Tune: 0 ST | Decay: 64 | Tone: 64 |
| EDIT | DRUM MIX | Selected track/sound | Drum: Kick | Level: 127 | — | — |
| EDIT | LANES | Selected track/sound | Kick: 100 % | Snare: 100 % | Clap: 100 % | Hatcl: 100 % |
| EDIT | LANES 2 | Selected track/sound | Hatop: 100 % | Tom: 100 % | Rim: 100 % | Bell: 100 % |
| EDIT | VOICE | Selected track/sound | Voice: Poly | Glide: 1.0 ms | Glide mode: Rate | Priority: Last |
| GLO | OUTPUT | Selected track/sound | MIDI: Off | Chan: 1 | Rec: Off | Pass: Loop |
| GLO | GLOBAL | Project/device (see scope notes) | A4: 440 Hz | Budget: 16 pts | Clk: Int | Tune: 0 |
| GLO | SYSTEM | Project/device (see scope notes) | MIDI: USB OFF | Drum: 10 | Rout: Ch1-4 | Cpu: 0 % |
| SAVE | PRESETS | Project/device (see scope notes) | — | — | — | — |
| SAVE | USER | Project/device (see scope notes) | — | — | — | — |
| SAVE | PROJECT | Project/device (see scope notes) | Slot: 1 | Boot: Off | Load: -- | Save: -- |
| ARP | ARP | Selected track/sound | Mode: Off | Rate: 1/16 | Oct: 1 | Gate: 50 % |
| ARP | ARP 2 | Selected track/sound | Swing: 0 % | Probability: 100 % | Hold: Off | Order: Note |
| SEQ | NOTES | Pattern / step | Step: 1 /16 | —: Kick | Hit: -- | Acc: -- |
| SEQ | PATTERN | Selected track/sound | Mode: Step | Div: 1/16 | Swing: 0 % | Gate: 50 % |
| SEQ | EVOLVE | Selected track/sound | Lock: 100 % | Every: Off | Amount: 30 % | Roll: Turn |
| SEQ | FORM | Selected track/sound | Form: Off | Contr: 60 % | Clear: Turn | Flrsh: Off |
| SEQ | TWEAK | Selected track/sound | Shift: +0 st | Drop: 0 % | Human: 0 % | Follow: Off |
| SEQ | METER | Selected track/sound | Local: 0 | Beats: 4 | Unit: 4 | Grps: 4 |
| SEQ | ACCENT GROUP | Selected track/sound | Count: 4 | Group: 1 | Beats: 1 | Total: 4 |
| SEQ | RHYTHM RATIO | Selected track/sound | Num: 1 | Den: 1 | Cond: Off | Ref: Local |
| SEQ | PHASE | Selected track/sound | Phase: 0 | Drift: 0 | Hold: 0 | Align: Turn |
| SEQ | CLOCK RESET | Selected track/sound | Reset: Play | Local: Off | Meter: 4/4 | Rate: 1:1 |
| SEQ | GEN SEED | Selected track/sound | Rng: 0 | Seed: 1 | Roll: Roll | — |
| SEQ | DRUM DEVELOP | Selected track/sound | Kit: 0 | Src: 1 | Vary: 0 | Fill: 0 |
| SEQ | DRUM LANES | Selected track/sound | Lane: 1 | Dens: 50 | Role: Kick | Src: 1 |
| SEQ | DRUM FEEL | Selected track/sound | Human: 0 | Ghost: 40 | Acc: 115 | Vary: 0 |
| SEQ | DRUM ANCHORS | Selected track/sound | Step: 1 | Mask: 0 | Lock: Hits | Clear: All |
| SEQ | AWAKE EDIT | Selected track/sound | Step: 1 | A: 1 | B: 0 | Probability: 100 |
| SEQ | POLY PITCHES | Selected track/sound | P1: 0 | P2: 2 | P3: 4 | P4: 6 |
| EDIT | DESIGNER | Selected track/sound | Open: Turn | — | — | — |
| SEQ | NOTE LENGTH | Selected track/sound | Mode: Step | Length: 1/8 | Fade: Native | Keys: Over |
| SEQ | HARMONY | Selected track/sound | Harm: Off | Outside: 0 % | Chord: 0 % | Resolve: 0 % |
| SEQ | NOTE RANGE | Selected track/sound | Low: C2 | High: C6 | Root: C | Scale: Chr |
| SEQ | FEEL | Selected track/sound | Div: 1/16 | Swing: 0 % | Gate: 50 % | — |
| SEQ | TEMPO | Project/device (see scope notes) | Bpm: 120 BPM | Swing: 0 % | Quantize: Off | — |
| GLO / MIXER | MIXER | Selected track/sound | Level: -4.0 dB | Pan: 0 % | — | Mute: Off |
| SEQ | PATTERNS | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG | Project/device (see scope notes) | — | — | — | — |
| SEQ | SONG CHORD | Project/device (see scope notes) | Row: None | Root: Key | Chord: Track | Bars: Pat |
| SEQ | SONG KEY | Project/device (see scope notes) | Row: None | Key: Track | Scale: Track | Harm: Song |
| SEQ | DRUM HIT | Selected track/sound | Step: 1 | Drum: Kick | Pitch: 0 ST | Length: Free |

### PATTERN controls by mode

| MODE | K1 | K2 | K3 | K4 |
| --- | --- | --- | --- | --- |
| STEP | Mode: Step | Div: 1/16 | Swing: 0 % | Gate: 50 % |
| Turing | Mode: Turing | Len: 16 /16 | Probability: Lock | Span: 2 OCT |
| Euclid | Mode: Euclid | Steps: 16 | Pulse: 4 | Rot: 0 |
| Markov | Mode: Markov | Leap: 25 % | Dens: 100 % | Span: 2 OCT |
| MORPH | Mode: Morph | Pos: 0 /127 | Snp A: 1 /8 | Snp B: 3 /8 |
| Grids | Mode: Grids | Map X: 128 | Map Y: 128 | Dens: 50 |
| BOUNCE | Mode: Bounce | Speed: 12 | Angle: 45 | Balls: 1 |
| AWAKE | Mode: Awake | Len A: 8 | Len B: 8 | Probability: 100 |
| POLY | Mode: Poly | Rhythm: 1 | Div: 3 | Span: 2 |
| KIT | Mode: Kit | Source: 1 | Vary: 0 | Fill: 0 |
