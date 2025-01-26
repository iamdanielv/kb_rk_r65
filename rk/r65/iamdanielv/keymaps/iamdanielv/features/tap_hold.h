#pragma once

#include "process_tap_dance.h"
#include QMK_KEYBOARD_H

enum tap_hold_keys {
    TD_RESET,   // require >= 3 taps to reset board
    TD_CLEAR,   // require >= 3 taps to clear eeprom
    TD_MO_CAPS, // Tap: caps lock; Hold: MO(EXT_LYR); Double Tap Hold: MO(NUM_LYR)
    TD_GRV      // Tap: `; Double Tap: ~; Hold: ``````
};

void safe_reset(tap_dance_state_t *state, void *user_data);

void safe_clear(tap_dance_state_t *state, void *user_data);

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP // Send two single taps
} td_state_t;

td_state_t cur_dance(tap_dance_state_t *state);

void mo_caps_finished(tap_dance_state_t *state, void *user_data);
void mo_caps_reset(tap_dance_state_t *state, void *user_data);

void grv_finished(tap_dance_state_t *state, void *user_data);
void grv_reset(tap_dance_state_t *state, void *user_data);
