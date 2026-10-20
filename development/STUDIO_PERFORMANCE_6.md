# Studio Tess Performance 6 — hardware test candidate

Version: **v1.4.1-studio-tess.6**, based on Audio Hardening 5.
Dual core is the primary build. MPE, patch/timbre morphing and the deferred UI
redesign are not included. Existing fonts, HOME, mixer, filter, circle of fifths,
FX rack and sequencer page order are retained; only the new feature pages are added.

## Generator note timing

Select a generator track, press SEQ and navigate to **NOTE LENGTH**.

| Knob | Display | Behaviour |
|---|---|---|
| 1 | MODE | STEP = existing GATE/DIV behaviour; FIXED = independent musical length; HIT = hold until the next actual generated hit |
| 2 | LEN | 1/32, 1/16, 1/8, 1/4, 1/2, 1 BAR; used by FIXED |
| 3 | FADE | NATIVE at zero; otherwise an outer release fade, displayed in milliseconds |
| 4 | KEYS | OVER = up to four independently held generated pitches; REPL = end the previous held key when the next hit occurs |

Existing projects load with STEP and NATIVE, preserving their previous behaviour.
The new FIXED mode defaults to OVER: changing LEN then changes held time without
slowing DIV. HIT always replaces the previous key at the next hit and ignores LEN
and KEYS; rests and dropped hits do not release it. Released audio tails can overlap
subject to the existing engine voice limits and shared voice budget.

Repeated pitches retrigger: the old same-pitch key ends before the new key starts.
In OVER, a fifth held pitch releases the oldest held owner. Normal synth voice
modes still apply: a MONO/LEGATO patch does not become polyphonic because OVER is on.
HUMAN still changes velocity; its gate-shortening applies only in STEP mode.
Fixed duration is calculated at note onset using the current tempo, independently
of DIV. Its running sample timer is not stretched retrospectively by later tempo
changes. Timing is resolved at the existing 32-sample control blocks.

For slow attacks, start with **FIXED, 1/2, OVER**, or choose **HIT** for sparse
Euclidean patterns. Try FADE around **27–102 ms** (raw values 5–10), then adjust
by ear. Increasing held duration allows the patch's native attack to develop.

## Release and transition protection

Held duration ends at note-off; audible release/fade happens afterward.
FADE is per track and applies to individual internal voices, including keyboard
and captured STEP playback. It does not fade the entire track or clear FX buffers.

With FM6 FADE enabled, the native operator voice remains keyed while its outer
gain fades. This prevents a fast R4 from defeating the chosen fade. At fade end,
the voice is retired and native release bookkeeping is completed. A native patch
that already decays while held can still become silent before that outer fade ends.
With NATIVE, original operator release behaviour remains in control. Other engines
retain their own envelopes; FADE can shorten their release and does not extend an
engine voice that has already ended.

FM6 voice reuse with FADE or FM AMP active also applies a bounded sample correction:
the first sample meets the preceding voice's last sample and the correction settles
within 128 samples. This is transition protection, not proof that the intermittent
hardware Bells distortion has been cured. The generic ADSR no longer gets first
opportunity to retire a normal FM6 voice before its native release is checked;
emergency stage-4 fades remain active.

## FM AMP

On an FM6 track, press **ENV** and select **FM AMP**. Knobs are **ATK, DEC, SUS, REL**.
Times display milliseconds; sustain displays percent. Larger time values are slower.
Editing any ADSR knob enables the overall per-voice envelope automatically.

**FM AMP MODE** provides **AMP ON/OFF** and access to the same track FADE setting.
AMP OFF bypasses the new amplitude envelope without discarding its settings.
All original operator rate/level, pitch-envelope and deeper editing pages remain.

The envelope multiplies each FM voice after synthesis and before track effects.
It does not rewrite the six native operator envelopes. With FADE at NATIVE, it
cannot extend a sound beyond its native operators' lifetime. AMP OFF plus neutral
tone macros and NATIVE fade reproduces the previous sound baselines.

## FM TONE

The ENV family also includes **FM TONE**:

| Knob | Display | Behaviour |
|---|---|---|
| 1 | BRI | Adjust modulator level: negative softens, positive brightens |
| 2 | CNTR | Adjust modulator envelope rates together: positive is slower, negative faster |
| 3 | ENV | Bipolar amount of a per-note decaying brightness contour |
| 4 | DEC | Time of that brightness contour in milliseconds |

BRI, CNTR and ENV are neutral at displayed **0**. DEC is inactive when ENV is zero.
Carrier operators are identified from the current algorithm; contour leaves their
native amplitude envelopes unchanged. Algorithms with no modulators have limited
or no response to the tone macros. These are FM macros, not a resonant low-pass filter.
The original patch bytes remain unchanged; all edits derive temporary effective values.

## MIDI and one-pass REC

USB MIDI note-offs use the actual generator held duration. External instruments
use their own release envelopes: MIDI note-off does not transmit the internal fade.
The output still needs a USB MIDI host, such as the Mac/DAW.

REC still requires an empty destination, waits for the next loop when already
playing, records one pattern-length pass, and automatically disarms while the
generator continues. It captures final pitch, velocity, onset and actual held
duration, including notes spanning rests. Captured notes still held at punch-out
are sealed at the capture boundary; the live generator's keys continue. This makes
the take finite and keeps whole-take undo/redo safe.

Switch MODE to STEP to replay/edit the capture. FADE and FM performance settings
remain on the sound/track for replay. STOP, mute processing, mode and bank changes,
project/track changes and panic release generator ownership. External-clock loss
uses the existing transport-stop path. An external instrument's own release may
still ring after its MIDI key has ended.

## Saves, upgrades and rollback

Save the project to retain note timing mode, length, overlap, FADE, FM AMP and TONE.
Shared **FM6 user sound** saves also retain FADE and AMP/TONE; they preserve the
track's generator timing choices when loaded. Factory sounds reset the sound
controls to bypass/neutral. Sound-load undo/redo includes the sound controls.
Other engine sound presets do not store the new FADE; their project saves do.
The external patch editor has no new macro controls in this candidate; the included
backup/editor files preserve the complete project/backup payloads.

HMF2 fits into the same 116-byte feature block and existing A/B project allocation.
Old HMF1 saves are read with the new features bypassed. No notes, pattern banks,
recording capacity, preset slots or flash sectors were taken for these settings.
The firmware also retains the tested .2/.3 forward migration.

**Preserve a full pre-install backup.** Audio5/.4 cannot read projects saved with
HMF2. Older firmware may ignore or drop new FM sound macros. Rollback requires the
Audio5 firmware **and the pre-install backup**, not merely flashing the old firmware.
Installing this build does not convert every saved project automatically; SAVE
commits a project's new feature format. Legacy sidecar migration still completes
before shared preset startup, as in Audio5.

## Suggested hardware test

1. Back up on the currently installed build. Install the main dual-core candidate
   with stable USB power and leave first-start migration uninterrupted.
2. Check About for v1.4.1-studio-tess.6. Load an existing project and first compare
   playback with STEP/NATIVE, AMP OFF and neutral tone macros.
3. On a slow-attack patch, compare FIXED 1/8 versus 1/2 while leaving DIV unchanged;
   compare REPL and OVER. Then try Euclid HIT with sparse pulses and rests.
4. Try FM6 Bells with FADE NATIVE, then 27–102 ms. Test repeated pitches, STOP tails,
   dry output, Echo and Reverb separately. Check whether the reported pops recur.
5. Shape FM AMP and TONE, save/reload a project and FM6 sound, then test one-pass
   REC and whole-take undo/redo. Replay in STEP and check USB MIDI release timing.
6. Compare USB and battery operation separately. Note the displayed CPU and voice
   count. Software tests and static DSP budgets do not measure the device's CPU load.

If artifacts return, capture counters before a power cycle:

```bash
python3 -u source/tools/usb_audio_stats.py --complete --seconds 30 --interval 1 > fm1-audio-report.jsonl
```

Play for approximately 15 seconds, then STOP and let the tail finish. Retain the
JSONL, patch name, FX settings and new performance settings. Single-core firmware
is included only for comparison, not as the primary update.

## Build reproduction

With the JieLi toolchain and AC79 SDK configured as described in BUILDING.md:

```bash
MELODEE_RAM_AUDIO=1 ./build.sh --release 1.4.1-studio-tess.6
MELODEE_RAM_AUDIO=1 PARALLEL=4 bash tests/run_tests.sh
```

Diagnostic only:

```bash
MELODEE_RAM_AUDIO=1 MELODEE_DUAL_CORE=0 STUDIO_SINGLE_DIAGNOSTIC=1 ./build.sh --release 1.4.1-studio-tess.6-single
```

See the bundle's validation logs for the exact software checks. Physical FM-1
playback, device CPU load and the intermittent Bells fault remain hardware tests.
