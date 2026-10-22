# Restarted update: dual wavetable, NIGHT and performable FX

By: Inputsignwave

22 October 2026. Additional user requirements following the approved sound
auditions. This is the combined build scope and a proposed implementation design,
not a claim that these features have already been compiled. It supplements
APPROVED_SOUND_DIRECTION_2026-10-22.md; retain all accepted/rejected sounds there.

## Confirmed requirements

- Return the UI to **NIGHT** for this build. Keep the agreed page layout.
- Support wavetable morphing across different tables, not only positions in one.
- Support **two wavetable layers with different tables and parameters**.
- Each wavetable layer needs an **amplitude ADSR and filter**. Provide attack,
  decay, sustain and release plus filter cutoff/resonance; filter-envelope amount
  can route the layer envelope to cutoff. This is distinct from the position/scan
  envelope. Independent layer amplitude shaping is now a requirement, not an
  optional omission to make the budget fit.
- Morph sound parameters with beat sync.
- Add warm drive/distortion to FX.
- Add FX LFO modulation with sync and a time-division dial.
- Include a new reverb implementation in this build, not only research.
- Reverb needs **both a tone control and a separate filter**: broad warm/bright
  tone shaping plus wet-signal low/high cuts, independently adjustable. Preserve
  the dry signal. Tail damping remains a separate internal/decay behaviour.
- Retain the accepted FM6 sounds, new DX7 percussion kits, kits built from our
  existing FM6 percussion, and the existing 808/909 kits. Exclude the rejected
  sloopDX melodic/drum imports; retain its accepted 808 SUB only.

All approved presets are **additions**, including the 12 approved new Prophet
patches. Retain the existing Prophet/FM6/WT presets and 808/909 sounds; use new
bank/slot identities, preserve old-project timbres, and count any required legacy
WT assets in the budget. Do not use richer-bank integration as permission to
replace existing WT presets. The previously agreed engine/old-reverb retirement
remains in force. See APPROVED_SOUND_DIRECTION_2026-10-22.md for exact selections.

## Wavetable design to implement and profile

Two layers A/B per playable note, with shared read-only waveform assets. Each
layer has its own bank, position, scan settings, tuning, level/pan, amplitude ADSR
and filter state. Filter controls include cutoff and resonance, with a bounded
envelope-to-cutoff amount. Distinguish amplitude ADSR, filter shaping and the
position/scan envelope in the UI. If the combined budget is tight, reduce voice
count or bank/frame footprint before removing the requested ADSR/filter controls.
Do not represent two unrelated tracks as the requested two-layer preset: A/B
settings must live together, save together and respond to one keyboard note.

Distinguish these operations in the controls and help:

1. **Frame scan:** interpolate adjacent single cycles within a chosen bank.
2. **A/B blend:** crossfade the two layer outputs while each layer can scan its
   own bank. With aligned phase/tuning this blends waveform shapes; with different
   pitches/envelopes it is a layered audio crossfade, not spectral interpolation.
3. **Parameter morph:** interpolate selected continuous controls between stored
   endpoints, such as positions, tone, scan depth and layer balance. Use suitable
   mappings for pitch/time controls; do not interpolate engine IDs or enum values.

Manual, envelope and tempo-synced LFO sources should drive the blend/morph. A
slow, independent position envelope is needed for evolving timbres, alongside
repeatable/free-running scan and clear retrigger behaviour. OFF/depth zero must
remain static. Define blend gain so correlated layers do not double in level;
test for cancellation as well as clipping. Smooth bank, position, pan and morph
changes, and retain the Wave2 amplitude/retrigger correction.

All eight accepted timbral directions remain in scope. Curate frames and remove
redundant ones rather than filling flash with an arbitrary large set. The offline
16-frame auditions establish a direction, not a mandatory target file layout.
Compare 8/12/16 useful frames, variable-length band-limited mip rows, 8/16-bit
precision and compressed source formats by ear and exact target link cost.

Use one enabled layer as the inexpensive starting/default configuration and an
explicit second-layer enable. Measure the dual-layer voice cap; do not promise
eight dual-layer notes plus maximum FM6 drums, effects and capture simultaneously.

## Tempo and FX modulation

Proposed operation: **SYNC OFF → RATE in Hz; SYNC ON → DIVISION** on the rate dial.
Include straight values from slow multi-bar movement to 1/32, with dotted and
triplet choices. Use the song/internal or external MIDI clock source consistently
for scan, parameter morph and FX. Preserve musical phase across tempo changes;
define transport restart and optional note retrigger separately. Reverb/delay
tails must continue to render when transport stops, even if modulation phase is held.

Use an FX modulation page with source/shape, depth, sync/division and destination.
Start with one LFO per active FX slot, calculated at control rate with smoothed
sample-rate targets, independently of the track's synth LFO. Useful destinations:
wet mix, filter/tone, drive, width and reverb movement. Modulating feedback or delay
times requires bounded gains/interpolation; avoid allocating or redesigning a tank
from the sample loop. A shared send FX uses its own modulation phase, not whichever
track happens to render last. Save modulation setup and support project/MIDI/REC
contracts deliberately rather than silently assigning old parameter meanings.

Current code already has a tempo-synced track LFO but does not expose this full
FX modulation routing. FX phase increments currently use free-rate LFO tables.
This request therefore requires actual routing/clock/control/storage work.

## Warm drive

Add a dedicated warm saturation effect with DRIVE, TONE, MIX and OUTPUT controls,
smooth parameter changes, DC control and level compensation/headroom. Aim for
gentle rounding at lower drive and useful distortion at higher drive. Existing
SPIT includes clipping but does not fulfil this new warmth/drive design by name
alone. Compare band-limited/oversampled processing against its real CPU cost;
do not claim alias-free saturation without measurement. MIX zero/OFF must retain
the exact dry path. Give new stored effect identities fresh values.

## Reverb in this build

Implement and audition one **shared stereo modulated plate** first, using the
acquired fixed-point Dattorro/Griesinger references and preserving their actual
authors/licences. One tank with a useful range of decay, tone/damping, predelay,
stereo width and modulation can cover tighter space through evolving ambient
tails. Provide modulation-rate/depth control and integrate the requested synced
movement; tempo sync concerns motion, not forced musical rounding of decay time.
Expose **TONE** for broad warmth/brightness and a distinct **FILTER** section for
wet low-cut/high-cut shaping. Do not relabel damping as two independent controls:
the tone/filter controls must have separately audible functions and saved values.
Filtering applies to the reverb path; the dry signal is unchanged. Smooth changes
and test tail stability, cutoff extremes and bypass alongside FX LFO modulation.
Warm/tape colour can come from the new drive stage or a bounded character option.
Keep the previously discussed Slöer-inspired stereo/movement goals as a creative
reference, not a claim about the pedal's internal algorithm.

The user approved the first Rings-derived audition on 22 October. The next
prototype adds independent **SIZE**, optional tape-style **WOBBLE** with zero/off,
and planned **SWELL / RISE** and **INFINITE / HOLD** performance behaviours.
These are pending DSP work, not controls already present in that audition.
Use [the performance-control specification](REVERB_PERFORMANCE_CONTROLS_2026-10-22.md)
for size/decay independence, wet-only swell, closed-input hold, transitions and
stability/resource acceptance. Do not treat feedback 0.95 as an infinite mode.

Start by measuring roughly 16–24 KiB of delay memory, with a smaller candidate if
combined load requires it. This is a design target, not preallocated free memory.
Do not import a full donor tank unchanged or allocate one per track by default.
Reserve retired ROOM/REVERB IDs and preserve legacy dry bypass and saved data;
the replacement gets a new ID. Compare host renders before selecting coefficients
and tank rate; then test stability, tail transitions and allocation failures.

## NIGHT handling

NIGHT already exists at palette index 0. The source generator currently makes
LO-FI (index 7) the provisional default. Set NIGHT as the new build's initial
theme and include it in UI/package verification. Existing saved palette IDs are
valid; define an explicit user-selectable change/migration for an existing device
rather than renumbering palette IDs or resetting unrelated settings. This scope
record alone does not change the device currently running Wave1.

## Combined budget and implementation order

Wave2's measured reference has **28,876 B flash free**, a **62,848 B main audio
arena** and the current **12,384 B shared WT allocation inside the working pools**.
Neither that pool nor the sum of disjoint banks is an unused reverb budget.

For illustration only, eight banks × eight frames × 8-bit rows of 129/65/33/17
entries would be 15,616 B of waveform data; sixteen frames would be 31,232 B.
Those figures exclude metadata, code and a safe highest-pitch path. Double layers
can share bank assets, but need independent phase/filter/envelope state and
additional rendering. A full 64-voice packed FM bank adds 8,192 B if built into
application flash. The combined request needs a real allocation plan and link;
individual feasibility does not prove all features fit at once.

1. Keep the tested Wave2 note-edge fix as the technical starting candidate;
   preserve earlier working builds and existing data. No user flash is assumed.
2. Set NIGHT; prototype richer banks and A/B rendering with bounded layer/voice
   state. Validate raw timbres and synced frame/parameter morphs without FX.
3. Add accepted FM6 voices and per-lane FM kit handling; profile simultaneous hits.
4. Add warm drive, slot LFO/sync controls and the compact shared reverb. Compile
   and measure the combined target early, before expanding presets or controls.
5. Verify persistence/migration, MIDI/recording, bypass and allocation failure,
   sound/aliasing/edge tests, retained engine PCM, dual-core consistency and target
   budgets. Run exact-package emulator checks and supply new listening samples.
   Physical listening/deadline measurements remain separate acceptance checks.

Expected code areas: `eng_wavetable.c` and generated assets; engine/voice resource
management; the modulation and song-clock paths; `fx.c`/new effect modules; parameter,
UI/editor and preset descriptors; versioned project/user-preset storage and MIDI;
palette generation; host/audio-matrix/target/emulator validation. Preserve FBKT
atomic storage, retired IDs and historical archives. Do not silently reinterpret
Wave1's eight controls or existing preset layouts to store two layers.
