# Studio Tess audio investigation and hardening 5

Based on the exact delivered `v1.4.1-studio-tess.4` source, originally
`1cdccb7add7679aa7af3b8c6a5231e51bd0b904b`, recovered here as `1b1aa31`.
Main target: dual core. Single core is diagnostic only.

## Hardware report and limits

FM6 distortion/artifacts were reported with Echo, then with FX off, and in the
audible tail after STOP. Displayed CPU exceeded 60% with one sequencer track.
The exact patch, insert/shared FX settings, voice count and device error counters
are not captured yet. One track can have several voices and release tails;
displayed CPU alone does not establish missed audio deadlines.

The reported hardware distortion is not reproduced in simulation. This candidate
does not establish its cause, certify physical timing or demonstrate lower CPU.

## Confirmed defect and hardening

The audio executor rejected bad descriptors, but its CPU0/serial merge still used
their old PCM and unchecked lengths. A rejected valid-length job could repeat
stale samples; a corrupt length could overrun PCM/output bounds. This affects
CPU0 jobs and serial fallback in the dual-core build, and the single-core
diagnostic. The baseline test fails because a rejected job modifies untouched
output. It passes after the fix.

Only successfully executed jobs can now merge. The merge rechecks length and
guard, and the guard also covers the output pointer. Submission rejects null
pointers, invalid lengths/IDs and oversized payloads before copying. Existing
CPU1 retirement/drop behaviour remains: advanced voice state is never replayed.

FM6 output now maps NaN sums to zero before integer conversion. Previously NaN
bypassed clipping and reached an undefined float-to-integer cast. Finite samples
retain their exact clipping/rounding; infinity retains bounded clipping. This is
defensive handling, not evidence that normal patches produce NaNs. It does not
repair arbitrary internal operator-state corruption.

## Investigation and validation

- Existing factory FM6 tests pass under undefined-behaviour checks.
- Fault tests cover stale PCM, bounds, IDs, function/output pointers, oversized
  submissions, serial fallback, CPU1 rejection and finite/non-finite FM6 output.
- New one-track test covers all three FM modes, dry/Echo/Reverb, sparse notes and
  dense retriggers: 18 scenarios, eight seconds each, using real transport STOP.
  Output is bounded, voices release, dry output settles silent, wet tails remain
  audible and concurrent dual-core PCM equals the serial reference exactly.
- The new tail test passes undefined-behaviour and float-cast-overflow checks.
- Full-suite, sanitizer and compiled target/package results are in validation/.
  Host time is not a device CPU/deadline measurement.

No polyphony reduction, new budget, Echo redesign, clock/supply/boot change or
storage format change is part of this pass. Operator arithmetic was already
largely the same in `.3`; new batching and shared control/frequency caching were
the main FM6 execution changes reviewed between `.3` and `.4`.

## Hardware diagnostic capture

This works with installed `.4` and with `.5`. From either extracted bundle,
activate the existing virtual environment, close other editor/MIDI connections,
then run:

```bash
python3 -u source/tools/usb_audio_stats.py --complete --seconds 30 --interval 1 > fm1-audio-report.jsonl
```

Play the affected pattern for about 15 seconds, then press STOP and let its tail
run until capture finishes. This reads counters without changing music, FX,
transport, patches or settings. Default port: `Melodee`; override with `--port`
only if needed. It does not enable USB audio streaming.

Compare counter increases during capture: DMA/deadline misses, CPU1 online,
rejections/faults/timeouts, overload cuts and memory failures. Active/held voices
show the work behind one track. Maxima are since boot; do not attribute older
maxima to the current pattern. Supply the preset name and insert/shared FX
settings alongside the capture. An audio example would help distinguish clipping,
FM timbre/aliasing and discontinuities, if available.

## Candidate status

`.5` is an audio hardening hardware-test candidate, not a proven cure for the
reported distortion. All UI proposals, including the HOME menu move, are deferred
in UI_BACKLOG.md. Existing mixer/filter/FX rack, font and circle of fifths remain.
