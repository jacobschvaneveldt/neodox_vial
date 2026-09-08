# neodox_vial

## Build

Needs [vial-qmk](https://github.com/Cyboard-DigitalTailor/vial-qmk) on branch `cyboard`, with the
[board definition](https://github.com/ergomechstore/neodox-ergomech-qmk) at `keyboards/neodox_ergomech_qmk/`.

    qmk config user.overlay_dir="/path/to/neodox_vial"
    cd /path/to/vial-qmk && qmk userspace-compile

Run it from the vial-qmk root.

## Flash

Hold BOOT, tap RESET, copy the `.uf2` onto the drive that appears.

## Credits

Bongo cat [dancarroll/qmk-bongo](https://github.com/dancarroll/qmk-bongo)