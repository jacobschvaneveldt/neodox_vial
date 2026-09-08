# neodox_vial

Vial keymap for the Neodox Ergomech, a split RP2040 board with an OLED on each half.
A [QMK Userspace](https://docs.qmk.fm/newbs_external_userspace) repo, so it holds the keymap only.

## Build

Needs [vial-qmk](https://github.com/Cyboard-DigitalTailor/vial-qmk) on branch `cyboard`, with the
[board definition](https://github.com/ergomechstore/neodox-ergomech-qmk) at `keyboards/neodox_ergomech_qmk/`.

    qmk config user.overlay_dir="/path/to/neodox_vial"
    cd /path/to/vial-qmk && qmk userspace-compile

Run it from the vial-qmk root. `QMK_FIRMWARE` is the working directory, so from anywhere else
it falls back to `~/qmk_firmware` and reports `Invalid keyboard`.

## Flash

Hold BOOT, tap RESET, copy the `.uf2` onto the drive that appears. Both halves run the same
firmware. After changing `DYNAMIC_KEYMAP_LAYER_COUNT`, flash both and reset the EEPROM, or the
stored combos are left pointing at the wrong addresses.

## Credits

GPL-2.0. Bongo cat adapted from [dancarroll/qmk-bongo](https://github.com/dancarroll/qmk-bongo),
OLED glyphs taken from QMK's `glcdfont.c`.
