# Physical control mapping — navigation draft

By: Inputsignwave

## FM6 EDIT — ALGORITHM knob

User decision, 21 October 2026: while editing the FM engine, the physical
**ALGORITHM** encoder should change the selected track's FM algorithm.

- Select algorithms **1–32**, using the existing FM6 patch editing path.
- Update the algorithm value/diagram immediately; apply the edit to the sound.
- Do not select another track when turning this knob in the FM6 EDIT context.
- Preserve operator settings and the rest of the patch; retain existing sound
  and project save behavior. The ordinary on-screen ALG control remains usable.
- Define and document track selection elsewhere in the revised navigation map;
  preserve normal menu/popup controls and unrelated engine contexts.
- Verify encoder bounds, track ownership, chart updates and save/reload when
  implementing the new layout.

Current source audit: `ui_input.c` routes EN_ALGO to `track_select()` on ordinary
pages. The ALGO page currently edits FP_ALG through knob 1 in `params.c`, with
`fm6_page_put()` applying the patch. This is a requested mapping change, not a
claim that the physical encoder already edits FM algorithms.

This document records the agreed layout change; firmware remains unchanged
while the complete menu/control layout is being reviewed.
