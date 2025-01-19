// Copyright 2024 iamdanielv (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "quantum.h"

#include "features/defines.h"
#include "features/indicator_queue.h"
#include "features/fn_mode.h"
#include "features/tap_hold.h"
#include "features/indicators.h"
#include "features/rgb_keys.h"

// ***********************
// * Keyboard Management *
// ***********************
void housekeeping_task_user(void) {
    // Note: We can decide what to do with the MAC Led in this function
    if (IS_LAYER_ON(_CTL_LYR)) {         // if the Ctl layer is active
        gpio_write_pin_low(LED_MAC_PIN); // low means turn on
    } else {
        gpio_write_pin_high(LED_MAC_PIN); // high means turn off
    }

    if (!keymap_config.no_gui) {
        // we have NOT enabled the no_gui,
        // we can re-use the Win Lock LED as NumLock indicator
        if (IS_LAYER_ON(_NUM_LYR)) { // if the Num layer is active
            // get the current LED state
            led_t led_state = host_keyboard_led_state();
            if (led_state.num_lock) {
                // Num/Win lock should be on
                gpio_write_pin_low(LED_WIN_LOCK_PIN);
            } else {
                // Num/Win lock should be off
                gpio_write_pin_high(LED_WIN_LOCK_PIN);
            }
        } // else we are not on the num layer, ignore
    }     // else we have enabled no_gui, skip re-using the LED
}

// *************
// * Tap Dance *
// *************
enum tap_dance_keys {
    TD_RESET, // require 3 taps to reset board
    TD_CLEAR, // require 3 taps to clear eeprom
    TD_CTL_TG // require double tap to enable ctl layer
};

// *****************************
// * Custom processing of keys *
// *****************************
enum custom_keycodes { KC_SWP_FN = SAFE_RANGE };

// clang-format off
tap_dance_action_t tap_dance_actions[] = {
    [TD_RESET]  = ACTION_TAP_DANCE_FN(safe_reset),
    [TD_CLEAR]  = ACTION_TAP_DANCE_FN(safe_clear),
    [TD_CTL_TG] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_RCTL, _CTL_LYR)
};
// clang-format on

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_WIN_LYR] = LAYOUT( // 0
        KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,     KC_6,     KC_7,     KC_8,      KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,     KC_Y,     KC_U,     KC_I,      KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_HOME,
        W_ALT_CAPS,KC_A,      KC_S,      KC_D,      KC_F,      KC_G,     KC_H,     KC_J,     KC_K,      KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,   KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        KC_LCTL,   KC_LGUI,   KC_LALT,                         KC_SPC,                       FN_RALT,   MO_FN,                KC_LEFT,            KC_DOWN,   KC_RGHT
    ),
    [_WIN_FN_LYR] = LAYOUT( // 1
        KC_GRV,    KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,    KC_F6,    KC_F7,    KC_F8,     KC_F9,    KC_F10,     KC_F11,   KC_F12,   KC_DEL,    _______,
        MY_GRV,    MY_CONS,   MY_TASK,   C(KC_F),   C(KC_R),   _______,  KC_PGUP,  KC_HOME,  KC_UP,     KC_END,   KC_PSCR,    KC_SCRL,  KC_PAUS,  KC_INS,    KC_END,
        _______,   KC_LALT,   KC_LGUI,   KC_LSFT,   KC_LCTL,   _______,  KC_PGDN,  KC_LEFT,  KC_DOWN,   KC_RIGHT, KC_HOME,    KC_END,             _______,   KC_SCRL,
        _______,   MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   _______,  _______,  MY_BACK,   MY_FWD,   _______,    MO_CTL,             _______,   KC_RCTL,
        _______,   _______,   _______,                         _______,                      KC_RCTL,   _______,              _______,            _______,   _______
    ),
    [_CTL_LYR] = LAYOUT( // 2
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   TD_KB_RST, _______,   _______,   _______,   _______,  _______,  _______,  RM_HUED,   RM_HUEU,  RGB_M_P,    RM_PREV,  RM_NEXT,  RM_TOGG,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  RM_SATD,   RM_SATU,  RM_SPDD,    RM_SPDU,            _______,   _______,
        _______,   TD_KB_CLR, _______,   _______,   _______,   _______,  NK_TOGG,  _______,  RM_VALD,   RM_VALU,  _______,    _______,            RM_VALU,   TG_CTL,
        KC_SWP_FN, _______,   _______,                         _______,                      _______,   _______,              RM_SPDD,            RM_VALD,   RM_SPDU
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
        MY_GRV,    MY_CONS,   MY_TASK,   C(KC_F),   C(KC_R),   _______,  KC_PGUP,  KC_HOME,  KC_UP,    KC_END,   KC_PSCR,    KC_SCRL,  KC_PAUS,  KC_INS,    TG_NAV,
        _______,   KC_LALT,   KC_LGUI,   KC_LSFT,   KC_LCTL,   _______,  KC_PGDN,  KC_LEFT,  KC_DOWN,  KC_RIGHT, KC_HOME,    KC_END,             _______,   _______,
        _______,   MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   _______,  _______,  MY_BACK,  MY_FWD,   _______,    _______,            _______,   _______,
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
    [_WIN_FN_LYR]   = {ENCODER_CCW_CW(_______, _______)},
    [_CTL_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_NUM_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_NAV_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_FN_LYR]       = {ENCODER_CCW_CW(_______, _______)}
};
#endif
// clang-format on

bool fn_mode_enabled = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == KC_SWP_FN) {
        if (record->event.pressed) {
            fn_mode_enabled = !fn_mode_enabled;
            blink_numbers(fn_mode_enabled);
            blink_space(true);
        }
        return false;
    }

    if (!process_fn_mode(keycode, record)) { return false; }
    if (!process_rgb_keys(keycode, record)) { return false; }

    switch (keycode) {
        case QK_MAGIC_TOGGLE_NKRO:
            if (record->event.pressed) {
                clear_keyboard(); // clear first buffer to prevent stuck keys
                wait_ms(50);
                keymap_config.nkro = !keymap_config.nkro;
                blink_NKRO(keymap_config.nkro);
                wait_ms(50);
                clear_keyboard(); // clear first buffer to prevent stuck keys
                wait_ms(50);
            }
            return false;
        case MY_GRV:
            if (record->tap.count > 0) { // Key is being tapped
                if (record->event.pressed) {
                    // Handle tap press event...
                    if (record->tap.count == 1) {
                        register_code16(KC_GRV);
                    } else if (record->tap.count == 2) {
                        // this is the 2nd tap,
                        // but we already did something on the 1st tap
                        // delete the previous key
                        tap_code(KC_BSPC);
                        register_code16(KC_TILD);
                    }
                } else {
                    // Handle tap release event...
                    if (record->tap.count == 1) {
                        unregister_code16(KC_GRV);
                    } else if (record->tap.count == 2) {
                        unregister_code16(KC_TILD);
                    }
                }
            } else { // Key is being held
                if (record->event.pressed) {
                    // Handle hold press event...
                    // send backticks to start a code block
                    SEND_STRING("``````");
                    // move cursor to the middle of the code block
                    tap_code(KC_LEFT);
                    tap_code(KC_LEFT);
                    tap_code(KC_LEFT);
                }
            }
            return false; // we handled all cases, stop further processing
        default:
            return true;
    }

    return true;
}

