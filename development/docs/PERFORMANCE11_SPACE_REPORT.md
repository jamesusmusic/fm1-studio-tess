# Inputsignwave: flash recovery and MPE groundwork

By: Inputsignwave

This is a development checkpoint. RC4 remains the hardware-tested baseline.
The Space2 firmware contains space recovery only; MPE and Bluetooth are not
active. The separate lower-zone controller prototype is host-tested and is
not included in the firmware binary.

## Measured recovery

| Measurement | RC4 | Space1: retired CZ editor | Space2: plus exact wavetable packing |
|---|---:|---:|---:|
| Application body, bytes | 573,168 | 566,588 | 560,232 |
| Body limit, bytes | 573,440 | 573,440 | 573,440 |
| Free application flash, bytes | 272 | 6,852 | **13,208** |
| Main managed audio arena, bytes | 51,232 | 53,184 | **53,120** |
| Disjoint audio banks, excluding optional cache | 58,900 | 60,868 | **60,804** |
| Ordinary RAM through BSS end, bytes | 95,648 | 95,632 | **95,632** |

Total application flash recovered is **12,936 bytes**. Space1 recovers 6,580 B;
Space2 adds 6,356 B net. The main audio arena remains above the unchanged
49,152-byte minimum. Bank capacities are not free RAM and cannot be treated as
one contiguous allocation. Total managed bank capacity grows by 1,904 B.
No project/user-bank partition, updater boundary or application slot was enlarged.
The installable package includes other components; its file size is not the
application flash usage.

### Retired editor paths

The CZ synthesis engine was already retired, but hidden editor decoding and
parameter descriptors remained reachable. Production CZ editor entry points
now refuse edits and return no descriptors; the descriptor table and old
conversion routines drop out of the linked image. The source remains in the
historical reference profile. Raw CZ patch storage/validation and project,
backup and user-record codecs remain. A new sanitizer test exercises every
retired control/page, confirms rejected edits write nothing, and checks exact
project/template recall of archived raw patch bytes.

### Original wavetable bank

The existing 12,384-byte bank consists of 48 rows of 129 signed 16-bit samples.
Every row is exactly odd-symmetric. Deduplicating the first 65 samples of each
row gives 45 unique half rows: 5,850 bytes plus 96 bytes of offsets, **5,946 B**
of encoded data. The gross asset saving is 6,438 B; native net saving after
reconstruction code/alignment is 6,356 B.

The audio owner reconstructs the complete original bank into its existing
12,384-byte shared allocation when first needed. Renderers, interpolation,
voice state, precision, presets and project IDs are unchanged. The original
raw table remains a host-test oracle in source but is absent from the target
image. Preparation work changes from a copy to bounded reconstruction; actual
hardware first-note latency remains an acceptance check.

The generator refuses output that cannot reconstruct exactly. Host tests
compare all 6,192 reconstructed signed samples with the unchanged oracle.
`tools/flash_inventory.py` independently reconstructs the bank from the actual
linked half-row/offset symbols and compares its SHA256 to RC4. It also compares
retained FM6/Prophet banks, WAVE LAYERS and font data: **27 arrays verified**,
with no source/precision/preset replacement. The linked CZ_PD table is absent.

### Other compression probes

The four main font bitmap arrays already use Huffman coding; their combined
20,835 B is not a raw/uncompressed saving opportunity. Plain existing LZ packing
of both wavetable banks saved only 865 B gross in an offline probe. The symmetry
encoding was selected because it preserves every sample while saving more.
WAVE LAYERS' eight approved banks remain unchanged.

## MPE groundwork

`experiments/mpe/mpe_controls.h` is a standalone lower-zone controller-state
prototype, not a production receiver. It supports independent member bend,
pressure and CC74; manager bend/pedal; RPN 0 bend ranges; RPN 6 configuration;
RPN cancellation and separate caller-owned contexts per MIDI source. Its host
context is **130 B**, excluding future voice ownership and modulation data.
ASan/UBSan tests pass, including opposite member bends under one manager bend,
separate pressure/slide values, two sources and 100,000 malformed/mixed messages.

Next is channel/source/generation-aware voice ownership and per-voice modulation
on WAVE LAYERS. The present track-wide bend and same-pitch ownership paths must
not be reused as if they were MPE. The [implementation plan](SEABOARD_MPE_PLAN.md)
records source routing, release/channel reuse, worker snapshots, compatibility,
engine stages, recording limits and BLE Central integration gates.

The earlier 24–32 KiB target was a planning allowance. We now have approximately
12.9 KiB free; measure the actual first voice implementation before sacrificing
features to reach an unverified reserve. Bluetooth's linked stack, RAM and timing
cost remain unmeasured. No direct radio support is claimed by the control-state
prototype or by the official SDK's interface alone.

## Exact candidate

Space1 source: `98f6e3417c6ce26c243e6ed74293ef84ec98b36c`.
Space2 native source: `54b694f52221eb85d4db3ca01174bcf76863c8f2`.
Space2 source tree: `3e4b63a7c206a7142dcfda1a6f456ebc9cc1d172`.
[Native build and full checks](https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/40016176176).
Artifact ID 12528602662; artifact ZIP SHA256:
`b880c535c6bc6cb3cb59c7ec7f6b70b52ed8a5437b11aec8a24113ab2bda1ee6`.
Experimental firmware `inputsignwave-1.4.1-performance11-space2.fwsc`, SHA256:
`f06987b3235567d014799b1363871890073be5f81662b551e3b1d2bf13bf553f`.

## Verification status

Space1 full local checks passed in 313 s. Space2 full local checks passed in
324 s: 349 historical golden renders unchanged, no health, voice/routing,
CPU-budget or crash failures. The new target's disassembly gates also pass
separately. Production tests include the new CZ archive test, all-sample
wavetable reconstruction, 32,768 wavetable pitch/control corners, retained
MIDI/recording/storage behavior, concurrent engine matrix, RC4 per-track reverb
routing and the full UI lint sweep. No golden or budget was reset.
The package/checksum/chip-key/application-boundary and recovery-loader checks
pass offline, with no USB or device writes.

Linux native compile/link, foundational sanitizers and the complete registered
host/web suite also passed, in **730 seconds**. Its 349 historical renders are
unchanged with zero health, voice/routing, CPU-budget or crash failures.
The all-palette UI sweep has zero layout findings, two permitted free-text
ellipses and zero off-gray MONO pixels. Optional external DaisySP/Dexed comparison suites require separate
reference checkouts and are not claimed as executed.


The exact Space2 package application ran in **fm1-emu v0.2.0** for **6,000
emulated ms**, over synthetic NOR only. The fixture starts the original
WAVETABLE engine (BRIGHT PLUCK), plus FM kit, Prophet and FM6 tracks, and uses
actual track FX selection, PLAY, held notes, SEND and SIZE edits. This exercises
cold reconstruction of the packed bank. The LCD capture shows the working
REV SHARED page. Output is 249,216 stereo frames at 44.1 kHz, peak 489,
148,404 nonzero channel samples and zero clipped samples, with 104 LCD frames
and no crash/invalid-instruction failure. Eight known unsupported MMIO addresses
remain. The emulator's serial worker fallback does not establish physical CPU1
or RF behavior; concurrent host tests cover worker contracts separately.

The 3,808-byte boot stub is byte-identical to RC4. No device was flashed.
Physical first-note timing and a Seaboard performance remain future hardware
acceptance checks; no MPE audio or Bluetooth pairing is claimed by this run.
