# Performance 11 USB MPE Dev1 validation

By: Inputsignwave

Native source: `2ca1b4e7dc89280ef35991d59cea64c72a21d502` on `INPUTSIGNWAVE-GEN`.
Source tree: `c8f3d37dee5959a3c3fe376f8ecd3352ed5a765c`.
Version: `v1.4.1-inputsignwave.11-mpe-dev1`.
Package: `inputsignwave-1.4.1-performance11-mpe-dev1.fwsc`.
SHA-256: `ed8e226d19e4a483f34d3f73b62840dcbea9e692b2724286e8e3cab83703de2b`.

This is a hardware testing candidate for live WAVE LAYERS MPE from a RISE 25
through a Mac/USB bridge. It does not claim physical Seaboard acceptance,
direct Bluetooth, MPE on FM6/Prophet, or expression recording. Setup and mapping
limits are in [the testing guide](PERFORMANCE11_MPE_TESTING.md).

## Implementation areas

| Area | Actual changes |
| --- | --- |
| `firmware/src/core.h` | Transient per-voice MPE channel/key identity, bend state, pressure, slide and Lift snapshot; no serialized project/preset fields added. |
| `mpe_controls.h`, `midi_mpe.c` | Lower-zone RPN negotiation and controls, fixed USB destination, independent voice allocation/ownership, sustain, release snapshots, resets, panic and overflow recovery. |
| `voice.c` | Independent equal-pitch allocation; normal note-offs exclude MPE owners; per-voice pitch smoothing, amplitude/filter/table offsets prepared before immutable worker snapshots. Normal voice starts invalidate old MPE ownership. |
| `midi_control.c`, `seq.c` | Source-aware USB interception; TRS retains ordinary routing. Track panic discards MPE ownership. A bounded bitmap releases old ordinary USB owners when entering MPE. Late note-offs preserve existing admission fades. |
| `params.c`, `ui_draw.c` | Append route values 5–8 without changing 0–4. Full picker/header labels MPE T1–T4; compact cards MPE1–MPE4. Header reflects ongoing route selection. |
| `melodee.c`, CI workflow | Development version and install filename; native compile/link and full registered host/web/sanitizer checks. |
| `tools/seaboard_usb_bridge.py` | Channel-preserving CoreMIDI bridge, exact/unambiguous port choice, updater SysEx exclusion, Ctrl-C/input-disconnect release. |
| `tests/mpe_test.c`, `mpe_dual_test.c`, bridge/UI/web tests | Actual queue/renderer contracts, concurrency parity, updated appended-route editor/format contracts. Production MPE tests run in the mandatory production suite, not the historical build where WAVE LAYERS is disabled. |

## Measured target resources

| Metric | Space2 | MPE Dev1 |
| --- | ---: | ---: |
| App body | 560,232 B | 562,860 B |
| Fixed app body limit | 573,440 B | 573,440 B |
| Spare application flash | 13,208 B | **10,580 B** |
| Main audio arena | 53,120 B | **50,784 B** |
| Required main arena minimum | 49,152 B | 49,152 B |
| Other audio banks: data / low / retained | 2,672 / 4,228 / 784 B | 1,520 / 4,228 / 784 B |
| Total audio banks, cache excluded | 60,804 B | 57,316 B |
| Ordinary RAM consumed through BSS end | 95,632 B | 96,784 B |

MPE adds **2,628 bytes** to the app body. The contiguous arena remains 1,632 B
above the existing enforced minimum. Separate banks cannot be summed as one
contiguous allocation. Optional cache RAM is not enabled or counted. Linked
app including the unchanged 3,808-byte boot stub is 566,668 B. The stub's bytes
match the hardware-tested RC4 build exactly. Saved-data partitions, updater
boundaries and firmware object IDs were not moved.

All **27 retained sound/font arrays** compare identically with RC4, including
lossless reconstruction of the original packed wavetable bank. Existing factory
FM6, added FM6, Prophet, layered table data and fonts are preserved.
Exact target disassembly guards pass without changing budgets. These guards and
host timings do not establish physical CPU percentages or worst-case USB latency.

## Completed local checks

The complete registered host/web suite passed in **353 seconds**. It includes
production retirement/storage parity, original and layered wavetable lifetime/
spectral/corner tests, FM kits, reverb/drive hardening, actual UI rendering,
recording/MIDI and concurrent worker tests. ASan/UBSan remain enabled for new
production paths. The 349 historical golden renders have **0 changed, 0 gone,
0 health failures, 0 voice/routing failures, 0 CPU entries over budget and
0 crashes**. No golden hashes or CPU budgets were updated.

Production MPE tests exercise independent equal pitches, opposite/member+manager
bends, controls before note-on, pressure/slide, retained note-off velocity,
sustain and retrigger/channel reuse, frozen tails, RPN ranges and zone changes,
voice stealing, late release of admission victims, source isolation, transition
from normal USB routing, incompatible engines, suspend/reset, panic, overflow,
and 12,000 mixed queued events with actual audio rendering. The existing
100,000 malformed/mixed-controller probe also passes ASan/UBSan with the
production `mpe_controls.h` forcibly included, rather than the prototype copy. Separate stereo PCM
cases distinguish bend, pressure and slide. Existing ordinary ownership/control
cases run again after MPE.

A real concurrent host worker produces **bit-identical serial/concurrent stereo
PCM across 256 blocks** with four live engine tracks, plate reverb, warm drive,
dense independent expression, sustain, reuse and stealing; all jobs join. A further host replay restricts allocator-bank ends to the native
50,784 / 1,520 / 4,228 / 784-byte capacities, with cache disabled: **31,736 B
peak/used and zero allocation failures**. This replay constrains the host
allocator; it is not a physical SRAM/CPU measurement. Bridge
offline tests cover all 16 channels, ambiguity refusal, message preservation,
SysEx filtering and shutdown. CoreMIDI enumeration also ran successfully on the
Mac, listing Vienna Instruments MIDI input and no output ports: neither test
instrument was connected. No note, controller or flash operation was sent to a
physical instrument during preparation.

All-palette UI: 261 screens × 8 palettes plus page/value sweep, **0 lint findings**,
2 permitted free-text ellipses and 0 MONO pixels off gray. Full/compact MPE route
names are tested, and the emulator screenshot shows the MPE T1 header.
Optional external DaisySP/Dexed comparison checkouts were unavailable and are
not claimed as executed.

## Exact-package chip emulator

`fm1-emu v0.2.0` ran the exact final package application over synthetic NOR for
**6,000 emulated ms**, with layered WT, FM kit, Prophet and FM6 tracks, shared
plate, warm drive and clocked FX settings, plus front-panel note, PLAY and FX edits.
Output: **249,216 stereo frames at 44.1 kHz**, peak 578, 148,417 nonzero channel
samples, **0 clipped samples**. It completed the requested instruction/time
limit without crash or invalid-instruction failure. Eight known unsupported
MMIO addresses remain. The synthetic song is not user data.

The emulator's scripted MIDI input is TRS; this run validates target boot,
combined audio and UI, not physical USB Seaboard transport or BLE. USB MPE
expression is covered by the actual source-tagged host queue/concurrent tests.
The emulator uses serial worker fallback; it cannot establish physical CPU1
headroom. The final rebuilt .fwsc and application are byte-identical to the earlier
`3ada5d0` package used for this emulator run; `2ca1b4e` changes only the test
worker shutdown. The next acceptance gate is the user's two-instrument USB performance.

The concurrent test worker exits cooperatively after all jobs join. This avoids
the Linux ASan forced-cancellation/unwind teardown failure encountered in the
first run, without disabling instrumentation or modifying firmware code; see
the related [sanitizer cancellation issue](https://github.com/google/sanitizers/issues/1010).

## Native Linux completion

[Native workflow 40024019175](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/40024019175)
completed successfully for `2ca1b4e`. Native compile/link, foundational
sanitizers and the entire registered production/historical host/web suite all
passed. The full suite took **768 seconds**, including the cooperative-worker
MPE ASan/UBSan parity check. Its 349 goldens are unchanged with zero health,
voice/routing, CPU-budget or crash failures. All-palette UI has zero lint
findings. Exact-package and recovery-loader checks also pass offline.

Native target artifact: `12531976854`; archive SHA-256:
`afb8ee3660452a0fb236b3891b16b9cd0f4431b495da1ebe7324739ce808a55e`.
The completed install delivery includes verified firmware, 741-file complete
source archive, install/bridge scripts, credits/licences, guides, identity and
checksummed validation evidence. No physical device was flashed.
