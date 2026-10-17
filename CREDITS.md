# Studio Tess credits

By: Inputsignwave

Inputsignwave is the contributor name for this project's custom Studio Tess features, integration work, bug fixes and hardening documented in [Tess_Developer_Handoff.md](Tess_Developer_Handoff.md), through Performance 6. This includes the studio harmony/SONG additions, USB MIDI and one-pass capture integration, generated-note duration and FM performance controls, project migration/storage integration, and the documented generator/audio safety changes.

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
