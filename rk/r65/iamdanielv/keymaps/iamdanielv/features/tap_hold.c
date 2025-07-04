// Copyright 2025 DV (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "tap_hold.h"
#include "action_layer.h"
#include "defines.h"
#include "dv_layer_lock.h"
#include "quantum.h"

/**
 * @brief Reset the keyboard if you tap the key more than three times.
 *
 * @param state Tap dance state structure.
 */
void safe_reset(tap_dance_state_t* state, void* user_data) {
    // from https://thomasbaart.nl/2018/12/13/qmk-basics-tap-dance/

    if (state->count >= 3) {
        // Reset the keyboard if you tap the key more than three times
        reset_keyboard();
        reset_tap_dance(state);
    }
}

/**
 * @brief Clear eprom if you tap the key more than three times
 *
 * @param state Tap dance state structure.
 */
void safe_clear(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 3) {
        eeconfig_init();
        soft_reset_keyboard();
        reset_tap_dance(state);
    }
}

// keep track of state for each tap dance
static td_state_t td_state[] = {
    [TD_RESET]   = TD_NONE,
    [TD_CLEAR]   = TD_NONE,
    [TD_MO_CAPS] = TD_NONE,
    [TD_GRV]     = TD_NONE,
    [TD_RALT]    = TD_NONE
};

// **********************************************************
// * This is based on example 4 from:                       *
// *  https://docs.qmk.fm/features/tap_dance                *
// * Changed the single tap single hold so that if it's     *
// * tapped once only look at it's current pressed state    *
// * to determine if it's a tap or hold.                    *
// **********************************************************

/* Return an integer that corresponds to what kind of tap dance should be executed.
 *
 * How to figure out tap dance state: interrupted and pressed.
 *
 * Interrupted: If the state of a dance is "interrupted", that means that another key has been hit
 *  under the tapping term. This is typically indicative that you are trying to "tap" the key.
 *
 * Pressed: Whether or not the key is still being pressed. If this value is true, that means the tapping term
 *  has ended, but the key is still being pressed down. This generally means the key is being "held".
 *
 * One thing that is currently not possible with qmk software in regards to tap dance is to mimic the "permissive hold"
 *  feature. In general, advanced tap dances do not work well if they are used with commonly typed letters.
 *  For example "A". Tap dances are best used on non-letter keys that are not hit while typing letters.
 *
 * Good places to put an advanced tap dance:
 *  z,q,x,j,k,v,b, any function key, home/end, comma, semi-colon
 *
 * Criteria for "good placement" of a tap dance key:
 *  Not a key that is hit frequently in a sentence
 *  Not a key that is used frequently to double tap, for example 'tab' is often double tapped in a terminal, or
 *    in a web form. So 'tab' would be a poor choice for a tap dance.
 *  Letters used in common words as a double. For example 'p' in 'pepper'. If a tap dance function existed on the
 *    letter 'p', the word 'pepper' would be quite frustrating to type.
 *
 * For the third point, there does exist the 'TD_DOUBLE_SINGLE_TAP', however this is not fully tested
 *
 */
/**
 * @brief Determines the current tap dance state.
 *
 * This function analyzes the tap dance state (count, pressed, interrupted) to
 * return an appropriate `td_state_t` value indicating the type of tap dance
 * that should be executed (e.g., single tap, single hold, double tap).
 *
 * @param state Pointer to the `tap_dance_state_t` structure.
 * @return A `td_state_t` value representing the current tap dance state.
 */
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) {
            // key is not pressed, assume it is a tap, wether interrupted or not
            return TD_SINGLE_TAP;
        } else {
            // key is still held. Means you want to send a 'HOLD'.
            return TD_SINGLE_HOLD;
        }
    } else if (state->count >= 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted)
            return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return TD_DOUBLE_HOLD;
        else
            return TD_DOUBLE_TAP;
    } else
        return TD_UNKNOWN; // this shouldn't happen
    // we already handled any counts greater than 2 and just clamp it at double tap
}

/**
 * @brief Handles the finished state of the `TD_MO_CAPS` tap dance.
 *
 * This function is called when the `TD_MO_CAPS` tap dance sequence is completed.
 * It determines the action to take based on the tap dance state, such as
 * activating layers or registering keycodes.
 *
 * @param state Pointer to the `tap_dance_state_t` structure.
 * @param user_data User-defined data (not used in this function).
 */
void mo_caps_finished(tap_dance_state_t *state, void *user_data) {
    td_state[TD_MO_CAPS] = cur_dance(state);
    switch (td_state[TD_MO_CAPS]) {
        case TD_SINGLE_HOLD:
            layer_on(EXT_LYR);
            break;
        case TD_DOUBLE_HOLD:
            layer_on(NUM_LYR);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP: // dance was interrupted, handle it the same as if it was a double tap
            dv_layer_lock_invert(HRM_BASE_LYR);
            break;
        // the default case for the caps lock key should be caps lock
        case TD_SINGLE_TAP:
        default:
            register_code16(KC_CAPS);
            break;
    }
}

/**
 * @brief Resets the state of the `TD_MO_CAPS` tap dance.
 *
 * This function is called to reset the `TD_MO_CAPS` tap dance state after it
 * has been processed. It handles deactivating layers or unregistering keycodes
 * as necessary.
 *
 * @param state Pointer to the `tap_dance_state_t` structure.
 * @param user_data User-defined data (not used in this function).
 */
void mo_caps_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state[TD_MO_CAPS]) {
        case TD_SINGLE_HOLD:
            if (!dv_is_layer_locked(EXT_LYR)) {
                // only turn off the layer if it hasn't been locked
                layer_off(EXT_LYR);
            }
            break;
        case TD_DOUBLE_HOLD:
            if (!dv_is_layer_locked(NUM_LYR)) {
                // only turn off the layer if it hasn't been locked
                layer_off(NUM_LYR);
            }
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP: // dance was interrupted, handle it the same as if it was a double tap
            // this was handled in the finished function, nothing to do here
            break;
        case TD_SINGLE_TAP:
        default:
            wait_ms(TAP_CODE_DELAY);
            unregister_code16(KC_CAPS);
            break;
    }
    td_state[TD_MO_CAPS] = TD_NONE;
}

/**
 * @brief Handles the finished state of the `TD_GRV` tap dance.
 *
 * This function is called when the `TD_GRV` tap dance sequence is completed.
 * It determines the action to take based on the tap dance state, such as
 * registering keycodes or sending a string.
 *
 * @param state Pointer to the `tap_dance_state_t` structure.
 * @param user_data User-defined data (not used in this function).
 */
void grv_finished(tap_dance_state_t *state, void *user_data) {
    td_state[TD_GRV] = cur_dance(state);
    switch (td_state[TD_GRV]) {
        case TD_DOUBLE_SINGLE_TAP:
            // the tap dance was interrupted,
            // handle it the same as if it was a double tap
        case TD_DOUBLE_TAP:
            register_code16(KC_GRV);
            break;
        case TD_SINGLE_TAP:
            register_code16(KC_ESC);
            break;
        case TD_DOUBLE_HOLD:
            // fall through to single hold
        case TD_SINGLE_HOLD:
            // type 6 ` then 3 lefts to put user in the middle
            // this is good for code blocks in markdown
            SEND_STRING("``````");
            tap_code(KC_LEFT);
            tap_code(KC_LEFT);
            tap_code(KC_LEFT);
            break;
        default:
            break;
    }
}

/**
 * @brief Resets the state of the `TD_GRV` tap dance.
 *
 * This function is called to reset the `TD_GRV` tap dance state after it
 * has been processed. It handles unregistering keycodes as necessary.
 *
 * @param state Pointer to the `tap_dance_state_t` structure.
 * @param user_data User-defined data (not used in this function).
 */
void grv_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state[TD_GRV]) {
        case TD_DOUBLE_SINGLE_TAP:
            // the tap dance was interrupted,
            // handle it the same as if it was a double tap
        case TD_DOUBLE_TAP:
            wait_ms(TAP_CODE_DELAY);
            unregister_code16(KC_GRV);
            break;
        case TD_SINGLE_TAP:
            wait_ms(TAP_CODE_DELAY);
            unregister_code16(KC_ESC);
            break;
        // case TD_DOUBLE_HOLD:
        // case TD_SINGLE_HOLD:
        // both hold conditions terminate the sequence, so nothing to do
        default:
            break;
    }
    td_state[TD_GRV] = TD_NONE;
}

/**
 * @brief Handles the finished state of the `TD_RALT` tap dance.
 *
 * This function is called when the `TD_RALT` tap dance sequence is completed.
 * It determines the action to take based on the tap dance state, such as
 * activating layers or registering keycodes.
 *
 * @param state Pointer to the `tap_dance_state_t` structure.
 * @param user_data User-defined data (not used in this function).
 */
void ralt_finished(tap_dance_state_t *state, void *user_data) {
    td_state[TD_RALT] = cur_dance(state);
    switch (td_state[TD_RALT]) {
        case TD_SINGLE_HOLD:
            layer_on(MEDIA_LYR);
            break;
        case TD_SINGLE_TAP: // treat a single tap as a double hold
        case TD_DOUBLE_HOLD:
            register_code16(KC_RALT);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP: // dance was interrupted, handle it the same as if it was a double tap
            dv_layer_lock_invert(MEDIA_LYR);
            break;
        default:
            // do nothing
            break;
    }
}

/**
 * @brief Resets the state of the `TD_RALT` tap dance.
 *
 * This function is called to reset the `TD_RALT` tap dance state after it
 * has been processed. It handles deactivating layers or unregistering keycodes
 * as necessary.
 *
 * @param state Pointer to the `tap_dance_state_t` structure.
 * @param user_data User-defined data (not used in this function).
 */
void ralt_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state[TD_RALT]) {
        case TD_SINGLE_HOLD:
            if (!dv_is_layer_locked(MEDIA_LYR)) {
                // only turn off the layer if it hasn't been locked
                layer_off(MEDIA_LYR);
            }
            break;
        case TD_SINGLE_TAP: // treat a single tap as a double hold
        case TD_DOUBLE_HOLD:
            wait_ms(TAP_CODE_DELAY);
            unregister_code16(KC_RALT);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP: // dance was interrupted, handle it the same as if it was a double tap
            // this was handled in the finished function, nothing to do here
            break;
        default:
            // do nothing
            break;
    }
    td_state[TD_RALT] = TD_NONE;
}

/**
 * @brief Handles tap and hold behavior for `LT(0, KC)` keycodes.
 *
 * This function is a helper for `handle_lt_0` to manage the tap and hold
 * behavior of `LT(0, KC)` keycodes, where the key acts as a tap when pressed
 * and released quickly, and as a hold when held down.
 *
 * @param hold_keycode The keycode to register when the key is held.
 * @param record The keyrecord structure containing information about the key event.
 * @return True if the key was handled, false otherwise.
 */
bool handle_lt_0_tap_hold(uint16_t hold_keycode, keyrecord_t *record) {
    if (record->tap.count == 0) {
        if (record->event.pressed) {
            // we simplify the logic here to just do a tap,
            // but really the user is holding the key
            // side effect is that we don't support auto key repeat
            tap_code16(hold_keycode);
        }
        // we handled the key here, so no need for further processing
        return false;
    }
    // else we want processing of the key to continue normally
    return true;
}

/**
 * @brief Handles `LT(0, KC)` keycodes.
 *
 * This function processes `LT(0, KC)` keycodes, which are used for tap and hold
 * functionality on layer 0. It determines whether the key was tapped or held
 * and calls the appropriate handler.
 *
 * @param keycode The keycode that was pressed or released.
 * @param record The keyrecord structure containing information about the key event.
 * @return True if the keycode was handled, false otherwise.
 */
bool handle_lt_0(uint16_t keycode, keyrecord_t *record) {
    // check if this is a Layer tap key, return true means we need to keep processing
    if (!(keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) return true;

    // check if this is on layer 0
    // we re-use these keys since they are effectively no ops
    // but give us the tap and hold feature for free
    // return true means we are not processing here and the pipeline should ocntinue
    if (QK_LAYER_TAP_GET_LAYER(keycode) != 0) return true;

    switch (keycode) {
        case HM_SCLN:
            return handle_lt_0_tap_hold(KC_HOME, record);
            break;
        case END_QUOT:
            return handle_lt_0_tap_hold(KC_END, record);
            break;
        case ALFT_COMM:
            return handle_lt_0_tap_hold(A(KC_LEFT), record);
            break;
        case ARGT_DOT:
            return handle_lt_0_tap_hold(A(KC_RIGHT), record);
            break;
        case CTLH_T:
            return handle_lt_0_tap_hold(C(KC_H), record);
            break;
        case CTLR_R:
            return handle_lt_0_tap_hold(C(KC_R), record);
            break;
        case CTLG_G:
            return handle_lt_0_tap_hold(C(KC_G), record);
            break;
        case MY_ENT:
            // act as enter on tap, Shift on hold
            // By doing it this way, we can react immediately on key press
            if (record->event.pressed) {
                // we are registering a key
                if (record->tap.count) {
                    register_code16(KC_ENT);
                } else {
                    register_code16(KC_RSFT);
                }
            } else {
                // we are releasing a key
                if (record->tap.count) {
                    wait_ms(TAP_CODE_DELAY); // wait a little bit, so programs don't filter the press
                    unregister_code16(KC_ENT);
                } else {
                    unregister_code16(KC_RSFT);
                }
            }
            return false;
            break;
        case LSFT_LLCK:
            if (record->event.pressed) {
                // we are registering a key
                if (record->tap.count > 1) {
                    blink_space(true);
                    indicator_enqueue(LEFT_SFT_KI, INDCTR_INTVL_FAST, INDCTR_FLSH_DOUBLE, RGB_DRK_RED);
                    // require at least 2 taps in order to push layer lock
                    uint8_t current_layer = layer_switch_get_layer(record->event.key);
                    dv_layer_lock_invert(current_layer);
                } else {
                    register_code16(KC_LSFT);
                }
            } else {
                // we are releasing a key
                if (record->tap.count > 1) {
                    // nothing to do since the layer lock is handled on press
                } else {
                    unregister_code16(KC_LSFT);
                }
            }
            return false;
            break;
        default:
            // we want all other keys to be processed normally
            return true;
    }
}
