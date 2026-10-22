# Inputsignwave compact stereo plate — audition 1

By: Inputsignwave

22 October 2026. **Standalone DSP audition candidate, not installed firmware.**
The production mixer, FX IDs, UI and stored project data have not changed.

## Base and authorship

The base is Mutable Instruments **Rings**, Emilie Gillet's MIT-licensed
Griesinger topology described in the Dattorro paper: four input allpasses and
two cross-coupled tank branches, each with two allpasses and one long delay.
Reference: `rings/dsp/fx/reverb.h` at revision
`f4cfa7697095a28d292bff758beb51deb49b5a86` in
<https://github.com/pichenettes/eurorack>.
The acquired original remains in the separate research folder. Its notice is
retained in `plate.c` and `LICENSE-Mutable.txt`.

Our adaptations are **By: Inputsignwave**: portable fixed-point C, scaled separate
int16 delay rings, caller-owned memory, bounded feedback/controls, smoothed
parameters and interpolated modulation, predelay, independent wet tone/cuts/width,
reset/failure contracts, tests, host rendering and comparison packaging. This is
not a drop-in Rings port, an exact recreation of its sound, or a Slöer emulation.
No Prysm, FoMni, Slöer or Waldorf code is copied into this candidate.

## DSP and controls

- One shared stereo tank, mono send input, wet-only stereo output. The caller
  owns dry routing, mix, send gain and resampling. Stereo input is summed in the
  audition adapter; stereo output is created by the two different tank branches.
- 22,050 Hz core: **24,274 B delay storage + 208 B host state**. Smaller 11,025 Hz
  variant: **12,136 B delays + 208 B host state**. State size includes a host
  pointer; target ABI size, alignment, resampler state and allocator overhead
  still need measuring. There is no per-track tank or hidden allocation.
- Decay is a bounded feedback gain, capped below 0.95. The host exposes a nominal
  RT60 mapping; filtering and modulation alter actual frequency-dependent decay.
- Damping is a lowpass **inside** the feedback loop. Tone is a separate bipolar
  shelf **outside** the tank; wet low-cut and high-cut are independent one-poles.
  They do not process or overwrite the dry signal.
- Width uses bounded mid/side scaling; zero makes wet output mono. Movement uses
  two differently paced triangle LFOs and fractional-delay interpolation, with a
  maximum approximately 0.5 ms tap depth. This is slow chorus-like movement,
  not pitch-shift shimmer. Input headroom is one quarter; wet gain is two.
- Coefficient and fractional predelay changes are slewed. Filter histories retain
  fractional precision to avoid integer deadbands. Buffer writes saturate and
  count saturation; no saturation occurred in the supplied auditions.
- A failed/uninitialised allocation returns wet silence; the production caller
  must retain dry bypass. Reinitialisation clears all tail/filter/LFO history.
- Fixed-point core has no allocation, floating point, filesystem or host-specific
  calls. It does use widened integer products; target instruction cost is open.

## Listening files

`START_HERE.html` in the sibling `Inputsignwave_Reverb_Audition_2026-10-22` folder
plays Clear Plate, Dark Ambient and Moving Air — **settings of one algorithm** —
on approved FM6 kalimba, FM percussion and Vowel Glass concept inputs. The drums
are an offline assembly of existing preview hits, not proof of finished FM kit
routing. The wavetable input is the accepted offline richer-bank concept, not
proof of the dual-layer firmware implementation.

Compare identical sends, RMS-matched mixes and wet-only files. There are isolated
warm/bright tone and low/high-cut comparisons, and a lower-bandwidth 11.025 kHz
version. All clips use 44.1 kHz PCM16. One input-specific gain is shared by every
setting for that input; one common safety trim applies to all files. There is no
limiter or effect-specific peak normalisation. RMS matching is full-file RMS,
not perceptual LUFS. Exact source hashes, settings, gains and measured levels are
in `AUDITION_REPORT.json`. Host reconstruction uses a 193-tap Kaiser FIR;
**this filter is not a target implementation or target cost measurement**.

These inputs came from the user's earlier local sound auditions. Their source
rights/provenance remain in that bundle; inclusion here is for the same private
listening exercise and does not establish firmware redistribution rights.

## Reproduce and test

Compile `plate.c` with `test.c` using C99, ASan and UBSan, at both supported rates:

```sh
clang -std=c99 -Wall -Wextra -Werror -O2 \
  -fsanitize=address,undefined -fno-sanitize-recover=all \
  test.c plate.c -o test-plate
./test-plate
# Repeat with -DPLATE_RATE=11025.
```

`audition.py` requires Python + NumPy and clang. It builds the host renderer,
reads the prior sound-audition bundle and creates the listening folder:

```sh
python3 audition.py --sources /path/to/Inputsignwave_Sound_Auditions_2026-10-22 \
  --output /path/to/Inputsignwave_Reverb_Audition_2026-10-22
```

The module tests silence, undersized/missing buffers, deterministic impulses,
late-tail decay, independent tone/cut controls, mono width, a 90-second full-scale
random-input/control-extreme stress and clearing/reinitialisation. Stress may
intentionally saturate the bounded output; listening renders must not saturate.
The first impulse check caught integer filter-state deadbands; fractional filter
histories fixed that, and both rate variants pass without relaxing the decay test.

## Firmware injection boundary

This is ready to audition and to begin integration, **not yet validated to ship**.
Before a flashable combined build:

1. Choose the core rate after listening. Fit one caller-owned shared tank in a
   contiguous arena along with dual WT layers, FM kits and capture/history.
   Memory above is required capacity, not proof that capacity is free.
2. Implement bounded target decimation/reconstruction and account for latency,
   alias suppression, state, stack and CPU. Host FIR cost is not transferred.
3. Add a fresh FX identity and explicit shared-send routing. Preserve the retired
   ROOM/REVERB IDs and old data/dry bypass. Add independent saved control IDs;
   no silent reinterpretation of old rate/character fields.
4. Connect tempo/transport-derived motion increments; the prototype currently
   auditions free-running rates. Define stop/restart and tail-preserving bypass,
   changing presets, resetting/deleting tracks and allocation-failure behaviour.
   Never clear or allocate in the sample loop; control writes need audio ownership.
5. Add UI/NIGHT, parameter metadata, MIDI/REC and persistence tests, actual target
   link/resource/static cost checks, concurrency and exact-package emulator checks.
   Measure physical CPU deadlines and listen on the FM-1 separately.

Tape drive, freeze/swell and pitch-shift shimmer are not implemented in this first
candidate. The separate requested warm-drive effect remains in the combined scope.
