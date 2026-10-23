# Approved reverb: performance controls and next additions

By: Inputsignwave

22 October 2026. The user listened to the first Rings-derived reverb audition and
approved its sound. This approves the algorithm direction; it does not select a
particular core rate or establish hardware resource fit. Preserve the auditioned
settings as references. The user now requires SIZE and requests optional tape
WOBBLE, and asks for SWELL and INFINITE performance modes. The latter behaviours
are included in the next prototype design, subject to DSP and resource validation.
No new DSP controls are implemented by this specification update.

## Control set and actual status

| Control | Function | Status |
| --- | --- | --- |
| SEND / RETURN | Per-track amount into the shared tank and overall wet return; keep dry level independent | Firmware routing pending; audition uses fixed gains |
| SIZE | Apparent space through coordinated tank/diffuser delay scaling, distinct from tail decay | New requirement, pending implementation |
| DECAY | How quickly the normal tail dies away | Implemented as bounded feedback in audition core |
| PREDELAY | Gap between direct sound and reverb arrival | Implemented in audition core |
| TONE | Broad warm/bright shelf of the wet sound | Implemented in audition core |
| LOW CUT / HIGH CUT | Separate wet bass/treble removal | Implemented in audition core |
| DAMPING | High-frequency loss within the decaying tail | Implemented in audition core |
| WIDTH | Mono through stereo spread | Implemented in audition core |
| MOVEMENT / RATE | Existing tank modulation amount and speed | Implemented free-running; transport sync pending |
| WOBBLE | Optional tape-like pitch instability; zero/off preserves the accepted reverb | New request, pending implementation and profiling |
| SYNC / DIVISION | Tempo-based rate where appropriate | Combined-build requirement, not yet connected |
| SWELL / RISE | Smoothly bring wet reverb forward over a chosen time | Planned performance mode; not implemented |
| INFINITE / HOLD | Hold the existing tail while new notes remain available dry | Planned performance mode; not implemented |

## SIZE design

Scale active diffuser/tank tap lengths inside fixed maximum allocations. Do not
reallocate or clear a sounding tank on a knob movement. Smooth/dual-tap crossfade
length changes and compare pitch glide versus crossfade artefacts before choosing
the performance mapping. Retune decay feedback for the changed round-trip time so
SIZE and DECAY remain useful independent controls. Preserve a neutral/default size
that reproduces the approved audition; never silently substitute size for decay
or predelay. The maximum existing allocation bounds maximum size: a larger spatial
range might require more storage and must be measured, not promised for free.

## Optional tape WOBBLE

One macro dial, zero/off neutral, blending slow wow and a smaller faster flutter
component with bounded smooth irregularity. This should sound different from the
existing periodic tank movement. Keep advanced wobble rate/depth optional to avoid
crowding the main performance page. Default off; preserve the approved clean plate.
Use bounded fractional delay movement and keep tone/filters independent. Trial
shared tank modulation versus a small wet-return modulated delay; the latter adds
buffer/CPU cost. Choose after sound/stability measurements. Wobble is time/pitch
movement, not saturation: the separate warm-drive/tape-colour work remains in scope.
Transport sync applies to rhythmic LFO movement; organic tape flutter need not be
forced into a clock division.

## SWELL behaviour to implement and audition

Rise time controls a smooth wet-return fade-in when swell is engaged. It does not
fade the dry keyboard output; incoming notes can feed the reverb during the rise.
This is the first explicit performance-swell definition, not a claim of a separate
reverse-reverb algorithm or an implemented auto-detection envelope. Entry, release
and retrigger need short ramps and clear momentary/latching UI semantics. Save the
configuration deliberately; do not automatically restore an engaged performance
state on project load. Test silence, repeated notes, release during a rise and
combinations with hold. Automatic per-note swell can be explored separately after
this shared-tank performance behaviour is proven.

## INFINITE / HOLD behaviour to implement and audition

Aim to hold the current reverberant texture, close new input to the tank, and keep
the keyboard's dry signal playable above it. Release reopens the send and returns
to the selected normal decay with a smooth transition. This is a freeze of the
existing texture, not merely DECAY at its current maximum and not an overdub mode.

The audition core caps feedback below 0.95 and has lossy damping/DC rejection and
int16 delay storage. Setting a coefficient to unity alone will not prove a stable,
constant-level infinite tail. Prototype a bounded hold path, manage damping/DC and
modulation losses/gain, and test prolonged silence/full input, near-zero tails,
release, SIZE/WOBBLE changes and repeated engagement. Reuse the tank if it works;
if an alternative capture loop is needed, account for its additional memory and
sound difference before committing. Do not claim bit-perfect indefinite retention
or a shipped infinite mode. Fail safely to a bounded held/long-decay behaviour
rather than permit runaway gain. Preserve wet tone/cuts during hold; determine
whether size/modulation changes should be restricted in this state by listening.

## Integration and navigation

Keep the existing approved page layout. Proposed controls can be grouped as core
space (send/return, size, decay, predelay), colour (tone, cuts, damping), movement
(width, motion, rate/sync, wobble), and performance (swell/rise, hold). This is an
organising proposal, not authorization to move sequencer functions to EDIT or to
reassign physical buttons. Final mappings follow the user's navigation decisions.

Keep fresh replacement FX/control identities, FBKT storage and legacy dry bypass.
Before shipping, implement settings/persistence/MIDI/REC deliberately, preserve dry
identity, and test complete dual-WT/FM-kit/FX/history loads, allocation failure,
target flash/RAM/CPU and exact-package emulator operation. User audition approval
is not a substitute for these checks.
