# Generator page visibility — agreed layout rule

By: Inputsignwave

21 October 2026. First requested change to the proposed navigation layout:
**hide beat/generator-specific parameter pages until that generator is selected
in the selected track's PATTERN MODE menu.**

**Layout correction, 21 October 2026:** generator, beat-development, rhythm,
phase, seed, harmony and note-range pages remain in **SEQ**. Only OUTPUT moves
from SEQ to GLO, second after MIXER. Sound Designer moves into EDIT; no extra
GENERATOR page is added to EDIT. Conditional visibility reduces the SEQ page
list without changing those page locations.

PATTERN MODE stays reachable at all times. This selector chooses STEP, Turing,
Euclid, Markov, MORPH, Grids, BOUNCE, AWAKE, POLY or KIT. It is distinct from
the track's sound engine (FM6, CZ, Prophet, SID or DRUM).

## Examples for the revised map

| Selected PATTERN MODE | Relevant parameter pages to expose |
| --- | --- |
| STEP | Ordinary pattern/note editing; hide generator-only pages |
| Turing | Turing parameters and applicable shared generator controls |
| Euclid | Euclid parameters and applicable shared generator controls |
| Markov | Markov parameters and applicable shared generator controls |
| MORPH | Rhythm-morph parameters and applicable shared generator controls |
| Grids | Grids parameters and applicable shared generator controls |
| BOUNCE | BOUNCE parameters and applicable shared generator controls |
| AWAKE | AWAKE parameters, A/B sequence editor and applicable shared controls |
| POLY | POLY parameters, pitches/dividers and applicable shared controls |
| KIT | KIT/drum development: source, lanes/density, anchors, variation/fill and feel |

## Navigation and state rules

- Apply visibility to the **selected track**, including when switching tracks.
- Hide other generators' pages from ordinary page scrolling and section/page maps.
- Show shared generator controls only where they affect the selected mode;
  determine the exact membership from the current implementation. Do not hide
  ordinary sequencing or sound controls merely because a generator is inactive.
- Preserve each mode's saved settings while hidden; reselecting it restores
  access to those values. Visibility must not reset parameters or change audio.
- When a mode/track change makes the current page irrelevant, return to an
  appropriate visible page without trapping navigation or losing the selector.
- Keep applicability clear for KIT/Grids and the DRUM sound engine, and distinguish
  rhythm MORPH from future sound/timbre morphing.
- Include these visibility conditions in the final page/function reference.

This is the agreed design rule for the menu reorganisation. The current
Performance 7 firmware has not been changed by this planning update.
