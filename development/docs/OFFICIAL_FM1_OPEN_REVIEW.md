# Official FM1 Open tools — relevance to Studio Tess

By: Inputsignwave

Reviewed 21 October 2026. Read-only source/SDK assessment; no official tools or
binaries were executed, no firmware code was imported and no device was flashed.
Performance 7 remains the working baseline and the agreed UI/navigation work
remains the immediate priority.

## Sources and exact revision

- [Official repository](https://github.com/m-vave/fm1_open)
- [Official workbench](https://open.m-vave.com/fm1-workbench)
- [Manufacturer announcement](https://forum.m-vave.com/topic/4)
- Reviewed commit: `4c7496daac04a95ff10d1afdcc8f9ead2dbabc41`, dated 21 October.
- Pinned source archive, SOURCE.json and SDK_INTEGRITY_REVIEW.json are retained
  outside the firmware repository in Research/FM1_Official_Open_2026-10-21.
- All 65 bundled files listed in sdk/manifest.json matched their SHA-256 values.
  This establishes snapshot consistency, not correctness or hardware performance.

The reviewed README describes FM1 Open V14: an official firmware/workbench,
public application sources and a separately supplied binary SDK. It is a
separate application/platform, not an update package for Studio Tess.

## Most relevant findings

| Area | Inspected files | Assessment for our work |
| --- | --- | --- |
| UI rendering | app/screen.c; sdk/include/fm1.h | The renderer composes 16-row RGB565 bands and skips unchanged bands/horizontal margins. Screen submission documents DMA ownership and timeout behavior. Useful design reference for reducing display transfers while reorganising our menus; adapting the strategy requires profiling our own buffers. |
| Typography | app/screen.c | Contains a compact five-column digit/uppercase bitmap font with punctuation and scaling. Useful illustration of a small custom renderer. It does not provide a richer font library than our existing Rubik/Pixel system or automatically create the aesthetic we want. |
| Browser preview/workbench | tools/Build-Web.ps1; web/engine/Simulator.c; sdk/Simulator.js | Browser UI, editing and audition workflow can inform a Studio Tess preview/editor. It compiles public C sources with WASI and supplies browser engine/platform implementations. Our firmware cannot simply be loaded into this workbench; keep the actual-binary chip emulator for platform checks. |
| Audio diagnostics | sdk/include/fm1.h; fm1_engine_control.h; app/board/audio.c | Published statistics cover blocks, short intervals, overruns, peak ticks, submitted/completed work, underruns and dropped MIDI. Compare their boundaries and counters with our existing audio.c/editor.c diagnostics; we already collect deadline and USB underrun/overrun data. |
| MIDI/Bluetooth and audio I/O | sdk/include/fm1.h; fm1_audio_io.h; app/midi_router.c; app/audio_output.c | Useful API contracts for bounded per-port MIDI queues, BLE roles, audio mixing and USB capture. The transport implementation lives behind the binary SDK. Adding it to Studio Tess would require platform integration and measurement, not a header-only import. |
| FX | web/engine/effects.c; app/presets.c; web/engine/sound.c | Public browser code implements filtered four-comb reverb and feedback delay with bounded samples/tail timeout. The 16 advertised demos are genre/project sketches with FX settings, not 16 supplied FX algorithms. Useful audition/reference material; no ready-made Slöer-style character reverb, tape/wobble/imaging suite, keyboard slices or stutter implementation was identified in these files. Device DSP resides in the binary SDK and has not been auditioned here. |
| Build/package checks | sdk/Build.ps1; sdk/manifest.json; sdk/config/linker.ld.in | Integrity hashes, linker/runtime registration checks and firmware reports are useful cross-checks for our current build guards. The reviewed kit pins JieLi Windows 2.5.3, containing PI32v2 clang 4.0.1 with r3 target. This is not a direct replacement for our verified Linux/Mac build path. |
| Samples/library | app/samples.c; app/fm_library.c; sdk/Samples.js; fm1_sea.h | Reference for WAV conversion, sample-bank management and standard DX7 voice exchange. Audio-buffer repeats/slices would still need our own design and RAM budget. Standard DX7 SysEx is a useful exchange boundary; Open projects/sample banks are separate from our formats. |

## Integration limits that matter

Our source-built studio engines and scheduler differ from the official binary
engine. Their advertised eight tracks are not evidence that our complete engine,
voice, FX and capture profile fits eight tracks or gains free RAM. The fixed
sample rate/block contract is 44.1 kHz, 128 stereo frames.

The raw flash API explicitly says that its user partition is not extra free
space and that writes are not transactional. Retain our FBK7 versioning, existing
object IDs, A/B saves and rollback guarantees rather than replacing them with
raw writes. Official project/sample files are not interchangeable with ours.

The public application carries GPLv3 with a specific SDK linking exception;
the native engine has separate component terms. The repository supplies the
engine as native binary archives, without its proprietary source. Any actual
port must preserve notices and establish the applicable terms for the precise
components being combined. No such integration is performed by this review.

## Recommended use and order

1. Continue the agreed navigation and visual identity changes in our existing UI.
   Compare dirty-region rendering and custom glyph approaches if profiling shows
   a benefit; keep the existing layout/control decisions and audio behavior.
2. Cross-check our memory/linker and timing diagnostics against the official
   contracts before expanding reverb or buffered performance FX.
3. Audition the official reverb/delay demos separately, then compare sound and
   measured cost with our current FX. Do not assume the browser implementation
   is identical to native DSP or already meets the requested character-reverb goal.
4. Treat an official-SDK-backed platform experiment as a separate later study,
   particularly if Bluetooth becomes a priority. It is not required for this
   menu update and should not replace the working Performance 7 baseline.

The official browser preview is a source-level application preview. Our chip
emulator runs the compiled firmware binary. They provide different evidence;
neither alone establishes physical FM-1 CPU timing, startup or endurance.
