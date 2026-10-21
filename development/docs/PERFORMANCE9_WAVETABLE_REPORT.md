# Inputsignwave Performance 9: wavetable prototype

By: Inputsignwave

21 October 2026. Development prototype; target compilation and resource measurements pending.
Prophet, FM6 and 808/909 remain. SID, CZ, SID kits, ROOM and REVERB remain retired.
Replacement reverb and resonator are separate next stages.

## Sound and controls

Select WAVETABLE in PRESETS or Sound Designer. Its stable engine ID is **21**;
20 remains the Prophet user-preset namespace. Existing IDs and disk layouts stay fixed.
Six factory sounds: INIT WAVE, HOLLOW KEYS, GLASS DRIFT, FOLDED BASS, SLOW SCAN,
BRIGHT PLUCK. Normal ADSR, LFO, modulation matrix, poly/mono/legato/unison and
recording paths apply. This engine generates sound, so it needs no sample imports.

| Page | Control | Behaviour |
| --- | --- | --- |
| WAVE SCAN | WAVE | Morph between original sine, hollow, glass and bright spectra. |
| WAVE SCAN | FOLD | Morph between three precomputed triangle-folded spectra. This is not live nonlinear wavefolding of incoming audio. |
| WAVE SCAN | RATE | Scan oscillator rate, using the existing LFO rate scale. |
| WAVE SCAN | DEPTH | Scan depth; zero disables scanning's audible effect. |
| WAVE TONE | TONE | Two-pole low-pass, transparent at full setting; responds to filter modulation. |
| WAVE TONE | SUB | Blend a sine one octave below, up to half the source. |
| WAVE TONE | COARSE | -24 to +24 semitones. |
| WAVE TONE | FINE | -50 to +50 cents. |

Front knobs map to WAVE, FOLD, TONE, DEPTH. MIDI CC74 adjusts TONE and CC71 FOLD.
Factory stars use a tagged four-byte area previously unused in the retired GRAIN
favorites row, preserving the old stars and FX key map. User presets retain the
normal packed format with explicit validation of ID21. Older firmware cannot
play the new engine and should not be used to resave these new sounds.

## Implementation and limits

`eng_wavetable.c` contains oscillator/control processing and six factory presets.
`gen_wavetable.py` reproducibly generates `wavetable_bank.h` from original mathematical
spectra: no third-party waveform assets or DSP were imported. Four frames, three
fold spectra, four harmonic limits (15/7/3/1) and 129 int16 points use **12,384 bytes**.
The bank lives in flash and is copied once to managed audio RAM when needed.
All tracks share it; per-voice phases and filter/ramp state fit existing voice fields.
The audio owner releases it when no wavetable voices remain. Allocation failure
produces silence and increments diagnostics, without overwriting other resources.

Pitch-dependent table crossfades reduce harmonics before Nyquist; linear phase,
wave, fold and band interpolation plus block control ramps reduce discontinuities.
This is not an absolute alias-free claim: table interpolation images, fast modulation
and pitch-jump transients remain subjects for listening. No hardware CPU percentage
can be inferred from host elapsed time or emulator throughput.

Host checks currently cover all 32,768 note/control corners, zero/short blocks and
output canaries; periodicity, DC, harmonic limits; shared allocation/idle release;
exhausted-arena failure; signed project/user-preset round trips and favorite settings;
six deterministic chord presets with complete note release. Coherent full-fold oscillator
measurements include interpolation images: at 1,100.89 Hz unwanted energy is -59.93 dB,
and at 4,998.39 Hz -76.61 dB relative to intended harmonic energy. Compared with a
naive live fold of the same source, those are improvements of 47.51 and 64.57 dB.
They are two static fixtures, not a full-band musical quality verdict.

Four-track concurrent playback is expanded from 256 retained assignments to 625
ordered assignments including wavetable; serial and concurrent PCM must match.
Historical reference goldens run with the new engine disabled. Production retained
PCM comparisons remain independent. Full results and target budgets will be added
after the build completes.
