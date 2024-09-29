// Copyright 2024 SDK (@sdk66)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    _WIN_LYR,    // 0
    _WIN_FN_LYR, // 1
    _MAC_LYR,    // 2
    _MAC_FN_LYR, // 3
    _EXTRA_LYR,  // 4
    _EXTRA2_LYR, // 5
};

void housekeeping_task_user(void) {
    // Note: We can decide what to do with the MAC Led in this function
    if (IS_LAYER_ON(_MAC_LYR) || IS_LAYER_ON(_MAC_FN_LYR)) {
        // if either Mac or Mac FN are on
        gpio_write_pin_low(LED_MAC_PIN); // low means turn on
    } else {
        gpio_write_pin_high(LED_MAC_PIN); // high means turn off
    }
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_WIN_LYR] = LAYOUT( // 0
        KC_ESC,   KC_1,       KC_2,       KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        KC_TAB,   KC_Q,       KC_W,       KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_DEL,
        KC_CAPS,  KC_A,       KC_S,       KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        KC_LSFT,  KC_Z,       KC_X,       KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        KC_LCTL,  KC_LGUI,    KC_LALT,                        KC_SPC,                       KC_RALT,  MO(1),                KC_LEFT,            KC_DOWN,   KC_RGHT
    ),

    [_WIN_FN_LYR] = LAYOUT( // 1
        KC_GRV,   KC_F1,      KC_F2,      KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,     KC_F11,   KC_F12,   _______,   _______,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    KC_HOME,  KC_SCRL,  RGB_MOD,   KC_INS,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    KC_PSCR,            _______,   KC_PAUSE,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  MO(4),      MO(5),              RGB_VAI,   _______,
        _______,  GU_TOGG,    TG(2),                          _______,                      KC_RCTL,  _______,              RGB_SPD,            RGB_VAD,   RGB_SPI
    ),

    [_MAC_LYR] = LAYOUT( // 2, gets toggled from Fn then left alt in both Win and Mac Layers
        KC_ESC,   KC_1,       KC_2,       KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        KC_TAB,   KC_Q,       KC_W,       KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_DEL,
        KC_CAPS,  KC_A,       KC_S,       KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        KC_LSFT,  KC_Z,       KC_X,       KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        KC_LCTL,  KC_LALT,    KC_LCMD,                        KC_SPC,                       KC_RCMD,  MO(3),                KC_LEFT,            KC_DOWN,   KC_RGHT
    ),

    [_MAC_FN_LYR] = LAYOUT( // 3
        KC_GRV,   KC_F1,      KC_F2,      KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,     KC_F11,   KC_F12,   _______,   _______,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    KC_HOME,  KC_SCRL,  RGB_MOD,   KC_INS,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    KC_PSCR,            _______,   KC_PAUSE,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  MO(4),      MO(5),              RGB_VAI,   _______,
        _______,  _______,    TG(2),                          _______,                      KC_RCTL,  _______,              RGB_SPD,            RGB_VAD,   RGB_SPI
    ),

    [_EXTRA_LYR] = LAYOUT( // 4, extra layer open to customization, accessible by pushing Fn, then /
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   _______,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   _______,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,  _______,    _______,                        _______,                      _______,  _______,              _______,            _______,   _______
    ),

    [_EXTRA2_LYR] = LAYOUT( // 5, used for booting the KB and clearing eeprom, accessible by pushing Fn, then right shift
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   _______,
        _______,  QK_BOOT,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   _______,
        _______,  _______,    _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,  EE_CLR,     _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,  _______,    _______,                        EE_CLR,                       _______,  _______,              _______,            _______,   _______
    )
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_WIN_LYR]      = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_WIN_FN_LYR]   = {ENCODER_CCW_CW(_______, _______)},
    [_MAC_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_MAC_FN_LYR]   = {ENCODER_CCW_CW(_______, _______)},
    [_EXTRA_LYR]    = {ENCODER_CCW_CW(_______, _______)},
    [_EXTRA2_LYR]   = {ENCODER_CCW_CW(_______, _______)}
};
#endif
// clang-format on
