#pragma once

#define VIAL_KEYBOARD_UID {0x54, 0xB9, 0x18, 0x3D, 0xE9, 0x29, 0xA8, 0x22}
#define FORCE_NKRO

#define DYNAMIC_KEYMAP_LAYER_COUNT 7

// Carries a keypress counter to the slave so its cat can tap per key.
#define SPLIT_TRANSACTION_IDS_USER RPC_ID_USER_TAP

// The driver's own timeout cuts to black with no warning and, because drawing
// counts as activity, fires later on the animated half. Handled in keymap.c.
#define OLED_TIMEOUT 0

// How long the screens stay lit after the last key, and how long they take to
// fade out and back in. Both halves work off the same keystroke clock.
#define SCREEN_ON_MS   30000
#define SCREEN_FADE_MS 1500

// Pause between the keystrokes DEL_LINE sends. Raise if it still misfires in
// a stubborn app; lower if the delete feels sluggish.
#define DEL_LINE_STEP_MS 12

#define COMBO_TERM 35

// Enables the combo_should_trigger() hook, used to scope each seeded combo
// set to the layout it was drawn for.
#define COMBO_SHOULD_TRIGGER

#undef MOUSEKEY_DELAY
#define MOUSEKEY_DELAY          0
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL       6
#undef MOUSEKEY_WHEEL_DELAY
#define MOUSEKEY_WHEEL_DELAY    0
#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED      6
#undef MOUSEKEY_TIME_TO_MAX
#define MOUSEKEY_TIME_TO_MAX    100
