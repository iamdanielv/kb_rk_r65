#include "tap_hold.h"
#include "action_layer.h"
#include "defines.h"
#include "quantum.h"

void safe_reset(tap_dance_state_t *state, void *user_data) {
    // from https://thomasbaart.nl/2018/12/13/qmk-basics-tap-dance/

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

// keep track of state for each tap dance
static td_state_t td_state[] = {
    [TD_RESET]   = TD_NONE,
    [TD_CLEAR]   = TD_NONE,
    [TD_MO_CAPS] = TD_NONE,
    [TD_GRV]     = TD_NONE
};

// *******************************************
// * This is based on example 4 from         *
// *  https://docs.qmk.fm/features/tap_dance *
// *******************************************

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
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else
            return TD_SINGLE_HOLD;
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

void mo_caps_finished(tap_dance_state_t *state, void *user_data) {
    td_state[TD_MO_CAPS] = cur_dance(state);
    switch (td_state[TD_MO_CAPS]) {
        case TD_SINGLE_HOLD:
            layer_on(EXT_LYR);
            break;
        case TD_DOUBLE_HOLD:
            layer_on(NUM_LYR);
            break;
        // the default case for the caps lock key should be caps lock
        // we only really modify single and double hold
        // to act as Momentary Layer switch
        // case TD_SINGLE_TAP:
        // case TD_DOUBLE_TAP:
        // case TD_DOUBLE_SINGLE_TAP:
        default:
            register_code16(KC_CAPS);
            break;
    }
}

void mo_caps_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state[TD_MO_CAPS]) {
        case TD_SINGLE_HOLD:
            if(!is_layer_locked(EXT_LYR)) {
                // only turn off the layer if it hasn't been locked
                layer_off(EXT_LYR);
            }
            break;
        case TD_DOUBLE_HOLD:
            // the NUM_LYR is not lockable
            // is is only used with a toggle
            // so it's ok to turn it off on release
            layer_off(NUM_LYR);
            break;
        // the default case for the caps lock key should be caps lock
        // we only really modify single and double hold
        // to act as Momentary Layer switch
        // case TD_SINGLE_TAP:
        // case TD_DOUBLE_TAP:
        // case TD_DOUBLE_SINGLE_TAP:
        default:
            wait_ms(100);
            unregister_code16(KC_CAPS);
            break;
    }
    td_state[TD_MO_CAPS] = TD_NONE;
}

void grv_finished(tap_dance_state_t *state, void *user_data) {
    td_state[TD_GRV] = cur_dance(state);
    switch (td_state[TD_GRV]) {
        case TD_DOUBLE_SINGLE_TAP:
            // the tap dance was interrupted,
            // handle it the same as if it was a double tap
        case TD_DOUBLE_TAP:
            register_code16(KC_ESC);
            break;
        case TD_SINGLE_TAP:
            register_code16(KC_GRV);
            break;
        case TD_DOUBLE_HOLD:
            // fall through to single hold
        case TD_SINGLE_HOLD:
            // type 6 ` then 3 lefts to put user in the middle
            SEND_STRING("``````");
            tap_code(KC_LEFT);
            tap_code(KC_LEFT);
            tap_code(KC_LEFT);
            break;
        default:
            break;
    }
}

void grv_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state[TD_GRV]) {
        case TD_DOUBLE_SINGLE_TAP:
            // the tap dance was interrupted,
            // handle it the same as if it was a double tap
        case TD_DOUBLE_TAP:
            wait_ms(50);
            unregister_code16(KC_ESC);
            break;
        case TD_SINGLE_TAP:
            wait_ms(50);
            unregister_code16(KC_GRV);
            break;
        // case TD_DOUBLE_HOLD:
        // case TD_SINGLE_HOLD:
        // both hold conditions terminate the sequence, so nothing to do
        default:
            break;
    }
    td_state[TD_GRV] = TD_NONE;
}
