# Studio Tess stability update 2

15 October 2026. FM-1 test firmware: **v1.3.5-studio-tess.2**.
Primary build: dual core. Single core is included only for controlled diagnosis.

## Scope and provenance

This selectively hardens our installed Melodee 1.3.5 + Tess integration, rather
than replacing it with Tess's older Melodee base or changing the save format.

- Previous studio source: `65fe2b7` (the installed `.1` implementation).
- New implementation: `01b3799` in the local `studio/tess-stability-2` branch.
- Melodee base: `baeabc495c5ae0a6df48c6cf81d056fbc59abb6b`.
- Original Tess donor: `34d401c17b13cc4cb0089e4de1b48c55438789bf`.
- Reviewed Tess interrupt fix: [b89586a](https://github.com/Rastapopolix/melodee-tess/commit/b89586ac4409303dbed21c17405328dd19237e4a).
- Mutation guards adapted from [0e2059d](https://github.com/Rastapopolix/melodee-tess/commit/0e2059d71a0808783589383e16c06614a7d12f8c),
  [c8211c0](https://github.com/Rastapopolix/melodee-tess/commit/c8211c06a81c0c264c6cf30c23e8173deed49333), and
  [9bbb7fc](https://github.com/Rastapopolix/melodee-tess/commit/9bbb7fce79e1e973d4405c9814c2cba94e20e037).

## Changes

1. All PATTERN, EVOLVE, FORM and TWEAK knob handlers publish changes with
   CPU0 interrupts held off. The audio sequencer cannot see a half-edited
   generator. CPU1 only renders jobs submitted and joined by the audio ISR;
   it does not independently access generator state.
2. Saturate encoder deltas before arithmetic; reject invalid knob numbers.
   Extreme input cannot overflow the signed parameter calculations.
3. Generator mode changes explicitly release the generator's own active note.
   Stress testing reproduced a stuck MIDI note 0: ordinary sequencer release
   could mistake the idle arpeggiator's zero sentinel for another owner.
4. Replace Euclid's random-seed retry loop with one bounded draw excluding the
   old seed. ROLL remains immediate and its screen still says TURN. Re-rolled
   pitches may differ from `.1`; rhythm and control meanings are retained.
5. Turing mutation repairs all-zero/all-one active registers. Euclid's pulse
   mutation reflects within 1..STEPS-1 (STEPS 1 stays one pulse). Markov's LEAP
   mutation reflects at 0 and 100. Manual pulse settings still allow zero or
   every step; these guards apply to mutation.
6. AMOUNT 0 makes `tessera_mutate` a true no-op, including its random draws and
   playhead reset. Euclid's explicit ROLL still changes its pitch seed, as before.
7. FORM recall bounds remembered values before use. In particular, shortening
   Euclid's STEPS cannot restore an oversized pulse count or rotation from an
   older section. Stale section bytes from old projects remain readable.
8. Saved-slot archive validation is read-only. Previously it unpacked candidate
   slots onto live state and restored only saved settings, losing unsaved
   playheads and captured LOCK phrases. Valid and rejected archives now leave
   the full runtime untouched during validation.

The 180-byte Tess block and TSR1 saved object remain unchanged. The saved-seed
format, 1..128 ROLL interface, FOLLOW redesign, harmony modes, SONG harmony and
generator MIDI output are deferred. Markov's existing density default is retained.

Reviewed newer Melodee changes include stock-V15 analog supply initialization,
automatic reset of unreadable preset banks, MIDI input channel ranges and undo
gestures. None is included here. They change hardware startup, stored-data policy
or interaction beyond this generator stability update. This build retains the
existing clock, supply behavior, preset protections and CPU1 fault fallback.

## Verification

| Check | Result |
| --- | --- |
| Dual-core target | Built; app 503,420 bytes |
| Single-core diagnostic target | Built; app 501,372 bytes |
| Dual-core audio working arena | 147,808 bytes; minimum 142,336; unchanged from `.1` |
| Dual-core permanent RAM data/BSS | 93,356 / 98,304 bytes |
| Dual-core POOL | 186,572 / 344,064 bytes |
| CPU1 placement/stacks/join checks | Passed |
| Target IRQ disassembly | CLI, CSYNC, STI verified in all four knob handlers |
| Package | Recomputed byte-for-byte from app and loader; installer accepts without force |
| Stub and loader | Identical to a rebuild of `.1`; stub v3 hash prefix baee55ed142fe1cc |
| Generator regressions | All 28 scenarios pass after four reviewed pitch-only baseline changes |
| Project block regression | Original saved-state bytes unchanged |
| New hardening test | 200,000 UI edits with pending playback steps; invalid tracks/knobs and INT_MIN/INT_MAX deltas; stop releases all notes |
| Archive validation | 46,080 byte mutations; invalid loads atomic and validation preserves all live state |
| Mutation/FORM guards | Range tests pass; AMOUNT zero leaves state and RNG untouched |
| Concurrent four-generator mix | 16,384 blocks with two Prophet/two FM6 tracks, insert/master FX and 512 live edits; PCM and saved state exactly match serial dispatch; actual worker jobs run; all notes release |
| Sanitizers | New hardening and backup tests pass AddressSanitizer/UndefinedBehaviorSanitizer; leak detection disabled because unsupported in this execution environment |
| `.1` to `.2` save fixture | Project, generator settings, notes, tempo and name load correctly |
| `.2` to `.1` save fixture | Same compatibility fixture passes in reverse |
| Full host/web suite | 97 jobs completed: 96 pass; one pre-existing CZ-1 determinism job fails |

The concurrent mix covers about 11.9 seconds of simulated audio per dispatch
mode. The 200,000-edit test accelerates generator stepping rather than emulating
physical interrupt timing. Host tests cannot reproduce board voltage, hardware
SRAM misreads or establish an hour of crash-free operation on the device.

The four changed golden cases are `evolve_euclid_every1_amount80`,
`form_euclid_abcb_morph_aabc`, `division_gate_length`, and `all_four_tracks`.
Only Euclid pitches differ; event times, velocities, durations and counts match.
All other scenario hashes and the saved block hash are unchanged. Sound-engine,
CPU and target budget baselines were not rewritten.

The known CZ-1 job fails determinism/dim-control comparisons, as in `.1` and the
previous unmodified-upstream comparison. It is not marked as passing, and this
update makes no CZ synthesis changes. Optional external reference comparisons
are not a substitute for hardware tests; the stock-V15 restore check was skipped
because its package was not present.

## Hardware follow-up

These are fixes and additional protections, not a confirmed diagnosis of the
red error screens. The separate Prophet preset-change freeze also remains
unexplained. No device was connected, flashed, voltage-measured or auditioned here.

Back up before installation. First check `.2` in About, an existing project's
load/save and the four generators on USB power. Then reproduce four-generator
playback with FX and rapid edits under the camera, followed by a comparable
battery session. Capture the complete red screen, control/page, active engine,
power source and whether this followed another crash. Avoid changing several
test conditions at once.

The dual-core build retains the upstream automatic CPU1 retirement/boot guard.
After a fault it can render on CPU0 until a full power cycle; the filename alone
does not prove CPU1 stayed online during every session. Use the single-core file
only when deliberately comparing the same project and actions.

Matching dual-core ELF, function symbols and disassembly travel with this
update. They support function/offset lookup of crash PCs; the optimized build
does not contain source-line debugging information. Symbols from `.1` must not
be used to interpret `.2` addresses.
