// Copyright 2024 iamdanielv (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    _WIN_LYR,       // 0
    _WIN_ALT_LYR,   // 1
    _CTL_LYR,       // 2
    _NUM_LYR,       // 3
    _NAV_LYR,       // 4
    _FN_LYR,        // 5
};

// ***********************
// * Keyboard Management *
// ***********************
void housekeeping_task_user(void) {
    // Note: We can decide what to do with the MAC Led in this function
    if (IS_LAYER_ON(_CTL_LYR)) { // if the Ctl layer is active
      gpio_write_pin_low(LED_MAC_PIN); // low means turn on
    } else {
      gpio_write_pin_high(LED_MAC_PIN); // high means turn off
    }

    if(!keymap_config.no_gui) {
        // we have NOT enabled the no_gui,
        // we can re-use the Win Lock LED as NumLock indicator
        if(IS_LAYER_ON(_NUM_LYR)) { // if the Num layer is active
            // get the current LED state
            led_t led_state = host_keyboard_led_state();
            if(led_state.num_lock) {
                // Num/Win lock should be on
                gpio_write_pin_low(LED_WIN_LOCK_PIN);
            } else {
                // Num/Win lock should be off
                gpio_write_pin_high(LED_WIN_LOCK_PIN);
            }
        } // else we are not on the num layer, ignore
    } // else we have enabled no_gui, skip re-using the LED
}


// from https://thomasbaart.nl/2018/12/13/qmk-basics-tap-dance/
void safe_reset(tap_dance_state_t *state, void *user_data) {
  if (state->count >= 3) {
    // Reset the keyboard if you tap the key more than three times
    reset_keyboard();
    reset_tap_dance(state);
  }
}

void safe_clear(tap_dance_state_t *state, void *user_data) {
  if (state->count >= 3) {
    // clear eprom if you tap the key more than three times
    eeconfig_init();
    soft_reset_keyboard();
    reset_tap_dance(state);
  }
}


// ************
// * Tap Hold *
// ************
enum tap_hold_keys {
    TD_RESET, // require 3 taps to reset board
    TD_CLEAR, // require 3 taps to clear eeprom
    TD_CTL_TG // require double tap to enable ctl layer
};

typedef struct {
    uint16_t on_tap;  // key to send on tap
    uint16_t on_hold; // key to send on hold
    uint16_t chosen;  // key to "release" when finished
                      // the value for it is set in td_tap_hold_finished
} td_tap_hold_t;

void td_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    td_tap_hold_t *tap_hold = (td_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            // "press" the key specified on_hold
            register_code16(tap_hold->on_hold);
            // keep track of which key was used
            tap_hold->chosen = tap_hold->on_hold;
        } else {
            // "press" the key specified on_tap
            register_code16(tap_hold->on_tap);
            // keep track of which key was used
            tap_hold->chosen = tap_hold->on_tap;
        }
    }
}

void td_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    td_tap_hold_t *tap_hold = (td_tap_hold_t *)user_data;

    if (tap_hold->chosen) {
        // there is something to cleanup
        // release the key that was "pressed"
        unregister_code16(tap_hold->chosen);

        // reset the key to 0 to signify nothing is selected
        tap_hold->chosen = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(on_tap, on_hold) \
    { .fn = {NULL, td_tap_hold_finished, td_tap_hold_reset}, .user_data = (void *)&((td_tap_hold_t){on_tap, on_hold, 0}), }

// clang-format off
tap_dance_action_t tap_dance_actions[] = {
    [TD_RESET]  = ACTION_TAP_DANCE_FN(safe_reset),
    [TD_CLEAR]  = ACTION_TAP_DANCE_FN(safe_clear),
    [TD_CTL_TG] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_RCTL, _CTL_LYR)
};
// clang-format on


// ******************************
// * Aliases to simplify keymap *
// ******************************
#define FN_W_CAPS LT(_WIN_ALT_LYR,KC_CAPS)
#define FN_W_RALT LT(_WIN_ALT_LYR,KC_RALT)

#define MY_UNDO   C(KC_Z)
#define MY_CUT    C(KC_X)
#define MY_COPY   C(KC_INS)
#define MY_PASTE  S(KC_INS)

#define TG_NAV    TG(_NAV_LYR)
#define TG_NUM    TG(_NUM_LYR)
#define TG_CTL    TG(_CTL_LYR)
#define MO_CTL    MO(_CTL_LYR)
#define MO_FN     MO(_FN_LYR)
#define TD_TG_CTL TD(TD_CTL_TG)
#define TD_KB_RST TD(TD_RESET)
#define TD_KB_CLR TD(TD_CLEAR)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_WIN_LYR] = LAYOUT( // 0
        KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,     KC_6,     KC_7,     KC_8,      KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,     KC_Y,     KC_U,     KC_I,      KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_HOME,
        FN_W_CAPS, KC_A,      KC_S,      KC_D,      KC_F,      KC_G,     KC_H,     KC_J,     KC_K,      KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,   KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        KC_LCTL,   KC_LGUI,   KC_LALT,                         KC_SPC,                       FN_W_RALT, MO_FN,                KC_LEFT,            KC_DOWN,   KC_RGHT
    ),
    [_WIN_ALT_LYR] = LAYOUT( // 1
        KC_GRV,    KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,    KC_F6,    KC_F7,    KC_F8,     KC_F9,    KC_F10,     KC_F11,   KC_F12,   KC_DEL,    _______,
        KC_GRV,    KC_HOME,   KC_UP,     KC_END,    C(KC_R),   KC_PGUP,  _______,  _______,  _______,   _______,  KC_PSCR,    KC_SCRL,  KC_PAUS,  KC_INS,    KC_END,
        _______,   KC_LEFT,   KC_DOWN,   KC_RIGHT,  C(KC_F),   KC_PGDN,  _______,  _______,  _______,   _______,  KC_HOME,    KC_END,             _______,   KC_SCRL,
        _______,   MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   _______,  _______,  KC_PGDN,   KC_PGUP,  _______,    MO_CTL,             _______,   KC_RCTL,
        _______,   _______,   _______,                         _______,                      KC_RCTL,   _______,              _______,            _______,   _______
    ),
    [_CTL_LYR] = LAYOUT( // 2
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   TD_KB_RST, _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  RGB_M_P,    RGB_RMOD, RGB_MOD,  RGB_TOG,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  RGB_SPD,    RGB_SPI,            _______,   _______,
        _______,   TD_KB_CLR, _______,   _______,   _______,   _______,  NK_TOGG,  RGB_HUI,  RGB_VAD,   RGB_VAI,  _______,    _______,            RGB_VAI,   TG_CTL,
        _______,   _______,   _______,                         _______,                      _______,   _______,              RGB_SPD,            RGB_VAD,   RGB_SPI
    ),
    [_NUM_LYR] = LAYOUT( // 3
        _______,   _______,   _______,   _______,   _______,   _______,  KC_NUM,   KC_P7,    KC_P8,     KC_P9,   KC_PAST,    _______,  _______,  _______,   _______,
        _______,   KC_BTN1,   KC_MS_U,   KC_BTN2,   _______,   _______,  XXXXXXX,  KC_P4,    KC_P5,     KC_P6,   KC_PPLS,    _______,  _______,  _______,   _______,
        _______,   KC_MS_L,   KC_MS_D,   KC_MS_R,   _______,   _______,  XXXXXXX,  KC_P1,    KC_P2,     KC_P3,   KC_PENT,    _______,            _______,   TG_NUM,
        _______,   _______,   _______,   _______,   _______,   _______,  XXXXXXX,  KC_P0,    KC_PDOT,   KC_PDOT, KC_PSLS,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      _______,   _______,             _______,            _______,   _______
    ),
    [_NAV_LYR] = LAYOUT( // 4
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   _______,
        _______,   KC_HOME,   KC_UP,     KC_END,    C(KC_R),   KC_PGUP,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   TG_NAV,
        _______,   KC_LEFT,   KC_DOWN,   KC_RIGHT,  C(KC_F),   KC_PGDN,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,   MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      _______,  _______,              _______,            _______,   _______
    ),
    [_FN_LYR] = LAYOUT( // 5
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,    KC_VOLD,  KC_VOLU,  KC_DEL,    _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   TG_NAV,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   TG_NUM,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    MO_CTL,             _______,   TD_TG_CTL,
        _______,   _______,   _______,                         _______,                      KC_RCTL,  _______,              _______,            _______,   _______
    )
};


#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_WIN_LYR]      = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_WIN_ALT_LYR]  = {ENCODER_CCW_CW(_______, _______)},
    [_CTL_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_NUM_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_NAV_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_FN_LYR]       = {ENCODER_CCW_CW(_______, _______)}
};
#endif
// clang-format on

// clang-format off
/* LED Matrix
        KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,     KC_6,     KC_7,     KC_8,      KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        56         55         54         53         52         51        50        49        48         47        46          45        44        43

        KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,     KC_Y,     KC_U,     KC_I,      KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_HOME,
        29         30         31         32         33         34        35        36        37         38        39          40        41        42         57

        CAPS,      KC_A,      KC_S,      KC_D,      KC_F,      KC_G,     KC_H,     KC_J,     KC_K,      KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        28         27         26         25         24         23        22        21        20         19        18          17                  16         58

        KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,   KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        3          4          5          6          7          8         9         10        11         12        13          14                  15         59

        KC_LCTL,   KC_LGUI,   KC_LALT,                         KC_SPC,                       KC_RALT,   FN,                   KC_LEFT,            KC_DOWN,   KCRGHT
        2          1          0                                65                            64         63                    62                  61         60
*/
// clang-format on

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {

    if (IS_LAYER_ON(_WIN_ALT_LYR)) {
        const uint8_t led_indexes[39] = {
            28, // use caps as indicator
            64, // use right alt as indicator

            56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, // the whole first row is used for Fn keys = 14 keys

            // 2nd row
            29, 30, 31, 32, 33, 34, 39, 40, 41, // = 9 keys

            // 3rd row
            27, 26, 25, 24, 23, 18, 17, // = 7 keys

            //4th row
            4, 5, 6, 7, 8, 11, 12 // = 7 keys
        };

        for (int i = 0; i < 39; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 255, 255, 0);
        }

        // highlight right shift as moving to ctl layer
        RGB_MATRIX_INDICATOR_SET_COLOR(14, 0, 255, 255);
    }

    if (IS_LAYER_ON(_CTL_LYR)) {
        const uint8_t led_indexes[15] = {
            59, // use PgDn as indicator

            // RGB buttons
            39, 40, 41, 42, // P [ } \ =  4 keys
            18, 17, // ; ' = 2 keys
            9, 10, 11, 12, // N M , . = 4 keys
            62, 61, 60, 15// arrow keys = 4 keys
        };

        for (int i = 0; i < 15; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0, 255, 255);
        }

        // highlight q and z as reset and clear
        RGB_MATRIX_INDICATOR_SET_COLOR(30, 0, 255, 255);
        RGB_MATRIX_INDICATOR_SET_COLOR(4, 0, 255, 255);
    }

    if (IS_LAYER_ON(_NUM_LYR)) {
        const uint8_t led_indexes[9] = {
            58, // use PgUp as indicator

            // Light up around the numpad to make it easier to see
            // 6 is used as numlock and starts the numpad
            50, 35, 22, 9, // 6 y h n = 4 keys
            // right side
            46, 39, 18, 13 // 0 p ; / = 4 keys
        };

        for (int i = 0; i < 9; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0, 255, 0);
        }
    }

    if (IS_LAYER_ON(_NAV_LYR)) {
        const uint8_t led_indexes[16] = {
            57, // use home key as indicator

            // 2nd row
            30, 31, 32, 33, 34, // = 5 keys

            // 3rd row
            27, 26, 25, 24, 23, // = 5 keys

            //4th row
            4, 5, 6, 7, 8  // = 5 keys
        };

        for (int i = 0; i < 16; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0, 0, 255);
        }
    }

    if (IS_LAYER_ON(_FN_LYR)) {
        // highlight the toggle buttons
        RGB_MATRIX_INDICATOR_SET_COLOR(57, 0, 0, 255);   // NAV = Home
        RGB_MATRIX_INDICATOR_SET_COLOR(58, 0, 255, 0);   // NUM = PgUp
        RGB_MATRIX_INDICATOR_SET_COLOR(59, 0, 255, 255); // CTL = PgDn

        // highlight right shift as moving to ctl layer
        RGB_MATRIX_INDICATOR_SET_COLOR(14, 0, 255, 255);

        // highlight the aux buttons on right of keyboard
        const uint8_t led_indexes[8] = {
            63, //highlight the fn button
            49, 48, 47, 46, 45, 44, 43 // used for media keys = 7 keys
        };

        for (int i = 0; i < 8; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 128, 128, 128);
        }
    }

    return true;
}
