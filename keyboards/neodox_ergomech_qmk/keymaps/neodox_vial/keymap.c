#include QMK_KEYBOARD_H
#include <string.h>
#include "transactions.h"  // split RPC: not pulled in by QMK_KEYBOARD_H
#include "oneshot.h"
#include "bongo_cat.h"

enum right_screen_anim { ANIM_DUCK, ANIM_BONGO };
#define ANIM_DEFAULT ANIM_DUCK

static uint8_t anim_mode = ANIM_DEFAULT;

// Uncomment to draw the animation on the master screen while tuning it.
// #define ANIM_ON_MASTER

static inline bool half_draws_anim(void) {
#ifdef ANIM_ON_MASTER
    return is_keyboard_master();
#else
    return !is_keyboard_master();
#endif
}

// Defined with the duck art below; declared here for the tap handler above.
#ifdef OLED_ENABLE
void duck_tap(void);
#endif

enum layers {
    _QWERTY,
    _NIGHT,
    _GALLIUM,
    _GAME,
    _LOWER,
    _RAISE,
    _ADJUST
};

enum keycodes {
    OS_SHFT = SAFE_RANGE,
    OS_CTRL,
    OS_ALT,
    OS_SUPR,
    DEL_LINE,
    ANIM_TOG
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_QWERTY] = LAYOUT(
        KC_TAB,  KC_1,    KC_2,   KC_3,   KC_4,   KC_5,                                             KC_6,  KC_7,    KC_8,    KC_9,   KC_0,    KC_BSPC,
        KC_ESC,  KC_Q,    KC_W,   KC_E,   KC_R,   KC_T,  TG(_GAME),                        KC_MPLY, KC_Y,  KC_U,    KC_I,    KC_O,   KC_P,    KC_DEL,
        KC_LCTL, KC_A,    KC_S,   KC_D,   KC_F,   KC_G,  KC_NO,                            KC_NO,   KC_H,  KC_J,    KC_K,    KC_L,   KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,   KC_C,   KC_V,   KC_B,  KC_TAB,    KC_ENT,       KC_LALT, KC_LGUI, KC_N,  KC_M,    KC_COMM, KC_DOT, KC_SLSH, KC_ENT,
                 KC_LALT, KC_TAB, KC_ENT, KC_SPC, LOWER, KC_LSFT,   KC_BSPC,      KC_LCTL, KC_SPC,  RAISE, KC_LEFT, KC_DOWN, KC_UP,  KC_RGHT
    ),

    [_NIGHT] = LAYOUT(
        KC_TAB,  KC_1,    KC_2,   KC_3,   KC_4,   KC_5,                                            KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_ESC,  KC_B,    KC_F,   KC_L,   KC_K,   KC_Q,  TG(_GAME),                       KC_MPLY, KC_P,    KC_G,    KC_O,    KC_U,    KC_COMM, KC_DEL,
        KC_LCTL, KC_N,    KC_S,   KC_H,   KC_T,   KC_M,  KC_NO,                           KC_NO,   KC_Y,    KC_C,    KC_A,    KC_E,    KC_I,    KC_QUOT,
        KC_LSFT, KC_X,    KC_V,   KC_J,   KC_D,   KC_Z,  KC_TAB,    KC_ENT,      KC_LALT, KC_LGUI, KC_QUOT, KC_W,    KC_SCLN, KC_SLSH, KC_DOT,  KC_ENT,
                 KC_LALT, KC_TAB, KC_ENT, KC_SPC, LOWER, KC_LSFT,   KC_R,        KC_LCTL, KC_SPC,  RAISE,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

    [_GALLIUM] = LAYOUT(
        KC_TAB,  KC_1,    KC_2,   KC_3,   KC_4,   KC_5,                                             KC_6,  KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_ESC,  KC_B,    KC_L,   KC_D,   KC_W,   KC_V,  TG(_GAME),                        KC_MPLY, KC_J,  KC_F,    KC_O,    KC_U,    KC_COMM, KC_DEL,
        KC_LCTL, KC_N,    KC_R,   KC_T,   KC_S,   KC_G,  KC_NO,                            KC_NO,   KC_Y,  KC_H,    KC_A,    KC_E,    KC_I,    KC_QUOT,
        KC_LSFT, KC_X,    KC_Q,   KC_M,   KC_C,   KC_Z,  KC_TAB,    KC_ENT,       KC_LALT, KC_LGUI, KC_K,  KC_P,    KC_QUOT, KC_SCLN, KC_DOT,  KC_ENT,
                 KC_LALT, KC_TAB, KC_ENT, KC_SPC, LOWER, KC_LSFT,   KC_BSPC,      KC_LCTL, KC_SPC,  RAISE, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

    [_GAME] = LAYOUT(
        KC_ESC, KC_1,    KC_2,   KC_3, KC_4, KC_5,                                             KC_6,  KC_7,    KC_8,    KC_9,    KC_0,    _______,
        KC_6,   KC_TAB,  KC_Q,   KC_W, KC_E, KC_R,  TG(_GAME),                        KC_MPLY, KC_T,  KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_7,   KC_LCTL, KC_A,   KC_S, KC_D, KC_F,  KC_NO,                            KC_NO,   KC_G,  KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_8,   KC_LSFT, KC_Z,   KC_X, KC_C, KC_V,  KC_TAB,    KC_ENT,       KC_LGUI, KC_LALT, KC_B,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                KC_LALT, KC_TAB, KC_9, KC_0, LOWER, KC_SPC,    KC_BSPC,      KC_LSFT, KC_SPC,  RAISE, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT
    ),

    [_LOWER] = LAYOUT(
        ANIM_TOG, _______, _______, _______, _______,    _______,                                            _______, _______, _______, _______, _______, _______,
        _______,  KC_1,    KC_2,    KC_3,    KC_4,       KC_5,    TG(_GAME),                        KC_MPLY, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
        KC_VOLU,  OS_SUPR, OS_ALT,  OS_CTRL, OS_SHFT,    KC_PERC, _______,                          _______, KC_ESC,  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______,
        KC_VOLD,  KC_MPRV, KC_MNXT, C(KC_C), C(KC_V),    KC_MPLY, _______,   _______,      _______, _______, KC_DEL,  KC_BSPC, KC_ENT,  KC_TAB,  KC_SCLN, _______,
                  _______, _______, _______, G(S(KC_S)), _______, _______,   _______,      _______, _______, _______, _______, _______, _______, _______
    ),

    [_RAISE] = LAYOUT(
        _______, _______, _______, _______, _______, _______,                                            _______, _______, DF(_QWERTY), DF(_NIGHT), DF(_GALLIUM), _______,
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_TILD, TG(_GAME),                        KC_MPLY, KC_CIRC, KC_AMPR, KC_QUES,     KC_SLSH,    KC_ASTR,      _______,
        _______, KC_LPRN, KC_RPRN, KC_LCBR, KC_RCBR, KC_PERC, _______,                          _______, KC_PIPE, OS_SHFT, OS_CTRL,     OS_ALT,     OS_SUPR,      _______,
        _______, KC_LBRC, KC_RBRC, KC_UNDS, KC_MINS, KC_GRV,  _______,   _______,      _______, _______, KC_BSLS, KC_EQL,  KC_RABK,     KC_LABK,    KC_PLUS,      _______,
                 _______, _______, _______, _______, _______, KC_SPC,    _______,      _______, _______, _______, _______, _______,     _______,    _______
    ),

    [_ADJUST] = LAYOUT(
        QK_BOOT, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                                              KC_F6,      KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        _______, KC_BTN2, KC_WH_U, KC_MS_U, KC_WH_D, KC_TAB,  TG(_GAME),                        KC_MPLY, C(KC_N),    C(KC_L), C(KC_T), C(KC_W), G(KC_S), KC_F12,
        _______, KC_BTN1, KC_MS_L, KC_MS_D, KC_MS_R, KC_Q,    _______,                          _______, G(S(KC_S)), KC_HOME, KC_END,  _______, _______, _______,
        _______, KC_BTN4, KC_BTN5, C(KC_C), C(KC_V), KC_ENT,  _______,   _______,      _______, _______, KC_DEL,     KC_BSPC, KC_LGUI, KC_LALT, _______, _______,
                 _______, _______, _______, _______, _______, _______,   _______,      _______, _______, _______,    _______, _______, _______, _______
    )
};
#if defined(ENCODER_MAP_ENABLE)
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

// Seeded only into a blank EEPROM; after that Vial owns them.
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

    { .input = { KC_Q, KC_M, KC_QUOT, KC_SCLN }, .output = DEL_LINE },
    { .input = { KC_C, KC_M, 0, 0 },             .output = KC_ESCAPE },
    { .input = { KC_QUOT, KC_SCLN, 0, 0 },       .output = KC_BSPC },
    { .input = { KC_QUOT, KC_P, 0, 0 },          .output = KC_ENTER },
    { .input = { KC_Q, KC_M, 0, 0 },             .output = C(KC_BSPC) },
};

// The three sets above are seeded in blocks of this size, in layout order.
#define COMBOS_PER_LAYOUT 5

// Combos match keycodes, not layers, so each block is scoped to its layout.
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    // A stray letter chord mid-game sending Escape is the worst case.
    if (layer_state_is(_GAME)) {
        return false;
    }

    // Combos added in Vial land past the seeded blocks, so leave them global.
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

// The matcher reads these; EEPROM loads them before the seeding runs.
extern combo_t  key_combos[VIAL_COMBO_ENTRIES];
extern uint16_t key_combos_keys[VIAL_COMBO_ENTRIES][5];

// Seeds combos into EEPROM and RAM; nothing reloads RAM again this boot.
void eeconfig_init_user(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(default_combos); i++) {
        dynamic_keymap_set_combo(i, &default_combos[i]);

        memcpy(key_combos_keys[i], default_combos[i].input, sizeof(default_combos[i].input));
        key_combos[i].keys    = key_combos_keys[i];
        key_combos[i].keycode = default_combos[i].output;
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

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

// Only the master sees keypresses, so it counts taps for the slave.
static uint8_t tap_count = 0;

typedef struct {
    uint8_t taps;
    uint8_t anim;
    uint8_t game;
} right_screen_sync_t;

// Inverts OLED colors
static bool game_active = false;

static void right_screen_tap(void) {
#ifdef OLED_ENABLE
    if (anim_mode == ANIM_BONGO) {
        bongo_tap();
    } else {
        duck_tap();
    }
#endif
}

static uint32_t last_activity = 0;

static void note_activity(void) {
    last_activity = timer_read32();
}

static void tap_sync_slave_handler(uint8_t in_len, const void *in_data, uint8_t out_len, void *out_data) {
    const right_screen_sync_t *in = (const right_screen_sync_t *)in_data;

#ifdef OLED_ENABLE
    // The two animations need different rotations, so re-init on a mode change.
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

oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state  = os_up_unqueued;
oneshot_state os_supr_state = os_up_unqueued;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        tap_count++;
        note_activity();
#ifdef ANIM_ON_MASTER
        // Only runs on the master, so feed its own animation directly.
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
            // The waits matter: apps that filter synthetic input drop fast taps.
            tap_code(KC_HOME);
            wait_ms(DEL_LINE_STEP_MS);
            register_code(KC_LSFT);
            wait_ms(DEL_LINE_STEP_MS);
            tap_code(KC_END);
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
// Bongo is landscape art and the duck portrait, so rotation follows the mode.
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (half_draws_anim() && anim_mode == ANIM_BONGO) {
        return OLED_ROTATION_180;
    }
    return OLED_ROTATION_270;
}

// Panel geometry in the rotated (portrait) space both screens draw in.
#define SCREEN_W 32
#define SCREEN_H 128

// The driver has no shape API, so the framing is drawn a pixel at a time.
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

static void round_corners(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
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
    round_corners(x0, y0, x1, y1);
}

static void draw_dotted(uint8_t y) {
    for (uint8_t x = 0; x < SCREEN_W; x += 2) {
        oled_write_pixel(x, y, true);
    }
}

// Left screen: the active layer and the four sticky mods.
#define UI_GLYPH_W   5
#define UI_GLYPH_ADV 6

// Printable ASCII from QMK's glcdfont.c (GPL-2.0), placed for exact centring.
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
        round_corners(0, top, SCREEN_W - 1, bot);
    } else {
        draw_round_rect(0, top, SCREEN_W - 1, bot);
    }
    // Ink flips on a filled pill, so the label reads as knocked out of it.
    draw_text_centered(top + UI_TEXT_INSET, label, !active);
}

static void render_layer_status(void) {
    // layer_state is 0 with no overlay held, so fall back to the default layer.
    uint8_t active_layer = layer_state ? get_highest_layer(layer_state) : get_highest_layer(default_layer_state);
    // Master-only state, so no split sync needed.
    uint8_t mod_bits = (os_shft_state != os_up_unqueued ? 1 : 0)
                     | (os_ctrl_state != os_up_unqueued ? 2 : 0)
                     | (os_alt_state  != os_up_unqueued ? 4 : 0)
                     | (os_supr_state != os_up_unqueued ? 8 : 0);

    // Redraw only on change; a frame is hundreds of writes plus a clear.
    static uint8_t last_layer = 0xFF;
    static uint8_t last_mods  = 0xFF;
    if (active_layer == last_layer && mod_bits == last_mods) {
        return;
    }
    last_layer = active_layer;
    last_mods  = mod_bits;

    // Required, not tidy: a pill going filled to outlined would keep its fill.
    oled_clear();

    draw_text_centered(UI_CAPTION_Y, "LAYER", true);

    draw_round_rect(0, UI_CARD_TOP, SCREEN_W - 1, UI_CARD_BOT);
    draw_text_centered(UI_NAME_Y, layer_name(active_layer), true);

    draw_dotted(UI_RULE_Y);

    render_mod_pill(0, "Sft", mod_bits & 1);
    render_mod_pill(1, "Ctl", mod_bits & 2);
    render_mod_pill(2, "Alt", mod_bits & 4);
    render_mod_pill(3, "Sup", mod_bits & 8);
}

// Sailor duck, sampled 1:1 from the reference art on its native 25px grid.
#define DUCK_SCALE 1
#define DUCK_COLS 28
#define DUCK_ROWS 32
#define DUCK_X_OFFSET 2
#define DUCK_Y_OFFSET 50
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

#define WATER_Y 76
#define WATER_FADE 12  // rows the surface texture fades out over
#define WATER_TOP  4   // dither threshold at the surface; 8 would be bare

#define BOB_FRAME_MS 110   // how long each step of the cycle holds
#define ANIM_HOLD_MS SCREEN_ON_MS  // bob for as long as the screen is lit

static const uint8_t bob_cycle[] = {0, 1, 2, 2, 1, 0};
#define BOB_FRAMES (sizeof(bob_cycle) / sizeof(bob_cycle[0]))

static uint32_t duck_last_tap   = 0;
static uint32_t duck_frame_time = 0;
static uint8_t  duck_frame      = 0;
static bool     duck_animating  = false;
static uint8_t  duck_ripple     = 0;  // wave phase, flipped each bob frame

// Clouds hold position when you stop; unlike gulls they read fine static.
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

#define GULL_W 5
#define GULL_H 2
#define GULL_GAP_FRAMES 14
static const uint8_t gull_frames[2][GULL_H][GULL_W] = {
    {{1, 1, 0, 1, 1}, {0, 0, 1, 0, 0}},  // wings up
    {{0, 0, 1, 0, 0}, {1, 1, 0, 1, 1}},  // wings down
};
#define GULL_Y_MIN 4
#define GULL_Y_MAX (WATER_Y - 20)

static int16_t gull_x     = -99;  // -99 = no gull in flight
static uint8_t gull_y     = 6;
static uint8_t gull_flap  = 0;
static uint8_t gull_gap   = 0;
static uint8_t gull_speed = 1;

// One fish per lane, each with its own gap, so the tank fills unevenly.
#define FISH_W 6
#define FISH_H 3
#define FISH_LANES 3
#define FISH_STEP_MS (BOB_FRAME_MS * 3)
#define FISH_GAP_FRAMES 40
#define BUBBLE_COUNT 5
#define BUBBLE_NONE 255
#define BUBBLE_STEP_MS (BOB_FRAME_MS * 2)
#define BUBBLE_GAP_FRAMES 12

static uint8_t bubble_x[BUBBLE_COUNT] = {0};
static uint8_t bubble_y[BUBBLE_COUNT] = {BUBBLE_NONE, BUBBLE_NONE, BUBBLE_NONE, BUBBLE_NONE, BUBBLE_NONE};
static uint8_t bubble_gap = 0;

static uint8_t water_shift = 0;

// Indexed by direction so a fish faces the way it swims.
static const uint8_t fish_shape[2][FISH_H][FISH_W] = {
    {{1, 0, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1},
     {1, 0, 1, 1, 1, 0}},
    {{0, 1, 1, 1, 0, 1},
     {1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 0, 1}},
};
static const uint8_t fish_rows[FISH_LANES] = {WATER_Y + 30, WATER_Y + 38, WATER_Y + 46};

// Anchored, bobbing slowly - the only thing down there that does not travel.
#define JELLY_W 7
#define JELLY_H 6
#define JELLY_X 23  // off centre, and drawn over the right-hand weed
#define JELLY_TOP (WATER_Y + 17)
#define JELLY_BOB_FRAMES 9

static const uint8_t jelly_shape[JELLY_H][JELLY_W] = {
    {0,0,1,1,1,0,0},
    {0,1,1,1,1,1,0},
    {1,1,1,1,1,1,1},
    {0,1,1,1,1,1,0},
    {0,1,0,1,0,1,0},
    {1,0,0,1,0,0,1},
};
static const uint8_t jelly_bob[] = {0, 1, 2, 2, 1, 0};
static uint8_t jelly_frame = 0;
static uint8_t jelly_tick  = 0;

// Rooted off the bottom edge so the scene needs no seabed line.
#define WEED_COUNT 3
#define WEED_SWAY_FRAMES 7

static const uint8_t weed_x[WEED_COUNT]   = {1, 5, 29};
static const uint8_t weed_top[WEED_COUNT] = {WATER_Y + 28, WATER_Y + 40, WATER_Y + 34};
static const int8_t  weed_wave[8]         = {0, 0, 1, 1, 0, -1, -1, 0};
static uint8_t weed_shift = 0;
static uint8_t weed_tick  = 0;

// Leftmost and rightmost lit column of each duck row, so the sky can be
// cleared behind it and clouds pass behind rather than through the outline.
static const uint8_t duck_lo[DUCK_ROWS] = {6,5,5,5,6,3,3,4,5,4,0,0,0,0,1,2,4,4,3,2,2,2,1,1,1,1,2,2,3,4,5,7};
static const uint8_t duck_hi[DUCK_ROWS] = {8,11,12,13,14,15,17,19,19,18,17,17,17,24,25,26,26,27,27,27,27,27,26,26,26,25,25,24,23,22,20,17};

static int16_t fish_x[FISH_LANES]   = {-99, -99, -99};  // -99 = lane empty
static uint8_t fish_left[FISH_LANES] = {0, 0, 0};
static uint8_t fish_gap[FISH_LANES]  = {0, 0, 0};

// Tiny xorshift, seeded off the timer so the sky differs between boots.
static uint16_t rng_state = 0;
static uint8_t rnd(uint8_t range) {
    rng_state ^= (uint16_t)(rng_state << 7);
    rng_state ^= (uint16_t)(rng_state >> 9);
    rng_state ^= (uint16_t)(rng_state << 8);
    return range ? (uint8_t)(rng_state % range) : 0;
}

static void scatter_clouds(void) {
    for (uint8_t i = 0; i < CLOUD_COUNT; i++) {
        cloud_rows[i]  = (uint8_t)(4 + i * ((WATER_Y - 12) / CLOUD_COUNT) + rnd(6));
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

static void draw_sprite(const uint8_t *sprite, uint8_t w, uint8_t h, int16_t x, int16_t y, bool on) {
    for (uint8_t r = 0; r < h; r++) {
        for (uint8_t c = 0; c < w; c++) {
            if (!sprite[r * w + c]) {
                continue;
            }
            int16_t px = x + c;
            int16_t py = y + r;
            if (px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H) {
                oled_write_pixel((uint8_t)px, (uint8_t)py, on);
            }
        }
    }
}

static void render_sky(void) {
    for (uint8_t i = 0; i < CLOUD_COUNT; i++) {
        int16_t x = (int16_t)((cloud_shift + cloud_phase[i]) % (SCREEN_W + CLOUD_W)) - CLOUD_W;
        draw_sprite(&cloud_shape[0][0], CLOUD_W, 2, x, cloud_rows[i], true);
    }

    if (gull_x != -99) {
        draw_sprite(&gull_frames[gull_flap][0][0], GULL_W, GULL_H, gull_x, gull_y, true);
    }
}

// Dither that thins with depth, with the wave crests drawn lit on top.
static void render_water(uint8_t ripple) {
    for (uint8_t y = WATER_Y; y < WATER_Y + WATER_FADE; y++) {
        uint8_t thr = WATER_TOP + (8 - WATER_TOP) * (y - WATER_Y) / (WATER_FADE - 1);
        for (uint8_t x = 0; x < SCREEN_W; x++) {
            oled_write_pixel(x, y, ((x * 7 + y * 3 + water_shift) % 8) >= thr);
        }
    }

    for (uint8_t x = 0; x < SCREEN_W; x++) {
        uint8_t t    = (x + ripple * 2) % 8;
        uint8_t rise = (t < 4) ? t : (8 - t);  // 0..4..0
        oled_write_pixel(x, WATER_Y + (rise / 2), true);
        if (x % 4 != 3) {  // dashed second line, reads as depth
            oled_write_pixel(x, WATER_Y + 5 + ((rise + 2) / 3), true);
        }
    }

    for (uint8_t i = 0; i < BUBBLE_COUNT; i++) {
        if (bubble_y[i] != BUBBLE_NONE) {
            uint8_t bx = bubble_x[i] + ((bubble_y[i] / 5) & 1);
            if (bx < SCREEN_W) {
                oled_write_pixel(bx, bubble_y[i], true);
            }
        }
    }

    for (uint8_t i = 0; i < WEED_COUNT; i++) {
        for (uint8_t y = weed_top[i]; y < SCREEN_H; y++) {
            uint8_t phase = (uint8_t)((SCREEN_H - 1 - y) + weed_shift) % 8;
            oled_write_pixel((uint8_t)(weed_x[i] + weed_wave[phase]), y, true);
        }
    }

    draw_sprite(&jelly_shape[0][0], JELLY_W, JELLY_H, JELLY_X,
                JELLY_TOP + jelly_bob[jelly_frame], true);

    for (uint8_t i = 0; i < FISH_LANES; i++) {
        if (fish_x[i] != -99) {
            draw_sprite(&fish_shape[fish_left[i]][0][0], FISH_W, FISH_H, fish_x[i], fish_rows[i], true);
        }
    }
}

static void render_duck(void) {
    bool changed = false;

    if (duck_animating && timer_elapsed32(duck_frame_time) >= BOB_FRAME_MS) {
        duck_frame_time = timer_read32();
        duck_frame      = (duck_frame + 1) % BOB_FRAMES;
        duck_ripple ^= 1;
        changed = true;

        if (++cloud_tick >= CLOUD_DRIFT_FRAMES) {
            cloud_tick = 0;
            cloud_shift++;
        }

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

        water_shift++;
        if (++jelly_tick >= JELLY_BOB_FRAMES) {
            jelly_tick  = 0;
            jelly_frame = (jelly_frame + 1) % (sizeof(jelly_bob) / sizeof(jelly_bob[0]));
        }
        if (++weed_tick >= WEED_SWAY_FRAMES) {
            weed_tick = 0;
            weed_shift++;
        }

        if (bubble_gap > 0) {
            bubble_gap--;
        } else {
            for (uint8_t i = 0; i < BUBBLE_COUNT; i++) {
                if (bubble_y[i] == BUBBLE_NONE) {
                    bubble_x[i] = rnd(SCREEN_W);
                    bubble_y[i] = SCREEN_H - 1;
                    bubble_gap  = (uint8_t)(BUBBLE_GAP_FRAMES + rnd(16));
                    break;
                }
            }
        }

        // Each lane spawns on its own, so they do not arrive in formation.
        for (uint8_t i = 0; i < FISH_LANES; i++) {
            if (fish_x[i] != -99) {
                continue;
            }
            if (fish_gap[i] > 0) {
                fish_gap[i]--;
            } else {
                fish_left[i] = rnd(2);
                fish_x[i]    = fish_left[i] ? SCREEN_W : -FISH_W;
                fish_gap[i]  = (uint8_t)(FISH_GAP_FRAMES + rnd(40));
            }
        }

        // Stops only at the end of a cycle, so it never freezes mid-bob.
        if (duck_frame == 0 && timer_elapsed32(duck_last_tap) > ANIM_HOLD_MS) {
            duck_animating = false;
        }
    }

    // Its own clock, so a crossing finishes after you stop typing.
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

    static uint32_t bubble_time = 0;
    if (timer_elapsed32(bubble_time) >= BUBBLE_STEP_MS) {
        bubble_time = timer_read32();
        for (uint8_t i = 0; i < BUBBLE_COUNT; i++) {
            if (bubble_y[i] == BUBBLE_NONE) {
                continue;
            }
            changed = true;
            if (bubble_y[i] <= WATER_Y + WATER_FADE) {
                bubble_y[i] = BUBBLE_NONE;
            } else {
                bubble_y[i]--;
            }
        }
    }

    static uint32_t fish_time = 0;
    if (timer_elapsed32(fish_time) >= FISH_STEP_MS) {
        fish_time = timer_read32();
        for (uint8_t i = 0; i < FISH_LANES; i++) {
            if (fish_x[i] == -99) {
                continue;
            }
            fish_x[i] += fish_left[i] ? -1 : 1;
            changed = true;
            if (fish_x[i] >= SCREEN_W || fish_x[i] < -FISH_W) {
                fish_x[i] = -99;  // gap for the next one was rolled at spawn
            }
        }
    }

    uint8_t bob = duck_animating ? bob_cycle[duck_frame] : 0;

    // Redraw only when something moved; a frame is a thousand-odd writes.
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
            break;
        }
        draw_hline(DUCK_X_OFFSET + duck_lo[row], DUCK_X_OFFSET + duck_hi[row], y, false);
    }

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

    render_water(duck_ripple);
}

// Not in the driver header, but a plain global: the panel cannot be switched
// off until the buffer has drained or the driver just turns it back on.
extern OLED_BLOCK_TYPE oled_dirty;

// Done here rather than the panel's fade command, so both halves match.
static bool screen_power_task(void) {
    static bool     lit        = true;
    static bool     fading_out = false;
    static uint32_t wake_at    = 0;

    uint32_t idle = timer_elapsed32(last_activity);

    if (idle >= SCREEN_ON_MS + SCREEN_FADE_MS) {
        if (oled_dirty == 0 && is_oled_on()) {
            oled_off();
        }
        lit = false;
        return true;
    }

    if (idle >= SCREEN_ON_MS) {
        uint32_t step = idle - SCREEN_ON_MS;
        fading_out = true;
        oled_set_brightness((uint8_t)(OLED_BRIGHTNESS - (uint32_t)OLED_BRIGHTNESS * step / SCREEN_FADE_MS));
        return false;
    }

    if (!lit) {
        oled_set_brightness(0);
        oled_on();
        lit     = true;
        wake_at = timer_read32();
    } else if (fading_out) {
        // Woken mid-fade: ramp on from the level it reached rather than snapping.
        wake_at = timer_read32() - ((uint32_t)oled_get_brightness() * SCREEN_FADE_MS / OLED_BRIGHTNESS);
    }
    fading_out = false;

    uint32_t since = timer_elapsed32(wake_at);
    oled_set_brightness(since >= SCREEN_FADE_MS
                        ? OLED_BRIGHTNESS
                        : (uint8_t)((uint32_t)OLED_BRIGHTNESS * since / SCREEN_FADE_MS));
    return false;
}

bool oled_task_user(void) {
    bool asleep = screen_power_task();
    oled_invert(game_active);  // no-op unless the state actually changed

    if (asleep) {
        return false;  // let the buffer drain; the movers resume on wake
    }


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
