# Current UI colour reference

By: Inputsignwave

21 October 2026. The current firmware supplies eight palettes. NIGHT is the default; the active palette saved on the physical FM-1 has not been read. The UI identity update is planned, not implemented.

## Default NIGHT palette

| Role | Design hex | Firmware RGB565 expanded to RGB |
| --- | --- | --- |
| Background | `#17141F` | `#101418` |
| Surface | `#24202E` | `#202029` |
| Main text | `#F3EEF8` | `#F6EEFF` |
| Active control | `#FFFFFF` | `#FFFAFF` |
| Track 1 | `#FF7A5C` | `#FF795A` |
| Track 2 | `#FFC145` | `#FFC241` |
| Track 3 | `#46D9B0` | `#41DAB4` |
| Track 4 | `#9D8CFF` | `#9C8DFF` |
| Recording | `#FF4848` | `#FF484A` |

## Colour roles

The selected track supplies THEME. Each track has four generated knob shades. Secondary text, inactive items, lines, selection/tints and panel layers use fixed RGB565 blends. Recording uses a fixed red (a darker red in light themes). QR codes use fixed black/white, and the crash screen uses a fixed red background.

Available themes: NIGHT, DAY, CONTRAST, SUNSET, OCEAN, NEON, CANDY, LO-FI. GRAY is a host-test palette, not a selectable device theme.

Source: `tools/gen_ui_palettes.py`, `firmware/src/gfx.c` and the generated `ui_palettes.h`. Swatch images and per-track colour data are saved in the workspace UI_Design/Current_UI_Colours_2026-10-21 folder.

## Planned aesthetic update

Develop a distinct Studio Tess visual identity alongside the navigation update. The user chose **LO-FI provisionally on 21 October 2026**. Use it as the starting palette for navigation mockups and the new aesthetic identity. Check text/surface contrast, selected versus inactive states, recording feedback, track differentiation and readability on the FM-1 display. Retain colour-role consistency across the page guide and menus.

## Provisional choice — LO-FI

Already selectable in the installed firmware: hold HOME, select COLOR, turn KNOB 1 to LO-FI, then close the menu. This planning update does not modify firmware or the device's saved colour setting.

| Role | Firmware RGB565 expanded to RGB |
| --- | --- |
| BG | `#201C18` |
| SURFACE | `#312820` |
| TEXT | `#EEE2D5` |
| ACTIVE | `#FFF6EE` |
| TRACK 1 | `#DE8D83` |
| TRACK 2 | `#D5B66A` |
| TRACK 3 | `#94BA8B` |
| TRACK 4 | `#8BA5CD` |
| REC | `#FF484A` |
