# Neodox Ergomech — Vial Keymap

My [Vial](https://get.vial.today/) keymap for the Neodox Ergomech, a split RP2040
board with an OLED on each half.

This is a [QMK Userspace](https://docs.qmk.fm/newbs_external_userspace) repo, so
it holds only the keymap. The firmware tree and the board definition stay where
they are; nothing here is a copy of anything, which means there is exactly one
source of truth for each piece.

## Layout

```
keyboards/neodox_ergomech_qmk/keymaps/neodox_vial/
├── keymap.c            layers, combos, OLED rendering, duck art
├── config.h            feature tuning
├── rules.mk            enabled features
├── oneshot.c/.h        custom sticky modifiers
├── bongo_cat.c/.h      bongo cat animation (see Credits)
└── vial.json           Vial layout definition
qmk.json                userspace build targets
```

The directory nesting is not decoration — QMK resolves a userspace keymap by
looking for `keyboards/<keyboard>/keymaps/<keymap>/` under the userspace root,
so the path has to mirror the firmware tree.

## Requirements

A working QMK environment (on Windows, the QMK MSYS shell) plus:

1. The firmware tree this is developed against:

   ```
   git clone --recurse-submodules -b cyboard git@github.com:Cyboard-DigitalTailor/vial-qmk.git
   ```

2. The board definition, at `keyboards/neodox_ergomech_qmk/` inside that tree —
   it is not part of vial-qmk, and comes from
   [ergomechstore/neodox-ergomech-qmk](https://github.com/ergomechstore/neodox-ergomech-qmk).

This is only known to build against that combination; it has not been tested
against stock vial-qmk or mainline QMK.

## Building

Point QMK at this repo once:

```
qmk config user.overlay_dir="/full/path/to/neodox_vial"
```

Then build **from the root of the vial-qmk checkout**:

```
cd /path/to/vial-qmk
qmk userspace-compile
```

The working directory matters. This QMK sets `QMK_FIRMWARE = Path.cwd()`, so
running from anywhere else sends it looking for the board in whatever tree the
`qmk` launcher falls back to — usually `~/qmk_firmware`, a stock checkout with
no Neodox board in it — and you get `ValueError: Invalid keyboard`.

## Flashing

Both halves run the same firmware. Put a half into bootloader mode by holding
BOOT and tapping RESET on its controller, then copy the generated `.uf2` to the
drive that appears.

Only the half whose behaviour changed needs reflashing — but after changing
`DYNAMIC_KEYMAP_LAYER_COUNT`, flash both and do a bootmagic EEPROM reset (hold
the top-left key while plugging in), because that define shifts every downstream
EEPROM address and will otherwise corrupt the stored combos.

## What's in the keymap

- **Seven layers**: QWERTY, Night, Gallium, Game, Lower, Raise, Adjust.
- **15 combos**, seeded into EEPROM on first boot and then owned by Vial. Each
  set is scoped to the base layout it was drawn for, and all of them are
  disabled on the Game layer.
- **Custom sticky modifiers** (`oneshot.c`) for Shift/Ctrl/Alt/Super.
- **Left OLED**: current layer and the four sticky mods, drawn with a vendored
  glyph table so every label is centred to the pixel.
- **Right OLED**: a sailor duck bobbing on the ocean, or a bongo cat — toggled
  at runtime with `ANIM_TOG`. Keypresses are pushed to that half over a split
  RPC transaction so the animation reacts per keystroke.

## Credits and licensing

Everything here is GPL-2.0-or-later, matching QMK.

- `bongo_cat.c/.h` is adapted from
  [dancarroll/qmk-bongo](https://github.com/dancarroll/qmk-bongo), with the
  WPM-threshold animation replaced by a tap-driven one.
- The OLED glyph table in `keymap.c` is lifted from QMK's own
  `drivers/oled/glcdfont.c`.
