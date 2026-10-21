# HOME actions — navigation draft

**Implemented in the Performance 8 candidate.** This dated planning record is
superseded for current navigation by [the page guide](STUDIO8_PAGE_GUIDE.md).


By: Inputsignwave

User decision, 20 October 2026: include the functions currently reached by
holding OCT+ in the new HOME menu. This supplements the proposed function tree;
no firmware mapping has changed. Source inventory: `ui_popup.c` and the selected
notes sheet in `ui_input.c`.

Layout revision, 21 October 2026: **Sound Designer moves to EDIT**.
**Create New Track** (renamed from Add track) is the first directly selectable
item in the OCT+ actions menu, which is planned to move into HOME.

```text
EDIT
└── Sound Designer

HOME
├── Track overview / sound selection / playback status
└── ACTIONS — currently reached through hold-OCT+
    ├── Create New Track — first item
    ├── SOUND
    │   ├── Init sound
    │   ├── Save as user sound
    │   ├── Favourite
    │   └── Undo last sound load
    ├── TRACKS
    │   └── Remove selected track
    ├── PATTERN
    │   ├── Quantise — whole / half / quarter step
    │   ├── Copy to another pattern slot
    │   ├── Delete pattern
    │   ├── Clear pattern
    │   └── Clear motion
    ├── SELECTED NOTES
    │   ├── Move / copy
    │   ├── Quantise
    │   ├── Delete selected
    │   └── Clear selection
    ├── SONG
    │   ├── Insert section
    │   ├── Duplicate section
    │   ├── Delete section
    │   ├── Take jam
    │   └── Clear song
    └── SHARED FX
        ├── Routing — chain / split
        └── Clear shared FX
```

The current hold action is context-dependent. The HOME design must identify
the target track, pattern, selected notes or song row, retain relevant selection
when entering HOME, and explain unavailable actions. Preserve existing undo,
confirmation and transport/recording rules. Audit the remaining OCT+ tap actions
separately (individual note/hit edits, preset/project slots, import and motion
edit actions); they are not all hold-OCT+ pages. Record their final destinations
in the complete page guide so no action is lost during remapping.
