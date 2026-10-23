# Performance 10 integration status

By: Inputsignwave

23 October 2026. **The complete requested build is not finished.**
Any integration-foundation artifact is not the combined WT/FM-kit/reverb update.

## Implemented source changes

- Append the 12 approved Inputsignwave Prophet designs after all 200 existing
  Sequential programs: 212 native sounds plus INIT. Original stored program
  indexes and native data are preserved; source data remains separately credited.
- Add their browser categories, generated editor data and preset lookup support.
- Fix new favorites beyond slot 207 using a tagged unused settings area, preserving
  existing Prophet stars, retired GRAIN bits, WT favorites and FX-key mappings.
  A persistence test covers the final added preset through saved settings import.
- Make NIGHT the initial palette; retain all palette IDs and saved settings.
  An existing saved LO-FI choice remains selectable and is not silently reset.
- Keep the previously tested Wave2 WT amplitude/retrigger fix.

Initial full tests exposed the old fixed factory-count expectations and a real
favorites-tag overflow, not a firmware audio regression. Those have been repaired;
targeted UI and native persistence tests pass. The original 200-program SysEx
fixture still has 200 records; the expanded internal factory bank has 212.
Do not change the fixture to conceal that distinction.

## Measured storage constraint

The full atomic bank payload is exactly 27,904 B, with only **36 B** unused at
its tail. Project common parameter count is 122, against a below-128 wire limit;
the reserved project gap before Tessera is **zero**. Reusing retired payloads or
silently changing old control meanings would violate the additive/preservation
requirements. The extra WT layers and FX parameters need a deliberate versioned
format and coverage across project, template, native/user preset, undo and editor.

A standalone lossless extension envelope prototype passes ASan/UBSan, corruption,
truncation, capacity-failure and exact-byte preservation tests, but random banks
with its 256-byte extension require 28,398 B and cannot fit. It is not integrated
or claimed as a universal fix. No existing storage reader or flash address changed.

## Validation and exact target measurements

The repaired full local suite passes in **182 s**. Native Linux compile and
registered sanitizer checks pass for source commit
`d15d813d2921bbed4388c315743296f29d101f33`; the GitHub full suite is tracked in
run 39873149875. The downloaded native target archive has verified SHA-256
`ec8bcb67e13fcaf4522668018c9ad1a4b02b78f98d99892ee805d6616d34369f`.

Exact application body: **546,468 / 573,440 B**, leaving **26,972 B** of flash.
This is +1,904 B versus Wave2. Main audio arena remains **62,848 B**; other
disjoint banks remain 3,120 + 4,260 + 784 B, excluding optional cache. These
figures include the new Prophet bank, not the future WT/FM-kit/reverb DSP.
No physical CPU percentage is inferred from the compile or host tests.

A second storage prototype now guarantees **168 B** recovered by packing each
four valid inactive steps into 241 rather than 244 bits. Sanitizer tests cover
95,424 rank/head combinations, exact preservation of all note/inactive fields,
full bank group counts and invalid-input atomicity. It avoids the RLE worst-case
problem but is **not integrated** into any production save path. The versioned
sound schema, old-layout migration and complete persistence coverage remain.

The native-app emulator smoke runs to 1,800 emulated ms, produces 44 LCD frames
and 70,912 stereo audio frames (peak 4,858, no clipping). Eight MMIO addresses
remain unsupported, as in earlier runs. The first smoke container used a temporary
development identity; the local package was subsequently corrected to the release
identity FM-1_91041. This is an app smoke test, not final package acceptance or
hardware CPU evidence. No full-feature firmware was delivered or device flashed.

## Still required before the full build can be delivered

- Versioned sound-state representation with defined worst-case capacity and
  atomic migration/rollback; preserve all existing patterns, recordings and data.
- Eight richer WT bank families, actual two-layer rendering, independent ADSRs /
  filters, scanning and tempo/morph controls alongside the six old WT sounds.
- Twenty-five additional FM6 voices and true per-hit FM kit patches/lifetimes.
- Production shared reverb routing/resampling and complete saved/UI/MIDI controls;
  size, wobble, swell and stable infinite/hold remain new DSP work.
- Warm drive and independent clocked FX LFO modulation.
- Full combined target link/allocation/CPU, storage/MIDI/REC, concurrency, emulator
  and package checks. Existing audition or foundation checks do not validate those.

The earlier 28,876 B flash headroom and 62,848 B arena are Wave2 figures. A new
exact target link is required for the current foundation and again for the full
candidate. Do not present them as available capacity after adding the new DSP.
