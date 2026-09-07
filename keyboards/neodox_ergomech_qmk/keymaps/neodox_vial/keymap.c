#include QMK_KEYBOARD_H
#include <string.h>
#include "transactions.h"  // split RPC: not pulled in by QMK_KEYBOARD_H
#include "oneshot.h"
#include "bongo_cat.h"

// Right/slave screen animation, switched at runtime by ANIM_TOG. The cat is
// 128x32 landscape art, the duck a 32x128 column; rotation follows the mode.
enum right_screen_anim { ANIM_DUCK, ANIM_BONGO };
#define ANIM_DEFAULT ANIM_DUCK

// Lives on both halves: the master owns it, and pushes it to the slave (which
// is the half that actually draws) over the split link.
static uint8_t anim_mode = ANIM_DEFAULT;

// Uncomment to draw the animation on the master screen while tuning it.
// #define ANIM_ON_MASTER

// Which half draws the animation; the other draws the layer/mod readout.
static inline bool half_draws_anim(void) {
#ifdef ANIM_ON_MASTER
    return is_keyboard_master();
#else
    return !is_keyboard_master();
#endif
}

// Defined further down with the duck art; declared here because the split tap
// handler above it needs to call it. (bongo_tap() comes from bongo_cat.h.)
#ifdef OLED_ENABLE
void duck_tap(void);
#endif

// Layer order. Index 0 is the power-on default; DF() keys on RAISE switch
// between the three base layouts (QWERTY / Night / Gallium).
enum layers {
    _QWERTY,
    _NIGHT,
    _GALLIUM,
    _GAME,
    _LOWER,
    _RAISE,
    _ADJUST
};

// Custom keycodes: four sticky (one-shot) mods plus a real delete-line.
enum keycodes {
    OS_SHFT = SAFE_RANGE,
    OS_CTRL,
    OS_ALT,
    OS_SUPR,
    DEL_LINE,
    ANIM_TOG
};

// Momentary layer holds; LOWER+RAISE together trigger ADJUST (tri-layer).
#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

// The layouts themselves, 70 keys each, ordered to match the enum above.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_QWERTY] = LAYOUT(
        KC_TAB,  KC_1,    KC_2,   KC_3,   KC_4,   KC_5,                                             KC_6,  KC_7,    KC_8,    KC_9,   KC_0,    KC_BSPC,
        KC_ESC,  KC_Q,    KC_W,   KC_E,   KC_R,   KC_T,  TG(_GAME),                        KC_MPLY, KC_Y,  KC_U,    KC_I,    KC_O,   KC_P,    KC_DEL,
        KC_LCTL, KC_A,    KC_S,   KC_D,   KC_F,   KC_G,  KC_NO,                            KC_NO,   KC_H,  KC_J,    KC_K,    KC_L,   KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,   KC_C,   KC_V,   KC_B,  KC_DEL,    KC_ESC,       KC_LALT, KC_LGUI, KC_N,  KC_M,    KC_COMM, KC_DOT, KC_SLSH, KC_ENT,
                 KC_LALT, KC_TAB, KC_ENT, KC_SPC, LOWER, KC_LSFT,   KC_BSPC,      KC_LCTL, KC_SPC,  RAISE, KC_LEFT, KC_DOWN, KC_UP,  KC_RGHT
    ),

    [_NIGHT] = LAYOUT(
        KC_TAB,  KC_1,    KC_2,   KC_3,   KC_4,   KC_5,                                            KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_ESC,  KC_B,    KC_F,   KC_L,   KC_K,   KC_Q,  TG(_GAME),                       KC_MPLY, KC_P,    KC_G,    KC_O,    KC_U,    KC_DOT,  KC_DEL,
        KC_LCTL, KC_N,    KC_S,   KC_H,   KC_T,   KC_M,  KC_NO,                           KC_NO,   KC_Y,    KC_C,    KC_A,    KC_E,    KC_I,    KC_QUOT,
        KC_LSFT, KC_X,    KC_V,   KC_J,   KC_D,   KC_Z,  KC_DEL,    KC_ESC,      KC_LALT, KC_LGUI, KC_QUOT, KC_W,    KC_SCLN, KC_SLSH, KC_COMM, KC_ENT,
                 KC_LALT, KC_TAB, KC_ENT, KC_SPC, LOWER, KC_LSFT,   KC_R,        KC_LCTL, KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

    [_GALLIUM] = LAYOUT(
        KC_TAB,  KC_1,    KC_2,   KC_3,   KC_4,   KC_5,                                             KC_6,  KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_ESC,  KC_B,    KC_L,   KC_D,   KC_C,   KC_V,  TG(_GAME),                        KC_MPLY, KC_J,  KC_F,    KC_O,    KC_U,    KC_COMM, KC_DEL,
        KC_LCTL, KC_N,    KC_R,   KC_T,   KC_S,   KC_G,  KC_NO,                            KC_NO,   KC_Y,  KC_H,    KC_A,    KC_E,    KC_I,    KC_QUOT,
        KC_LSFT, KC_X,    KC_Q,   KC_M,   KC_W,   KC_Z,  KC_DEL,    KC_ESC,       KC_LALT, KC_LGUI, KC_K,  KC_P,    KC_QUOT, KC_SCLN, KC_DOT,  KC_ENT,
                 KC_LALT, KC_TAB, KC_ENT, KC_SPC, LOWER, KC_LSFT,   KC_BSPC,      KC_LCTL, KC_SPC,  RAISE, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

    [_GAME] = LAYOUT(
        KC_ESC, KC_1,    KC_2,   KC_3, KC_4, KC_5,                                             KC_6,  KC_7,    KC_8,    KC_9,    KC_0,    _______,
        KC_6,   KC_TAB,  KC_Q,   KC_W, KC_E, KC_R,  TG(_GAME),                        KC_MPLY, KC_T,  KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_7,   KC_LCTL, KC_A,   KC_S, KC_D, KC_F,  KC_NO,                            KC_NO,   KC_G,  KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_8,   KC_LSFT, KC_Z,   KC_X, KC_C, KC_V,  KC_ENT,    KC_ESC,       KC_LGUI, KC_LALT, KC_B,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                KC_LALT, KC_TAB, KC_9, KC_0, LOWER, KC_SPC,    KC_BSPC,      KC_LSFT, KC_SPC,  RAISE, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

    [_LOWER] = LAYOUT(
        QK_BOOT, _______, _______, _______, _______, _______,                                            _______, _______, _______, _______, _______, _______,
        _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    TG(_GAME),                        KC_MPLY, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
        KC_VOLU, OS_SUPR, OS_ALT,  OS_CTRL, OS_SHFT, KC_PERC, _______,                          _______, KC_ESC,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______,
        KC_VOLD, KC_MPRV, KC_MNXT, C(KC_C), C(KC_V), KC_MPLY, _______,   _______,      _______, _______, KC_DEL,  KC_BSPC, KC_TAB,  KC_ENT,  KC_SCLN, _______,
                 _______, _______, _______, _______, _______, _______,   _______,      _______, _______, _______, _______, _______, _______, _______
    ),

    [_RAISE] = LAYOUT(
        _______, _______, _______, _______, _______, _______,                                            _______, _______, DF(_QWERTY), DF(_NIGHT), DF(_GALLIUM), _______,
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_TILD, TG(_GAME),                        KC_MPLY, KC_CIRC, KC_AMPR, KC_QUES,     KC_SLSH,    KC_ASTR,      _______,
        _______, KC_LPRN, KC_RPRN, KC_LCBR, KC_RCBR, KC_PERC, _______,                          _______, KC_PIPE, OS_SHFT, OS_CTRL,     OS_ALT,     OS_SUPR,      _______,
        _______, KC_LBRC, KC_RBRC, KC_UNDS, KC_MINS, KC_GRV,  _______,   _______,      _______, _______, KC_BSLS, KC_EQL,  KC_RABK,     KC_LABK,    KC_PLUS,      _______,
                 _______, _______, _______, _______, _______, KC_SPC,    _______,      _______, _______, _______, _______, _______,     _______,    _______
    ),

    [_ADJUST] = LAYOUT(
        ANIM_TOG, _______, _______, _______, _______, _______,                                            _______, _______, _______, _______, _______, _______,
        _______,  KC_BTN2, KC_WH_U, KC_MS_U, KC_WH_D, KC_TAB,  TG(_GAME),                        KC_MPLY, C(KC_N), C(KC_L), C(KC_T), C(KC_W), G(KC_S), _______,
        _______,  KC_BTN1, KC_MS_L, KC_MS_D, KC_MS_R, KC_Q,    _______,                          _______, C(KC_1), C(KC_2), C(KC_3), C(KC_4), C(KC_5), _______,
        _______,  KC_BTN4, KC_BTN5, C(KC_C), C(KC_V), KC_ENT,  _______,   _______,      _______, _______, KC_DEL,  KC_BSPC, KC_LGUI, KC_LALT, _______, _______,
                  _______, _______, _______, _______, _______, _______,   _______,      _______, _______, _______, _______, _______, _______, _______
    )
};
#if defined(ENCODER_MAP_ENABLE)
// Encoders, same on every layer: left scrolls horizontally, right is volume.
const uint16_t PROGMEM encoder_map[][2][2] = {
    [_QWERTY]  = { ENCODER_CCW_CW(KC_WH_R, KC_WH_L), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
    [_NIGHT]   = { ENCODER_CCW_CW(KC_WH_R, KC_WH_L), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
    [_GALLIUM] = { ENCODER_CCW_CW(KC_WH_R, KC_WH_L), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
    [_GAME]    = { ENCODER_CCW_CW(KC_WH_R, KC_WH_L), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
    [_LOWER]   = { ENCODER_CCW_CW(KC_WH_R, KC_WH_L), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
    [_RAISE]   = { ENCODER_CCW_CW(KC_WH_R, KC_WH_L), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
    [_ADJUST]  = { ENCODER_CCW_CW(KC_WH_R, KC_WH_L), ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
};
#endif

// Default combos. Vial owns key_combos[] and keeps combos in EEPROM, so these
// are only seeded on a blank EEPROM; afterwards edit them in the Vial app.
static const vial_combo_entry_t default_combos[] = {
    { .input = { KC_X, KC_C, KC_COMM, KC_DOT }, .output = DEL_LINE },
    { .input = { KC_V, KC_C, 0, 0 },            .output = KC_ESCAPE },
    { .input = { KC_COMM, KC_DOT, 0, 0 },       .output = KC_BSPC },
    { .input = { KC_COMM, KC_M, 0, 0 },         .output = KC_ENTER },
    { .input = { KC_X, KC_C, 0, 0 },            .output = C(KC_BSPC) },

    { .input = { KC_V, KC_J, KC_SCLN, KC_SLSH }, .output = DEL_LINE },
    { .input = { KC_D, KC_J, 0, 0 },             .output = KC_ESCAPE },
    { .input = { KC_SCLN, KC_SLSH, 0, 0 },       .output = KC_BSPC },
    { .input = { KC_SCLN, KC_W, 0, 0 },          .output = KC_ENTER },
    { .input = { KC_V, KC_J, 0, 0 },             .output = C(KC_BSPC) },

    // Gallium, position-translated the same way as the original QWERTY set
    // (same physical row-3 positions: X/C -> Q/M, V -> W, M -> P, COMM/DOT -> QUOT/SCLN).
    { .input = { KC_Q, KC_M, KC_QUOT, KC_SCLN }, .output = DEL_LINE },
    { .input = { KC_W, KC_M, 0, 0 },             .output = KC_ESCAPE },
    { .input = { KC_QUOT, KC_SCLN, 0, 0 },       .output = KC_BSPC },
    { .input = { KC_QUOT, KC_P, 0, 0 },          .output = KC_ENTER },
    { .input = { KC_Q, KC_M, 0, 0 },             .output = C(KC_BSPC) },
};

// The three sets above are seeded in blocks of this size, in layout order.
#define COMBOS_PER_LAYOUT 5

// Combos match on keycodes, not layers - Night's D+J would also fire on
// QWERTY. Scope each seeded block to the layout it was drawn for.
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    // Nothing fires while GAME is toggled on: a stray letter chord mid-game
    // sending Escape or Ctrl+Backspace is the worst case for this.
    if (layer_state_is(_GAME)) {
        return false;
    }

    // Combos added later in the Vial GUI land past the seeded blocks; leave
    // those global rather than guessing which layout they belong to.
    if (combo_index >= COMBOS_PER_LAYOUT * 3) {
        return true;
    }

    uint8_t base = get_highest_layer(default_layer_state);
    switch (combo_index / COMBOS_PER_LAYOUT) {
        case 0:  return base == _QWERTY;
        case 1:  return base == _NIGHT;
        default: return base == _GALLIUM;
    }
}

// The matcher reads these RAM arrays, and they are loaded from EEPROM before
// our seeding runs - so we mirror vial.c's private reload_combo() by hand below.
extern combo_t  key_combos[VIAL_COMBO_ENTRIES];
extern uint16_t key_combos_keys[VIAL_COMBO_ENTRIES][5];

// Runs once on a blank/reset EEPROM: seed the combos above into storage AND
// into RAM, since nothing reloads RAM again this boot.
void eeconfig_init_user(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(default_combos); i++) {
        dynamic_keymap_set_combo(i, &default_combos[i]);

        memcpy(key_combos_keys[i], default_combos[i].input, sizeof(default_combos[i].input));
        key_combos[i].keys    = key_combos_keys[i];
        key_combos[i].keycode = default_combos[i].output;
    }
}

// Holding LOWER and RAISE together activates ADJUST.
layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

// Keys that clear any queued sticky mods.
bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
        case LOWER:
        case RAISE:
            return true;
        default:
            return false;
    }
}

// Keys that don't count as "using" a sticky mod, so mods can be stacked.
bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
        case LOWER:
        case RAISE:
        case KC_LSFT:
        case OS_SHFT:
        case OS_CTRL:
        case OS_ALT:
        case OS_SUPR:
            return true;
        default:
            return false;
    }
}

// The animation lives on the slave, but only the master sees keypresses - so
// the master counts taps and pushes the count over the split link.
static uint8_t tap_count = 0;

// What the master pushes to the slave each time either field changes.
typedef struct {
    uint8_t taps;
    uint8_t anim;
    uint8_t game;
} right_screen_sync_t;

// Game is a toggle layer that is easy to leave on by accident, so both panels
// invert while it is active - unmissable, and one I2C command to switch.
static bool game_active = false;

// Feed a tap to whichever animation is currently showing.
static void right_screen_tap(void) {
#ifdef OLED_ENABLE
    if (anim_mode == ANIM_BONGO) {
        bongo_tap();
    } else {
        duck_tap();
    }
#endif
}

// Both halves time their screens off this. The master stamps it on every key
// and encoder turn; the slave stamps it when that keypress arrives over RPC.
static uint32_t last_activity = 0;

static void note_activity(void) {
    last_activity = timer_read32();
}

static void tap_sync_slave_handler(uint8_t in_len, const void *in_data, uint8_t out_len, void *out_data) {
    const right_screen_sync_t *in = (const right_screen_sync_t *)in_data;

#ifdef OLED_ENABLE
    // Mode change: the two animations need different rotations, so re-init the
    // panel. oled_init() re-derives rotation via oled_init_user() and clears.
    if (in->anim != anim_mode) {
        anim_mode = in->anim;
        oled_init(OLED_ROTATION_270);  // arg is overridden by oled_init_user()
    }
#endif

    game_active = in->game;

    static uint8_t last_seen = 0;
    if (in->taps != last_seen) {
        last_seen = in->taps;
        right_screen_tap();
        note_activity();  // typing wakes this screen too, not just the master's
    }
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(RPC_ID_USER_TAP, tap_sync_slave_handler);
}

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) {
        return;
    }
    game_active = layer_state_is(_GAME);

    static uint8_t last_taps = 0;
    static uint8_t last_anim = 0xFF;
    static uint8_t last_game = 0xFF;
    if (tap_count != last_taps || anim_mode != last_anim || game_active != last_game) {
        right_screen_sync_t out = {.taps = tap_count, .anim = anim_mode, .game = game_active};
        if (transaction_rpc_send(RPC_ID_USER_TAP, sizeof(out), &out)) {
            last_taps = tap_count;
            last_anim = anim_mode;
            last_game = game_active;
        }
    }
}

// Live state of each sticky mod (also drives the left OLED indicators).
oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state  = os_up_unqueued;
oneshot_state os_supr_state = os_up_unqueued;

#ifdef OLED_ENABLE
// Label for the last key pressed, shown on the card. Deliberately naive: a
// combo shows its trigger keys first, then the keycode the combo produces.
#define KEY_LABEL_MAX 5
static char last_key[KEY_LABEL_MAX] = "";

// Unshifted then shifted printable for the number row and punctuation, in
// keycode order from KC_1 through KC_SLASH.
static const char key_sym[][2] = {
    {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'},
    {'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'},
};
static const char key_punct[][2] = {
    {'-', '_'}, {'=', '+'}, {'[', '{'}, {']', '}'}, {0x5C, '|'},
    {'#', '~'}, {';', ':'}, {0x27, '"'}, {'`', '~'}, {',', '<'},
    {'.', '>'}, {'/', '?'},
};

// Fills last_key, or leaves it alone for keycodes with nothing useful to show
// (mods, layer keys, anything Vial remapped beyond basic keycodes).
static void set_key_label(uint16_t keycode) {
    bool shift = (get_mods() | get_weak_mods()) & MOD_MASK_SHIFT;
    const char *named = NULL;

    if (keycode >= KC_A && keycode <= KC_Z) {
        last_key[0] = (shift ? 'A' : 'a') + (keycode - KC_A);
        last_key[1] = 0;
        return;
    }
    if (keycode >= KC_1 && keycode <= KC_0) {
        last_key[0] = key_sym[keycode - KC_1][shift ? 1 : 0];
        last_key[1] = 0;
        return;
    }
    if (keycode >= KC_MINUS && keycode <= KC_SLASH) {
        last_key[0] = key_punct[keycode - KC_MINUS][shift ? 1 : 0];
        last_key[1] = 0;
        return;
    }

    switch (keycode) {
        case KC_ENTER:     named = "ENT";  break;
        case KC_ESCAPE:    named = "ESC";  break;
        case KC_BSPC:      named = "BSP";  break;
        case KC_TAB:       named = "TAB";  break;
        case KC_SPACE:     named = "SPC";  break;
        case KC_DELETE:    named = "DEL";  break;
        case KC_HOME:      named = "HOM";  break;
        case KC_END:       named = "END";  break;
        case KC_PGUP:      named = "PGU";  break;
        case KC_PGDN:      named = "PGD";  break;
        case KC_LEFT:      named = "LFT";  break;
        case KC_RIGHT:     named = "RGT";  break;
        case KC_UP:        named = "UP";   break;
        case KC_DOWN:      named = "DWN";  break;
        case DEL_LINE:     named = "DLN";  break;
        case ANIM_TOG:     named = "ANIM"; break;
        default: return;  // nothing sensible to show; keep the previous key
    }
    uint8_t i = 0;
    while (named[i] != 0 && i < KEY_LABEL_MAX - 1) {
        last_key[i] = named[i];
        i++;
    }
    last_key[i] = 0;
}
#endif

// DEL_LINE selects the whole line and deletes it; everything else just
// feeds the four sticky-mod state machines.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        tap_count++;
        note_activity();
#ifdef OLED_ENABLE
        set_key_label(keycode);
#endif
#ifdef ANIM_ON_MASTER
        // process_record_user only runs on the master, so when the master
        // draws the animation, feed it directly - no split round trip.
        right_screen_tap();
#endif
    }

    if (keycode == ANIM_TOG) {
        if (record->event.pressed) {
            anim_mode = (anim_mode == ANIM_BONGO) ? ANIM_DUCK : ANIM_BONGO;
        }
        return false;
    }

    if (keycode == DEL_LINE) {
        if (record->event.pressed) {
            // Select from line start through the newline, then delete. The
            // waits matter: apps that filter synthetic input drop fast taps.
            tap_code(KC_HOME);
            wait_ms(DEL_LINE_STEP_MS);
            register_code(KC_LSFT);
            wait_ms(DEL_LINE_STEP_MS);
            tap_code(KC_DOWN);
            wait_ms(DEL_LINE_STEP_MS);
            unregister_code(KC_LSFT);
            wait_ms(DEL_LINE_STEP_MS);
            tap_code(KC_BSPC);
        }
        return false;
    }

    update_oneshot(
        &os_shft_state, KC_LSFT, OS_SHFT,
        keycode, record
    );

    update_oneshot(
        &os_ctrl_state, KC_LCTL, OS_CTRL,
        keycode, record
    );

    update_oneshot(
        &os_alt_state, KC_LALT, OS_ALT,
        keycode, record
    );

    update_oneshot(
        &os_supr_state, KC_LGUI, OS_SUPR,
        keycode, record
    );

    return true;
}

#ifdef OLED_ENABLE
// Rotation follows the animation: bongo is landscape, the duck portrait.
// Swap 180 for 0 to flip which end the cat's table sits at.
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (half_draws_anim() && anim_mode == ANIM_BONGO) {
        return OLED_ROTATION_180;
    }
    return OLED_ROTATION_270;
}

// Panel geometry in the rotated (portrait) space both screens draw in.
#define SCREEN_W 32
#define SCREEN_H 128

// The OLED driver offers a fixed 6x8 text grid and a single pixel plot - no
// shape API - so the framing below is drawn a pixel at a time.
static void draw_hline(uint8_t x0, uint8_t x1, uint8_t y, bool on) {
    for (uint8_t x = x0; x <= x1; x++) {
        oled_write_pixel(x, y, on);
    }
}

static void draw_vline(uint8_t x, uint8_t y0, uint8_t y1, bool on) {
    for (uint8_t y = y0; y <= y1; y++) {
        oled_write_pixel(x, y, on);
    }
}

static void draw_fill(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool on) {
    for (uint8_t y = y0; y <= y1; y++) {
        draw_hline(x0, x1, y, on);
    }
}

// Knocking the four corner pixels back out is what makes a box read as
// rounded at this size.
static void draw_corners(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    oled_write_pixel(x0, y0, false);
    oled_write_pixel(x1, y0, false);
    oled_write_pixel(x0, y1, false);
    oled_write_pixel(x1, y1, false);
}

static void draw_round_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    draw_hline(x0, x1, y0, true);
    draw_hline(x0, x1, y1, true);
    draw_vline(x0, y0, y1, true);
    draw_vline(x1, y0, y1, true);
    draw_corners(x0, y0, x1, y1);
}

// Dotted rule: separates the layer block from the mods, and closes the bottom.
static void draw_dotted(uint8_t y) {
    for (uint8_t x = 0; x < SCREEN_W; x += 2) {
        oled_write_pixel(x, y, true);
    }
}

// Left/master screen: last key pressed, with the layer above it and the four
// sticky mods below. Glyphs are drawn by hand so everything centres exactly.
#define UI_GLYPH_W   5
#define UI_GLYPH_ADV 6

// Printable ASCII lifted from QMK's own OLED font (drivers/oled/glcdfont.c,
// GPL-2.0) so the lettering matches the rest of the firmware.
#define UI_FONT_FIRST ' '
#define UI_FONT_LAST  '~'
static const uint8_t ui_font[][UI_GLYPH_W] = {
    {0x00, 0x00, 0x00, 0x00, 0x00},  // space
    {0x00, 0x00, 0x5F, 0x00, 0x00},  // !
    {0x00, 0x07, 0x00, 0x07, 0x00},  // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14},  // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12},  // $
    {0x23, 0x13, 0x08, 0x64, 0x62},  // %
    {0x36, 0x49, 0x56, 0x20, 0x50},  // &
    {0x00, 0x08, 0x07, 0x03, 0x00},  // '
    {0x00, 0x1C, 0x22, 0x41, 0x00},  // (
    {0x00, 0x41, 0x22, 0x1C, 0x00},  // )
    {0x2A, 0x1C, 0x7F, 0x1C, 0x2A},  // *
    {0x08, 0x08, 0x3E, 0x08, 0x08},  // +
    {0x00, 0x80, 0x70, 0x30, 0x00},  // ,
    {0x08, 0x08, 0x08, 0x08, 0x08},  // -
    {0x00, 0x00, 0x60, 0x60, 0x00},  // .
    {0x20, 0x10, 0x08, 0x04, 0x02},  // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E},  // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00},  // 1
    {0x72, 0x49, 0x49, 0x49, 0x46},  // 2
    {0x21, 0x41, 0x49, 0x4D, 0x33},  // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10},  // 4
    {0x27, 0x45, 0x45, 0x45, 0x39},  // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x31},  // 6
    {0x41, 0x21, 0x11, 0x09, 0x07},  // 7
    {0x36, 0x49, 0x49, 0x49, 0x36},  // 8
    {0x46, 0x49, 0x49, 0x29, 0x1E},  // 9
    {0x00, 0x00, 0x14, 0x00, 0x00},  // :
    {0x00, 0x40, 0x34, 0x00, 0x00},  // ;
    {0x00, 0x08, 0x14, 0x22, 0x41},  // <
    {0x14, 0x14, 0x14, 0x14, 0x14},  // =
    {0x00, 0x41, 0x22, 0x14, 0x08},  // >
    {0x02, 0x01, 0x59, 0x09, 0x06},  // ?
    {0x3E, 0x41, 0x5D, 0x59, 0x4E},  // @
    {0x7C, 0x12, 0x11, 0x12, 0x7C},  // A
    {0x7F, 0x49, 0x49, 0x49, 0x36},  // B
    {0x3E, 0x41, 0x41, 0x41, 0x22},  // C
    {0x7F, 0x41, 0x41, 0x41, 0x3E},  // D
    {0x7F, 0x49, 0x49, 0x49, 0x41},  // E
    {0x7F, 0x09, 0x09, 0x09, 0x01},  // F
    {0x3E, 0x41, 0x41, 0x51, 0x73},  // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F},  // H
    {0x00, 0x41, 0x7F, 0x41, 0x00},  // I
    {0x20, 0x40, 0x41, 0x3F, 0x01},  // J
    {0x7F, 0x08, 0x14, 0x22, 0x41},  // K
    {0x7F, 0x40, 0x40, 0x40, 0x40},  // L
    {0x7F, 0x02, 0x1C, 0x02, 0x7F},  // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F},  // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E},  // O
    {0x7F, 0x09, 0x09, 0x09, 0x06},  // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E},  // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46},  // R
    {0x26, 0x49, 0x49, 0x49, 0x32},  // S
    {0x03, 0x01, 0x7F, 0x01, 0x03},  // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F},  // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F},  // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F},  // W
    {0x63, 0x14, 0x08, 0x14, 0x63},  // X
    {0x03, 0x04, 0x78, 0x04, 0x03},  // Y
    {0x61, 0x59, 0x49, 0x4D, 0x43},  // Z
    {0x00, 0x7F, 0x41, 0x41, 0x41},  // [
    {0x02, 0x04, 0x08, 0x10, 0x20},  // backslash
    {0x00, 0x41, 0x41, 0x41, 0x7F},  // ]
    {0x04, 0x02, 0x01, 0x02, 0x04},  // ^
    {0x40, 0x40, 0x40, 0x40, 0x40},  // _
    {0x00, 0x03, 0x07, 0x08, 0x00},  // `
    {0x20, 0x54, 0x54, 0x78, 0x40},  // a
    {0x7F, 0x28, 0x44, 0x44, 0x38},  // b
    {0x38, 0x44, 0x44, 0x44, 0x28},  // c
    {0x38, 0x44, 0x44, 0x28, 0x7F},  // d
    {0x38, 0x54, 0x54, 0x54, 0x18},  // e
    {0x00, 0x08, 0x7E, 0x09, 0x02},  // f
    {0x18, 0xA4, 0xA4, 0x9C, 0x78},  // g
    {0x7F, 0x08, 0x04, 0x04, 0x78},  // h
    {0x00, 0x44, 0x7D, 0x40, 0x00},  // i
    {0x20, 0x40, 0x40, 0x3D, 0x00},  // j
    {0x7F, 0x10, 0x28, 0x44, 0x00},  // k
    {0x00, 0x41, 0x7F, 0x40, 0x00},  // l
    {0x7C, 0x04, 0x78, 0x04, 0x78},  // m
    {0x7C, 0x08, 0x04, 0x04, 0x78},  // n
    {0x38, 0x44, 0x44, 0x44, 0x38},  // o
    {0xFC, 0x18, 0x24, 0x24, 0x18},  // p
    {0x18, 0x24, 0x24, 0x18, 0xFC},  // q
    {0x7C, 0x08, 0x04, 0x04, 0x08},  // r
    {0x48, 0x54, 0x54, 0x54, 0x24},  // s
    {0x04, 0x04, 0x3F, 0x44, 0x24},  // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C},  // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C},  // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C},  // w
    {0x44, 0x28, 0x10, 0x28, 0x44},  // x
    {0x4C, 0x90, 0x90, 0x90, 0x7C},  // y
    {0x44, 0x64, 0x54, 0x4C, 0x44},  // z
    {0x00, 0x08, 0x36, 0x41, 0x00},  // {
    {0x00, 0x00, 0x77, 0x00, 0x00},  // |
    {0x00, 0x41, 0x36, 0x08, 0x00},  // }
    {0x02, 0x01, 0x02, 0x04, 0x02},  // ~
};

static uint8_t text_width(const char *s) {
    return (uint8_t)(strlen(s) * UI_GLYPH_ADV - 1);
}

static void draw_text(uint8_t x, uint8_t y, const char *s, bool ink) {
    for (; *s; s++, x += UI_GLYPH_ADV) {
        if (*s < UI_FONT_FIRST || *s > UI_FONT_LAST) {
            continue;  // outside the table; leave a blank rather than garbage
        }
        const uint8_t *cols = ui_font[*s - UI_FONT_FIRST];
        for (uint8_t i = 0; i < UI_GLYPH_W; i++) {
            for (uint8_t b = 0; b < 8; b++) {
                if (cols[i] & (1 << b)) {
                    oled_write_pixel(x + i, y + b, ink);
                }
            }
        }
    }
}

static void draw_text_centered(uint8_t y, const char *s, bool ink) {
    draw_text((SCREEN_W - text_width(s)) / 2, y, s, ink);
}

// One visual language: the layer name sits in the same rounded pill as the
// mods, just taller, and the caption is the only plain text on the screen.
#define UI_CAPTION_Y 11
#define UI_CARD_TOP  23
#define UI_CARD_BOT  44
#define UI_NAME_Y    30
#define UI_RULE_Y    50
#define UI_PILL_H    12
#define UI_TEXT_INSET 2

static const uint8_t ui_pill_top[] = {57, 73, 89, 105};

static const char *layer_name(uint8_t layer) {
    switch (layer) {
        case _QWERTY:
            return "QWRT";
        case _NIGHT:
            return "NIGT";
        case _GALLIUM:
            return "GALL";
        case _GAME:
            return "GAME";
        case _LOWER:
            return "LOWR";
        case _RAISE:
            return "RAIS";
        case _ADJUST:
            return "ADJT";
        default:
            return "????";
    }
}

static void render_mod_pill(uint8_t idx, const char *label, bool active) {
    uint8_t top = ui_pill_top[idx];
    uint8_t bot = top + UI_PILL_H - 1;
    if (active) {
        draw_fill(0, top, SCREEN_W - 1, bot, true);
        draw_corners(0, top, SCREEN_W - 1, bot);
    } else {
        draw_round_rect(0, top, SCREEN_W - 1, bot);
    }
    // Ink flips on a filled pill, so the label reads as knocked out of it.
    draw_text_centered(top + UI_TEXT_INSET, label, !active);
}

// Buffer blocks are 8 logical rows tall, so redrawing just the card interior
// dirties 3 of 16 instead of all of them. Border rows are left alone.
static void render_key_card(void) {
    draw_fill(1, UI_CARD_TOP + 1, SCREEN_W - 2, UI_CARD_BOT - 1, false);
    draw_text_centered(UI_NAME_Y, last_key, true);
}

static void render_layer_status(void) {
    // layer_state is 0 with no overlay held, so fall back to default_layer_state
    // or this always reads "layer 0" regardless of what DF() selected.
    uint8_t active_layer = layer_state ? get_highest_layer(layer_state) : get_highest_layer(default_layer_state);
    // Master-only state, so no split sync needed.
    uint8_t mod_bits = (os_shft_state != os_up_unqueued ? 1 : 0)
                     | (os_ctrl_state != os_up_unqueued ? 2 : 0)
                     | (os_alt_state  != os_up_unqueued ? 4 : 0)
                     | (os_supr_state != os_up_unqueued ? 8 : 0);

    // The frame is a few hundred pixel writes plus a clear, so redraw only when
    // something actually changed rather than on every scan.
    static uint8_t last_layer = 0xFF;
    static uint8_t last_mods  = 0xFF;
    static char    last_shown[KEY_LABEL_MAX] = "";
    bool frame = (active_layer != last_layer) || (mod_bits != last_mods);
    bool key   = strcmp(last_shown, last_key) != 0;
    if (!frame && !key) {
        return;
    }
    last_layer = active_layer;
    last_mods  = mod_bits;
    memcpy(last_shown, last_key, KEY_LABEL_MAX);

    // Typing only changes the card, and that happens on every keystroke, so
    // keep the full repaint for the rarer layer and modifier changes.
    if (!frame) {
        render_key_card();
        return;
    }

    // Clearing is required, not just tidy: a pill going from filled back to
    // outlined would otherwise keep its old fill.
    oled_clear();

    // Layer as the caption, last key pressed in the card below it.
    draw_text_centered(UI_CAPTION_Y, layer_name(active_layer), true);

    draw_round_rect(0, UI_CARD_TOP, SCREEN_W - 1, UI_CARD_BOT);
    draw_text_centered(UI_NAME_Y, last_key, true);

    draw_dotted(UI_RULE_Y);

    render_mod_pill(0, "Sft", mod_bits & 1);
    render_mod_pill(1, "Ctl", mod_bits & 2);
    render_mod_pill(2, "Alt", mod_bits & 4);
    render_mod_pill(3, "Sup", mod_bits & 8);
}

// Sailor duck, a 1:1 copy of the reference art. The reference is itself pixel
// art on a 25px grid, sampled here at one screen pixel per cell.
#define DUCK_SCALE 1
#define DUCK_COLS 28
#define DUCK_ROWS 32
#define DUCK_X_OFFSET 2
#define DUCK_Y_OFFSET 62
static const uint8_t duck_bitmap[DUCK_ROWS][DUCK_COLS] = {
    {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0},
    {0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0},
    {0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0},
    {0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,1,0},
    {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0,0,1,1},
    {0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1},
    {0,0,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,1},
    {0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,0},
    {0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0},
    {0,0,1,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,0,0},
    {0,0,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0},
    {0,0,0,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
};

// Scene, top to bottom: sky with clouds and gulls, the duck on the surface,
// open water, then the seabed. The bitmap is scaled by DUCK_SCALE.
#define WATER_Y (DUCK_Y_OFFSET + 30)
#define SEABED_Y 119

// A fixed bob cycle: starts on the first keystroke and keeps looping until
// ANIM_HOLD_MS after the last one, always finishing the cycle it is in.
#define BOB_FRAME_MS 110   // how long each step of the cycle holds
#define ANIM_HOLD_MS 10000 // keep bobbing this long after the last keystroke

static const uint8_t bob_cycle[] = {0, 1, 2, 2, 1, 0};
#define BOB_FRAMES (sizeof(bob_cycle) / sizeof(bob_cycle[0]))

static uint32_t duck_last_tap   = 0;
static uint32_t duck_frame_time = 0;
static uint8_t  duck_frame      = 0;
static bool     duck_animating  = false;
static uint8_t  duck_ripple     = 0;  // wave phase, flipped each bob frame

// Clouds drift while you type and simply hold position when you stop - unlike
// the gulls they read fine as static scenery.
#define CLOUD_W 7
#define CLOUD_DRIFT_FRAMES 6
static const uint8_t cloud_shape[2][CLOUD_W] = {
    {0, 0, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0},
};
#define CLOUD_COUNT 3
// Randomised once at first keypress so the sky isn't identical every boot.
static uint8_t cloud_rows[CLOUD_COUNT]  = {6, 26, 46};
static uint8_t cloud_phase[CLOUD_COUNT] = {0, 19, 9};

static uint8_t cloud_shift = 0;
static uint8_t cloud_tick  = 0;

// Gulls only ever exist mid-flight: one spawns while the animation is running,
// crosses the sky and is gone. Nothing hovers when you stop typing.
#define GULL_W 5
#define GULL_H 2
#define GULL_GAP_FRAMES 14
static const uint8_t gull_frames[2][GULL_H][GULL_W] = {
    {{1, 1, 0, 1, 1}, {0, 0, 1, 0, 0}},  // wings up
    {{0, 0, 1, 0, 0}, {1, 1, 0, 1, 1}},  // wings down
};
// Bounds for a gull's flight height; the exact value is picked per crossing.
#define GULL_Y_MIN 4
#define GULL_Y_MAX 56

static int16_t gull_x     = -99;  // -99 = no gull in flight
static uint8_t gull_y     = 6;
static uint8_t gull_flap  = 0;
static uint8_t gull_gap   = 0;
static uint8_t gull_speed = 1;

// Tiny xorshift PRNG. Seeded from the timer on the first keypress, so the
// sequence differs between power-ups rather than replaying the same sky.
static uint16_t rng_state = 0;
static uint8_t rnd(uint8_t range) {
    rng_state ^= (uint16_t)(rng_state << 7);
    rng_state ^= (uint16_t)(rng_state >> 9);
    rng_state ^= (uint16_t)(rng_state << 8);
    return range ? (uint8_t)(rng_state % range) : 0;
}

static void scatter_clouds(void) {
    for (uint8_t i = 0; i < CLOUD_COUNT; i++) {
        cloud_rows[i]  = (uint8_t)(4 + i * 18 + rnd(9));   // keeps them apart
        cloud_phase[i] = rnd(SCREEN_W + CLOUD_W);
    }
}

// Fed from the master over the split link on every keypress.
void duck_tap(void) {
    if (rng_state == 0) {  // first keypress: seed and lay out the sky
        rng_state = (uint16_t)timer_read32() | 1u;
        scatter_clouds();
    }
    duck_last_tap = timer_read32();
    if (!duck_animating) {
        duck_animating  = true;
        duck_frame      = 0;
        duck_frame_time = timer_read32();
    }
}

// Blit a sprite, clipping anything that falls outside the panel.
static void draw_sprite(const uint8_t *sprite, uint8_t w, uint8_t h, int16_t x, int16_t y) {
    for (uint8_t r = 0; r < h; r++) {
        for (uint8_t c = 0; c < w; c++) {
            if (!sprite[r * w + c]) {
                continue;
            }
            int16_t px = x + c;
            int16_t py = y + r;
            if (px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H) {
                oled_write_pixel((uint8_t)px, (uint8_t)py, true);
            }
        }
    }
}

static void render_sky(void) {
    // Clouds at staggered heights, wrapping around as they drift.
    for (uint8_t i = 0; i < CLOUD_COUNT; i++) {
        int16_t x = (int16_t)((cloud_shift + cloud_phase[i]) % (SCREEN_W + CLOUD_W)) - CLOUD_W;
        draw_sprite(&cloud_shape[0][0], CLOUD_W, 2, x, cloud_rows[i]);
    }

    if (gull_x != -99) {
        draw_sprite(&gull_frames[gull_flap][0][0], GULL_W, GULL_H, gull_x, gull_y);
    }
}

// Surface crests, then sparser marks going down for open water, then seabed.
static void render_ocean(uint8_t ripple) {
    for (uint8_t x = 0; x < SCREEN_W; x++) {
        uint8_t t    = (x + ripple * 2) % 8;
        uint8_t rise = (t < 4) ? t : (8 - t);  // 0..4..0
        oled_write_pixel(x, WATER_Y + (rise / 2), true);
        if (x % 4 != 3) {  // dashed second line, reads as depth
            oled_write_pixel(x, WATER_Y + 5 + ((rise + 2) / 3), true);
        }
    }

    for (uint8_t y = WATER_Y + 12; y < SEABED_Y - 2; y += 8) {
        for (uint8_t x = (uint8_t)((y + ripple * 2) % 5); x < SCREEN_W; x += 6) {
            oled_write_pixel(x, y, true);
            oled_write_pixel(x + 1, y, true);
        }
    }

    for (uint8_t x = 0; x < SCREEN_W; x++) {
        oled_write_pixel(x, SEABED_Y, true);
    }
    for (uint8_t x = 0; x < SCREEN_W; x += 2) {
        oled_write_pixel(x, SEABED_Y + 3, true);
    }
    for (uint8_t x = 1; x < SCREEN_W; x += 3) {
        oled_write_pixel(x, SEABED_Y + 6, true);
    }
}

static void render_duck(void) {
    bool changed = false;

    // The duck's cycle only advances while animating.
    if (duck_animating && timer_elapsed32(duck_frame_time) >= BOB_FRAME_MS) {
        duck_frame_time = timer_read32();
        duck_frame      = (duck_frame + 1) % BOB_FRAMES;
        duck_ripple ^= 1;
        changed = true;

        if (++cloud_tick >= CLOUD_DRIFT_FRAMES) {
            cloud_tick = 0;
            cloud_shift++;
        }

        // Spawn a gull only while typing, and only when none is crossing.
        if (gull_x == -99) {
            if (gull_gap > 0) {
                gull_gap--;
            } else {
                gull_x     = -GULL_W;
                gull_y     = (uint8_t)(GULL_Y_MIN + rnd(GULL_Y_MAX - GULL_Y_MIN));
                gull_speed = (uint8_t)(1 + rnd(2));            // 1 or 2 px a frame
                gull_gap   = (uint8_t)(GULL_GAP_FRAMES + rnd(20));
            }
        }

        // Only allowed to stop at the end of a cycle, so it never freezes
        // mid-bob - so the real stop is up to one cycle past ANIM_HOLD_MS.
        if (duck_frame == 0 && timer_elapsed32(duck_last_tap) > ANIM_HOLD_MS) {
            duck_animating = false;
        }
    }

    // A gull already in the air keeps going on its own clock, so it always
    // finishes crossing instead of hanging in the sky when you stop typing.
    static uint32_t gull_time = 0;
    if (gull_x != -99 && timer_elapsed32(gull_time) >= BOB_FRAME_MS) {
        gull_time = timer_read32();
        gull_x += gull_speed;
        gull_flap ^= 1;
        changed = true;
        if (gull_x >= SCREEN_W) {
            gull_x = -99;  // gap for the next one was rolled at spawn
        }
    }

    uint8_t bob = duck_animating ? bob_cycle[duck_frame] : 0;

    // Only redraw when something moved - the scene is over a thousand pixel
    // writes plus a clear, which would saturate the I2C bus every scan.
    static uint8_t last_bob = 255;
    if (bob != last_bob) {
        changed  = true;
        last_bob = bob;
    }
    if (!changed) {
        return;
    }

    oled_clear();
    render_sky();

    for (uint8_t row = 0; row < DUCK_ROWS; row++) {
        uint8_t y = DUCK_Y_OFFSET + row * DUCK_SCALE + bob;
        if (y >= WATER_Y) {
            break;  // below the surface
        }
        for (uint8_t col = 0; col < DUCK_COLS; col++) {
            if (!duck_bitmap[row][col]) {
                continue;
            }
            for (uint8_t dy = 0; dy < DUCK_SCALE; dy++) {
                for (uint8_t dx = 0; dx < DUCK_SCALE; dx++) {
                    if (y + dy < WATER_Y) {
                        oled_write_pixel(DUCK_X_OFFSET + col * DUCK_SCALE + dx, y + dy, true);
                    }
                }
            }
        }
    }

    render_ocean(duck_ripple);
}

// Both directions are done here rather than with the panel's own fade command,
// so the timing is exact and identical on the two halves.
static void screen_power_task(void) {
    static bool     lit        = true;
    static bool     fading_out = false;
    static uint32_t wake_at    = 0;

    uint32_t idle = timer_elapsed32(last_activity);

    if (idle >= SCREEN_ON_MS + SCREEN_FADE_MS) {
        if (lit) {
            oled_off();
            lit = false;
        }
        return;
    }

    if (idle >= SCREEN_ON_MS) {
        uint32_t step = idle - SCREEN_ON_MS;
        fading_out = true;
        oled_set_brightness((uint8_t)(OLED_BRIGHTNESS - (uint32_t)OLED_BRIGHTNESS * step / SCREEN_FADE_MS));
        return;
    }

    if (!lit) {
        oled_set_brightness(0);
        oled_on();
        lit     = true;
        wake_at = timer_read32();
    } else if (fading_out) {
        // Woken mid-fade: start the ramp from the level the fade reached, so
        // the brightness carries on smoothly instead of snapping back.
        wake_at = timer_read32() - ((uint32_t)oled_get_brightness() * SCREEN_FADE_MS / OLED_BRIGHTNESS);
    }
    fading_out = false;

    uint32_t since = timer_elapsed32(wake_at);
    oled_set_brightness(since >= SCREEN_FADE_MS
                        ? OLED_BRIGHTNESS
                        : (uint8_t)((uint32_t)OLED_BRIGHTNESS * since / SCREEN_FADE_MS));
}

bool oled_task_user(void) {
    screen_power_task();
    oled_invert(game_active);  // no-op unless the state actually changed

    if (!half_draws_anim()) {
        render_layer_status();
    } else if (anim_mode == ANIM_BONGO) {
        render_bongo_cat();
    } else {
        render_duck();
    }
    return false;
}
#endif
