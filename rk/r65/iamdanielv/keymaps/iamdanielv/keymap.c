// Copyright 2024 iamdanielv (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "features/defines.h"
#include "quantum.h"

#include "features/dv_layer_lock.h"
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
    // if the Ctl layer is active or FN key mode is enabled
    if (IS_LAYER_ON(KBCTL_LYR) || fn_mode_enabled) {
        gpio_write_pin_low(LED_MAC_PIN); // low means turn on
    } else {
        gpio_write_pin_high(LED_MAC_PIN); // high means turn off
    }

    if (!keymap_config.no_gui) {
        // we have NOT enabled the no_gui,
        // we can re-use the Win Lock LED as NumLock indicator
        if (IS_LAYER_ON(NUM_LYR)) { // if the Num layer is active
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

bool fn_mode_enabled = false;

// *****************************
// * Custom processing of keys *
// *****************************
enum custom_keycodes { KC_SWP_FN = SAFE_RANGE };

// clang-format off
tap_dance_action_t tap_dance_actions[] = {

    [TD_RESET]     = ACTION_TAP_DANCE_FN(safe_reset),
    [TD_CLEAR]     = ACTION_TAP_DANCE_FN(safe_clear),

    // Tap: CAPS_LOCK; Hold: MO(EXT_LYR); Double Tap: TO(HRM_LYR); Double Hold: MO(NUM_LYR)
    [TD_MO_CAPS]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mo_caps_finished, mo_caps_reset),
    // on Tap: Esc; on Double Tap: `; on Hold: ``````
    [TD_GRV]       = ACTION_TAP_DANCE_FN_ADVANCED(NULL, grv_finished, grv_reset)
};
// clang-format on

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [BASE_LYR] = LAYOUT(
        KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,     KC_6,     KC_7,     KC_8,      KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,     KC_Y,     KC_U,     KC_I,      KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_HOME,
        MO_CAPS,   KC_A,      KC_S,      KC_D,      KC_F,      KC_G,     KC_H,     KC_J,     KC_K,      KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,   KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        KC_LCTL,   KC_LGUI,   KC_LALT,                         KC_SPC,                       MO_RALT,   MO_KBCTL,             KC_LEFT,            KC_DOWN,   KC_RGHT
    ),
    [HRM_BASE_LYR] = LAYOUT(
        MY_GRV,    _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   GUI_A,     ALT_S,     SFT_D,     CTL_F,     _______,  _______,  CTL_J,    SFT_K,     ALT_L,    GUI_SCLN,   _______,            _______,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      _______,   _______,              _______,            _______,   _______
    ),
    [EXT_LYR] = LAYOUT(
        KC_GRV,    _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   MY_CONS,   MY_TASK,   C(KC_F),   C(KC_R),   C(KC_H),  KC_PGUP,  KC_HOME,  KC_UP,     KC_END,   KC_PSCR,    KC_SCRL,  KC_PAUS,  KC_INS,    KC_END,
        _______,   KC_LALT,   KC_LGUI,   KC_LSFT,   KC_LCTL,   C(KC_G),  KC_PGDN,  KC_LEFT,  KC_DOWN,   KC_RIGHT, KC_HOME,    KC_END,             _______,   KC_SCRL,
        _______,   MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   KC_BSPC,  KC_DEL,   MY_BACK,   MY_FWD,   _______,    _______,            _______,   KC_RCTL,
        KC_SWP_FN, QK_LLCK,   _______,                         _______,                      KC_RCTL,   _______,              _______,            _______,   _______
    ),
    [KBCTL_LYR] = LAYOUT(
        _______,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX,    XXXXXXX,  XXXXXXX,  RM_TOGG,   _______,
        _______,   TD_KB_RST, XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  XXXXXXX,  RM_HUED,   RM_HUEU,  RGB_M_P,    RM_PREV,  RM_NEXT,  RM_TOGG,   QK_LLCK,
        _______,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  XXXXXXX,  RM_SATD,   RM_SATU,  RM_SPDD,    RM_SPDU,            _______,   TG_NUM,
        _______,   TD_KB_CLR, XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  NK_TOGG,  XXXXXXX,  RM_VALD,   RM_VALU,  TG_EXT,     TG_HRM,             RM_VALU,   TG_EXT,
        KC_SWP_FN, _______,   _______,                         _______,                      TG_NUM,    _______,              RM_SPDD,            RM_VALD,   RM_SPDU
    ),
    [NUM_LYR] = LAYOUT(
        _______,   _______,   _______,   _______,   _______,   _______,  KC_NUM,   KC_P7,    KC_P8,     KC_P9,   KC_PAST,    _______,  _______,  _______,   _______,
        _______,   KC_BTN1,   KC_MS_U,   KC_BTN2,   MSW_UP,    XXXXXXX,  XXXXXXX,  KC_P4,    KC_P5,     KC_P6,   KC_PPLS,    _______,  _______,  _______,   XXXXXXX,
        _______,   KC_MS_L,   KC_MS_D,   KC_MS_R,   MSW_DN,    XXXXXXX,  XXXXXXX,  KC_P1,    KC_P2,     KC_P3,   KC_PENT,    _______,            _______,   TG_NUM,
        _______,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  KC_P0,    KC_PDOT,   KC_PDOT, KC_PSLS,    _______,            _______,   XXXXXXX,
        _______,   _______,   _______,                         _______,                      TG_NUM,    _______,             _______,            _______,   _______
    ),
    [MEDIA_LYR] = LAYOUT(
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,    KC_VOLD,  KC_VOLU,  _______,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   QK_LLCK,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   TG_NUM,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   TG_EXT,
        _______,   _______,   _______,                         _______,                      TG_MEDIA, _______,              _______,            _______,   _______
    )
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [BASE_LYR]      = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [HRM_BASE_LYR]  = {ENCODER_CCW_CW(_______, _______)},
    [EXT_LYR]       = {ENCODER_CCW_CW(_______, _______)},
    [KBCTL_LYR]     = {ENCODER_CCW_CW(_______, _______)},
    [NUM_LYR]       = {ENCODER_CCW_CW(_______, _______)},
    [MEDIA_LYR]     = {ENCODER_CCW_CW(_______, _______)},
};
#endif
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == KC_SWP_FN) {
        if (record->event.pressed) {
            fn_mode_enabled = !fn_mode_enabled;
            blink_numbers(fn_mode_enabled);
            blink_space(true);
        }
        return false;
    }

    if (keycode == QK_LLCK){
        // when we lock or unlock a layer, flash the space bar area
        blink_space(true);

        if (IS_LAYER_ON(EXT_LYR)) {
            // blink the new arrow keys
            indicator_enqueue(I_KI, 150, 2, INDICATOR_RGB_DARK_RED); // up - I
            indicator_enqueue(J_KI, 150, 2, INDICATOR_RGB_DARK_RED); // left - J
            indicator_enqueue(K_KI, 150, 2, INDICATOR_RGB_DARK_RED); // down - K
            indicator_enqueue(L_KI, 150, 2, INDICATOR_RGB_DARK_RED); // right - L
        }
        // we only handled the flashing of the indicators, so keep processing the key code
    }

    if (!process_fn_mode(keycode, record)) { return false; }
    if (!process_rgb_keys(keycode, record)) { return false; }
    if (!dv_process_layer_lock(keycode, record, QK_LLCK)) { return false; }

    switch (keycode) {
        case KC_BSPC: {
            // based on: https://getreuer.info/posts/keyboards/macros3/index.html
            // shift + backspace is delete
            // both shift held is shift + delete
            static uint16_t registered_key = KC_NO;
            if (record->event.pressed) {  // On key press.
                const uint8_t mods = get_mods();
                #ifndef NO_ACTION_ONESHOT
                uint8_t shift_mods = (mods | get_oneshot_mods()) & MOD_MASK_SHIFT;
                #else
                        uint8_t shift_mods = mods & MOD_MASK_SHIFT;
                #endif  // NO_ACTION_ONESHOT
                if (shift_mods) {  // At least one shift key is held.
                    registered_key = KC_DEL;
                    // If one shift is held, clear it from the mods. But if both
                    // shifts are held, leave as is to send Shift + Del.
                    if (shift_mods != MOD_MASK_SHIFT) {
                    #ifndef NO_ACTION_ONESHOT
                        del_oneshot_mods(MOD_MASK_SHIFT);
                    #endif  // NO_ACTION_ONESHOT
                        unregister_mods(MOD_MASK_SHIFT);
                    }
                } else {
                    registered_key = KC_BSPC;
                }

                register_code(registered_key);
                set_mods(mods);
            } else {  // On key release.
                wait_ms(50); // wait a little bit, so programs don't filter the press
                unregister_code(registered_key);
            }
        }
        return false;

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
        default:
            return true;
    }

    return true;
}
