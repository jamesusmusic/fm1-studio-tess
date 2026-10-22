# Inputsignwave firmware credits

By: Inputsignwave

Inputsignwave is the contributor name for this project's custom Studio Tess features, integration work, bug fixes and hardening documented in [Tess_Developer_Handoff.md](Tess_Developer_Handoff.md), through Performance 6, with subsequent additions listed below. This includes the studio harmony/SONG additions, USB MIDI and one-pass capture integration, generated-note duration and FM performance controls, project migration/storage integration, and the documented generator/audio safety changes.

When reusing or porting these contributions, retain the name Inputsignwave in the relevant source comments and project credits/changelog. For a modified upstream file use a scoped notice such as:

```c
/* Studio Tess contributions — By: Inputsignwave.
 * See CREDITS.md and Tess_Developer_Handoff.md for scope and provenance. */
```

This credit applies to Studio Tess contributions, not to the entire upstream codebase. Existing copyright notices, SPDX identifiers and licence terms remain intact. This file adds no licence terms.

## Upstream and third-party work

- Melodee: Kerem Kilic (Ellic Studio), https://github.com/keremimo/melodee
- Tess / Tessera generative sequencing: Rastapopolix and the contributors credited by that project, https://github.com/Rastapopolix/melodee-tess
- Felucca: Leo Kuroshita (@kurogedelic), Hügelton Instruments, as credited in the retained source.
- Bundled third-party components: retain their individual notices and licences in the source and LICENSES directories.

Grids, BOUNCE, AWAKE and POLY remain credited to their actual upstream authors and source projects when integrated. Inputsignwave credit covers our port/adaptations and original additions, not authorship of those algorithms.

## Performance 7 additions

By: Inputsignwave

Our Performance 7 contributions include selective Tess generator integration,
independent local timing and rhythmic conditions, coordinated KIT development,
note ownership and fractional capture integration, FBK7 storage adaptation,
resource hardening and validation. See `development/STUDIO7_BUILD_STATUS.md` and
`development/STUDIO7_VALIDATION.md` for implementation and test scope. The pinned
Grids/BOUNCE/AWAKE/POLY algorithms retain their upstream authorship; our credit
applies to porting, integration and our original additions.

## Performance 8 and firmware identity

By: Inputsignwave

Inputsignwave is our firmware's name (formerly Studio Tess). Our Performance 8
contributions include conditional generator-page navigation, HOME action/context
integration, the EDIT Sound Designer launcher, GLO OUTPUT placement, the physical
FM6 algorithm mapping, palette-default integration and navigation validation.
The branding update adds the on-device name and explicit `By: Inputsignwave`
credit, application USB/editor/console names, compatible port discovery and this
attribution record. See `development/INPUTSIGNWAVE_BRANDING.md` and
`development/STUDIO8_VALIDATION.md` for implementation and validation scope.

Melodee, Tess/Tessera, Felucca and third-party algorithms remain their respective
authors' work. Renaming our firmware does not replace their notices, claim their
authorship or change the GPL and bundled component licence terms.

## Performance 9 removal foundation

By: Inputsignwave

Our changes retire SID, CZ, SID drum kits, ROOM and REVERB from the production
DSP while preserving legacy identities and saved payloads; add silent/bypass
compatibility, default-track and UI/editor handling, host alignment hardening,
production PCM/concurrency/allocation/sanitizer checks and target resource
comparisons. See `development/docs/PERFORMANCE9_REMOVAL_REPORT.md`. The new
engines and replacement reverb are not yet implemented. Historical/reference
sources retain their original authors and licences; retained upstream DSP
remains the work of its respective authors.

Inputsignwave Performance 9 wavetable: original mathematical wave/fold bank,
bandwidth-limited oscillator, shared bank lifetime, controls/preset/favorites/editor
integration, spectral and storage tests — **By: Inputsignwave**. Existing Melodee,
Felucca, Prophet, FM6 and drum notices retain their original scope and authors.

## Replacement reverb audition prototype

The standalone module in `development/prototypes/reverb/` adapts the modulated
Griesinger/Dattorro topology of Mutable Instruments Rings, **Copyright 2014
Emilie Gillet**, under the MIT licence. Its original notice is preserved in
`plate.c` and `LICENSE-Mutable.txt`. Fixed-point C/rate/memory adaptation, additional
tone and wet filter controls, bounded state, tests and audition packaging are
**By: Inputsignwave**. This prototype is not yet included in production firmware.
