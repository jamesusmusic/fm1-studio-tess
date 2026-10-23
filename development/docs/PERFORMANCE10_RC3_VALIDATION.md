# Inputsignwave Performance 10 RC3

By: Inputsignwave

## Requested behavior

The replacement stereo reverb (effect ID 10, internally FX_PLATE) displays as
REVERB, including the picker, rack, title and full value cards. Old IDs 7/8 stay
hidden dry-bypass aliases. The DSP and saved effect identities are unchanged.

The on-device Prophet browser presents our twelve additions first: canonical
IDs 201–212, then INIT 0, then original Sequential voices 1–200. Engine selection
starts at 201; explicit Init sound remains 0. Existing projects/native patches,
favorites, recent entries and protocol IDs retain their original identities.
No factory sound was deleted or physically renumbered. The prior audition list's
zero-based factory-bank offsets are distinct from these on-device preset IDs;
the delivery catalogue uses the on-device IDs.

The active local/remote development branch is INPUTSIGNWAVE-GEN. Git ref names
cannot contain spaces. Prior studio-tess-8 history remains available.

## Exact build

Source commit: `0d471ceb4384618fd5fb294df756a24b4725d68a`.
Tree: `8b8a26ebbab73fad4e77bcc34d18e5633ba8f66f`.
Actions: https://github.com/jamesusmusic/fm1-studio-tess/actions/runs/39947975282
Native artifact ID 12499396087, archive SHA256:
`c21d06aa077b9389c0d334e641ed6fcccec314b1479058665254bfa59f8db3a9`.
Firmware `inputsignwave-1.4.1-performance10-rc3.fwsc`, SHA256:
`f40e9b5ebdec2fc27dabc41503e0447b4db0473345bd8f32218df9b6494d31b1`.

Application body **572,464 / 573,440 B**, leaving **976 B**. Main audio arena
**51,520 B**, disjoint tails 2,656 + 4,260 + 784 B (cache excluded), unchanged
from RC2. These are working-bank capacities, not free RAM. Exact package/chip-key,
application boundary, recovery-loader checksum and target static CPU guards pass.
No existing CPU budget or golden expectation was reset.

## Deeper tests and hardening

- RC2 null-preview regression retained: all FX types, every insert/send preview,
  note playback at 30% mix, retired aliases, picker mapping and normal-font labels.
- New canonical-order test visits all 213 Prophet identities exactly once and
  verifies inverse list lookup, new default and explicit INIT. REVERB fits the
  normal picker font.
- New production-transition stress requests 16 keys, changes engine through the
  actual queued fade/ownership path, renders audio and full FX panels while
  randomizing Drive/rate/output/tone. Four ASan/UBSan seeds (default, 1, 42,
  4294967295): **32,768 active audio/preview blocks**, plus transition settling.
  The first draft fixture bypassed engine switching with a host-only instant-load
  helper; this violated its preconditions. It was corrected to the production
  load/fade path rather than treating that synthetic violation as a device bug.
- Registered regression covers unchanged historical PCM, 2,401 engine assignments
  with true concurrent worker parity, note/record ownership, storage failure and
  migration, MIDI/editor malformed input, native voices, FX tails and UI.
- DMA ownership, stale-half protection, nested IRQ deadline accounting and bounded
  CPU1 fault/timeout fallback are exercised by the existing audio/worker/batch
  tests. USB audio tests cover endpoint DMA isolation and refill deadlines.
- The full UI sweep includes both wavetable engines with full Drive/Reverb
  panels/pickers in all palettes; zero layout findings, two permitted free-text
  ellipses and zero off-gray MONO pixels.

The complete local registered suite passed in **262 seconds**. The exact RC3
disassembly also passes the target guards separately.
Linux Actions run 39947975282 passed native compile/link, sanitizers and the
complete expanded host/web suite in **653 seconds**. Its 349 golden renders
are unchanged; zero health, voice/routing, CPU budget or crash failures.
The exact package application was verified byte-identical to the native binary
and overlaid only with synthetic saved NOR. fm1-emu v0.2.0 ran **6,000 emulated
ms**, with later-timed input to allow boot completion: FX navigation, PLAY, held
notes and Drive/output knob changes. The final LCD capture shows the full DRIVE
preview with Phase Prism, MIX 30%, DRIVE 62%, OUT 44%, and a running transport.
The synthetic song also contains an FM kit, new Prophet RH Right Keys, Eno FM6,
shared Reverb and a synced FX LFO. **249,216 stereo frames at 44.1 kHz**, peak 603,
218,840 nonzero channel samples, **zero clipping**; 70 LCD frames, display on.
No crash screen or invalid-instruction failure occurred. Eight known unsupported
MMIO addresses remain. The app uses serial fallback here; concurrent host matrix
and worker fault tests cover CPU1 contracts separately. The emulator's 39.3%
ISR-instruction share/100.3 M ISR instructions per emulated second are diagnostics,
not a physical CPU percentage or deadline margin.

A second 6,000 ms run selected Prophet, stepped presets forward/back and played
notes: 249,216 stereo frames, peak 92, 103,866 nonzero channel samples and no
clipping. Its capture confirms Prophet selection with a held C4. Native table
inspection also verifies the downloaded firmware has REVERB at FX ID 10 and
empty aliases at retired IDs 7/8, separately from host UI rendering.

## External resources checked

The current official M-VAVE repository remains at
[4c7496d](https://github.com/m-vave/fm1_open/tree/4c7496daac04a95ff10d1afdcc8f9ead2dbabc41).
All **65** pinned SDK manifest files verify again. Its `app/board/audio.c` guards
DMA half ownership and deadline before committing audio, acknowledges pending
interrupts and bounds late writes. We compared these contracts with our own
registered audio, worker and USB DMA tests. No vendor binaries or platform code
were injected. The official workbench/simulator targets FM1 Open's separate
engine/API and storage, so it does not validate this firmware by loading it as
an Open project. See the existing OFFICIAL_FM1_OPEN_REVIEW.md for ABI/licensing.

[fm1-emu](https://github.com/joakimeriksson/fm1-emu/releases/tag/v0.2.0) latest
release was rechecked as v0.2.0; the pinned macOS ARM64 chip emulator executes our
native app. Its source key/encoder mapping was also reviewed for scripted tests.
No executable was upgraded blindly, and no user device or saved NOR was touched.

## Remaining acceptance

No offline tool proves physical CPU utilization, stack high-water, cache/bus
contention, analog quality or worst-case FM-1 deadlines. Emulator unsupported
MMIO/CPU1 limitations are reported with the results. Test the final RC3 on hardware:
Reverb naming and routing, the first twelve Prophet additions, Phase Prism plus
Drive around 30%, note releases, long Hold/Swell and project reload.

Full-bank backup remains recommended. RC3 uses the same FBK8/TPLM formats as
RC1/RC2; older pre-Performance-10 firmware cannot read new saves. Optional record
undo can remain unavailable under heavy allocations; sound/recording take priority.
