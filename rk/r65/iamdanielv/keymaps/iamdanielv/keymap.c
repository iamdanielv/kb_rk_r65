// Copyright 2024 iamdanielv (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "features/defines.h"
#include "quantum.h"

#include "features/dv_layer_lock.h"
#include "features/indicator_queue.h"
#include "features/fn_mode.h"
#include "features/tap_hold.h"
#include "features/indicators.h"
#include "features/rgb_keys.h"

/**
 * @brief Manages keyboard-related tasks, including LED indicators.
 *
 * This function is responsible for controlling the MAC LED based on the active layer
 * (KBCTL_LYR) or if FN key mode is enabled. It also re-uses the Win Lock LED as a
 * NumLock indicator if `keymap_config.no_gui` is not enabled and the NUM_LYR is active.
 */
void housekeeping_task_user(void) {
    // Note: We can decide what to do with the MAC Led in this function
    // if the Ctl layer is active or FN key mode is enabled
    if (IS_LAYER_ON(KBCTL_LYR) || fn_mode_enabled) {
        gpio_write_pin_low(LED_MAC_PIN); /**< low means turn on */
    } else {
        gpio_write_pin_high(LED_MAC_PIN); /**< high means turn off */
    }

    if (!keymap_config.no_gui) {
        // we have NOT enabled the no_gui,
        // we can re-use the Win Lock LED as NumLock indicator
        if (IS_LAYER_ON(NUM_LYR)) { /**< if the Num layer is active */
            // get the current LED state
            led_t led_state = host_keyboard_led_state();
            if (led_state.num_lock) {
                // Num/Win lock should be on
                gpio_write_pin_low(LED_WIN_LOCK_PIN);
            } else {
                // Num/Win lock should be off
                gpio_write_pin_high(LED_WIN_LOCK_PIN);
            }
        } /**< else we are not on the num layer, ignore */
    } /**< else we have enabled no_gui, skip re-using the LED */
}

bool fn_mode_enabled = false;
bool recalculate_rgb = true;

/**
 * @brief Custom processing of keycodes and tap dance actions
 */
/**
 * @brief Defines custom keycode for swapping FN mode.
 */
enum custom_keycodes { KC_SWP_FN = SAFE_RANGE };

// clang-format off
/**
 * @brief Tap dance actions definitions.
 *
 * This array defines various tap dance actions, including:
 * - `TD_RESET`: Resets the keyboard safely.
 * - `TD_CLEAR`: Clears the keyboard safely.
 * - `TD_MO_CAPS`: Tap for CAPS_LOCK, Hold for MO(EXT_LYR), Double Tap for TO(HRM_LYR), Double Hold for MO(NUM_LYR).
 * - `TD_GRV`: Tap for Esc, Double Tap for `, Hold for ```````.
 * - `TD_RALT`: Custom tap dance for right alt.
 */
tap_dance_action_t tap_dance_actions[] = {

    [TD_RESET]     = ACTION_TAP_DANCE_FN(safe_reset),
    [TD_CLEAR]     = ACTION_TAP_DANCE_FN(safe_clear),

    // Tap: CAPS_LOCK; Hold: MO(EXT_LYR); Double Tap: TO(HRM_LYR); Double Hold: MO(NUM_LYR)
    [TD_MO_CAPS]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mo_caps_finished, mo_caps_reset),
    // on Tap: Esc; on Double Tap: `; on Hold: ``````
    [TD_GRV]       = ACTION_TAP_DANCE_FN_ADVANCED(NULL, grv_finished, grv_reset),
    [TD_RALT]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ralt_finished, ralt_reset),
};
// clang-format on

// clang-format off
/**
 * @brief Defines the keymaps for different layers of the keyboard
 *
 * - `BASE_LYR`: The default QWERTY layer
 * - `HRM_BASE_LYR`: Home Row Mods base layer
 * - `EXT_LYR`: Extended layer
 * - `KBCTL_LYR`: Keyboard control layer
 * - `NUM_LYR`: Number pad layer
 * - `MEDIA_LYR`: Media control layer
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [BASE_LYR] = LAYOUT(
        MY_GRV,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,     KC_6,     KC_7,     KC_8,      KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,     KC_Y,     KC_U,     KC_I,      KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_HOME,
        MO_CAPS,   KC_A,      KC_S,      KC_D,      KC_F,      KC_G,     KC_H,     KC_J,     KC_K,      KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,   KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        KC_LCTL,   KC_LGUI,   KC_LALT,                         KC_SPC,                       MO_RALT,   MO_KBCTL,             KC_LEFT,            KC_DOWN,   KC_RGHT
    ),
    [HRM_BASE_LYR] = LAYOUT(
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   _______,   _______,   _______,   CTLR_R,    CTLH_T,   _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   GUI_A,     ALT_S,     SFT_D,     CTL_F,     CTLG_G,   _______,  CTL_J,    SFT_K,     ALT_L,    HM_SCLN,    END_QUOT,           MY_ENT,    _______,
        LSFT_LLCK, _______,   _______,   CTLS_C,    _______,   _______,  _______,  _______,  ALFT_COMM, ARGT_DOT, _______,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      _______,   _______,              _______,            _______,   _______
    ),
    [EXT_LYR] = LAYOUT(
        KC_GRV,    KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,    KC_F6,    KC_F7,    KC_F8,     KC_F9,    KC_F10,     KC_F11,   KC_F12,   KC_DEL,    _______,
        _______,   MY_CONS,   MY_TASK,   C(KC_F),   C(KC_R),   C(KC_H),  KC_PGUP,  KC_HOME,  KC_UP,     KC_END,   KC_PSCR,    KC_SCRL,  KC_PAUS,  KC_INS,    KC_END,
        _______,   KC_LGUI,   KC_LALT,   KC_LSFT,   KC_LCTL,   C(KC_G),  KC_PGDN,  KC_LEFT,  KC_DOWN,   KC_RIGHT, KC_HOME,    KC_END,             _______,   KC_SCRL,
        LSFT_LLCK, MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   KC_BSPC,  KC_DEL,   MY_BACK,   MY_FWD,   _______,    _______,            KC_VOLU,   KC_RCTL,
        _______,   _______,   _______,                         _______,                      KC_RCTL,   _______,              MY_BACK,            KC_VOLD,   MY_FWD
    ),
    [KBCTL_LYR] = LAYOUT(
        _______,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,   XXXXXXX,  XXXXXXX,    XXXXXXX,  XXXXXXX,  RM_TOGG,   _______,
        _______,   TD_KB_RST, XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  XXXXXXX,  RM_HUED,   RM_HUEU,  RGB_M_P,    RM_PREV,  RM_NEXT,  RM_TOGG,   _______,
        _______,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  XXXXXXX,  RM_SATD,   RM_SATU,  RM_SPDD,    RM_SPDU,            _______,   _______,
        LSFT_LLCK, TD_KB_CLR, XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  NK_TOGG,  XXXXXXX,  RM_VALD,   RM_VALU,  TG_EXT,     TG_HRM,             RM_VALU,   _______,
        KC_SWP_FN, _______,   _______,                         _______,                      TG_NUM,    _______,              RM_SPDD,            RM_VALD,   RM_SPDU
    ),
    [NUM_LYR] = LAYOUT(
        _______,   _______,   _______,   _______,   _______,   _______,  KC_NUM,   KC_P7,    KC_P8,     KC_P9,   KC_PAST,    _______,  _______,  _______,   _______,
        _______,   KC_BTN1,   KC_MS_U,   KC_BTN2,   MSW_UP,    XXXXXXX,  XXXXXXX,  KC_P4,    KC_P5,     KC_P6,   KC_PPLS,    _______,  _______,  _______,   _______,
        _______,   KC_MS_L,   KC_MS_D,   KC_MS_R,   MSW_DN,    XXXXXXX,  XXXXXXX,  KC_P1,    KC_P2,     KC_P3,   KC_PENT,    _______,            _______,   _______,
        LSFT_LLCK, XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,  XXXXXXX,  KC_P0,    KC_PDOT,   KC_PDOT, KC_PSLS,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      QK_LLCK,   _______,             _______,            _______,   _______
    ),
    [MEDIA_LYR] = LAYOUT(
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,    KC_VOLD,  KC_VOLU,  _______,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        LSFT_LLCK, _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      QK_LLCK,  _______,              _______,            _______,   _______
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

/**
 * @brief Inlined function declarations for key handlers.
 *
 * These functions are declared as `inline` to allow the compiler to potentially
 * insert their code directly at the point of call, optimizing performance.
 * sample: inline void myInlinedFunction() __attribute__((always_inline));
 */

// function definitions for key handlers
inline bool handle_backspace(keyrecord_t *record) __attribute__((always_inline));
inline bool handle_nkro_toggle(keyrecord_t *record) __attribute__((always_inline));
inline void handle_layer_lock_or_toggle_indicator(uint16_t keycode, keyrecord_t *record) __attribute__((always_inline));

/**
 * @brief Main function to process key presses.
 *
 * This function is the entry point for all key processing. It handles custom keycodes
 * like `KC_SWP_FN` and `QK_LLCK`, and then dispatches to other handlers for further processing.
 *
 * @param keycode The keycode of the pressed or released key.
 * @param record Pointer to the keyrecord_t structure containing key event details.
 * @return True if the pipeline should continue processing, false if the key was handled here.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == KC_SWP_FN) {
        if (record->event.pressed) {
            fn_mode_enabled = !fn_mode_enabled;
            blink_numbers(fn_mode_enabled);
            blink_space(true);
        }
        return false;
    }

    handle_layer_lock_or_toggle_indicator(keycode, record);

    if (!process_fn_mode(keycode, record)) { return false; }
    if (!process_rgb_keys(keycode, record)) { return false; }
    if (!dv_process_layer_lock(keycode, record, QK_LLCK)) { return false; }
    if (!handle_lt_0( keycode,  record)) { return false; }

    switch (keycode) {
        case KC_BSPC:
            return handle_backspace(record);
        case QK_MAGIC_TOGGLE_NKRO:
            return handle_nkro_toggle(record);
        default:
            // everything else should be handled normally
            return true;
    }

    return true;
}

/**
 * @brief Handles layer lock or toggle indicators
 *
 * This function checks for layer lock or toggle key presses and triggers LED indicators accordingly.
 * It flashes the space bar for any layer lock/toggle and flashes the EXT_LYR arrows if the EXT_LYR is toggled.
 *
 * @param keycode The keycode of the pressed or released key.
 * @param record Pointer to the keyrecord_t structure containing key event details.
 */
void handle_layer_lock_or_toggle_indicator(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        // we only really care about key presses and ignore key releases
        // Check for any layer lock or toggle key press
        if (keycode == QK_LLCK || IS_QK_TOGGLE_LAYER(keycode) || keycode == LSFT_LLCK) {
            // When we lock, unlock or toggle a layer, flash the space bar
            blink_space(true);

            // check if the toggle or layer lock is coming from the EXT_LYR or is for the EXT_LYR
            if (keycode == TG(EXT_LYR) || EXT_LYR == layer_switch_get_layer(record->event.key))  {
                // If it's the EXT_LYR toggle or layer lock, flash the EXT_LYR arrows
                indicator_enqueue(I_KI, INDCTR_INTVL_FAST, INDCTR_FLSH_DOUBLE, RGB_DRK_BLUE); // up - I
                indicator_enqueue(J_KI, INDCTR_INTVL_FAST, INDCTR_FLSH_DOUBLE, RGB_DRK_BLUE); // left - J
                indicator_enqueue(K_KI, INDCTR_INTVL_FAST, INDCTR_FLSH_DOUBLE, RGB_DRK_BLUE); // down - K
                indicator_enqueue(L_KI, INDCTR_INTVL_FAST, INDCTR_FLSH_DOUBLE, RGB_DRK_BLUE); // right - L
            }
        }
    }
}

/**
 * @brief Handles the backspace key, converting it to Delete when Shift is held.
 *
 * This function modifies the behavior of the backspace key. If a single Shift key
 * is held down, the backspace key sends `KC_DEL`. If both Shift keys are held down,
 * the backspace key sends `S(KC_DEL)` (Shift + Delete).
 * This is based on: https://getreuer.info/posts/keyboards/macros3/index.html
 *
 * @param record Pointer to the keyrecord_t structure containing key event details.
 * @return False to indicate that the key event has been fully handled and should not be processed further.
 */
bool handle_backspace(keyrecord_t *record) {
    // shift + backspace is delete
    // both shift held is shift + delete
    static uint16_t registered_key = KC_NO;
    if (record->event.pressed) { // On key press.
        const uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
        uint8_t shift_mods = (mods | get_oneshot_mods()) & MOD_MASK_SHIFT;
#else
        uint8_t shift_mods = mods & MOD_MASK_SHIFT;
#endif // NO_ACTION_ONESHOT
        if (shift_mods) { // At least one shift key is held.
            registered_key = KC_DEL;
            // If one shift is held, clear it from the mods. But if both
            // shifts are held, leave as is to send Shift + Del.
            if (shift_mods != MOD_MASK_SHIFT) {
#ifndef NO_ACTION_ONESHOT
                del_oneshot_mods(MOD_MASK_SHIFT);
#endif // NO_ACTION_ONESHOT
                unregister_mods(MOD_MASK_SHIFT);
            }
        } else {
            registered_key = KC_BSPC;
        }
        register_code(registered_key);
        set_mods(mods);
    } else {                     // On key release.
        wait_ms(TAP_CODE_DELAY); // wait a little bit, so programs don't filter the press
        unregister_code(registered_key);
    }
    return false;
}

/**
 * @brief Handles the NKRO (N-Key Rollover) toggle.
 *
 * This function is called when the NKRO toggle key is pressed. It clears the keyboard buffer,
 * toggles the `keymap_config.nkro` setting, blinks the NKRO indicator, and clears the buffer again.
 *
 * @param record Pointer to the keyrecord_t structure containing key event details.
 * @return False to indicate that the key event has been fully handled and should not be processed further.
 */
bool handle_nkro_toggle(keyrecord_t *record) {
    if (record->event.pressed) {
        clear_keyboard(); /**< clear first buffer to prevent stuck keys */
        wait_ms(50);
        keymap_config.nkro = !keymap_config.nkro;
        blink_NKRO(keymap_config.nkro);
        wait_ms(50);
        clear_keyboard(); /**< clear first buffer to prevent stuck keys */
        wait_ms(50);
    }
    return false;
}
