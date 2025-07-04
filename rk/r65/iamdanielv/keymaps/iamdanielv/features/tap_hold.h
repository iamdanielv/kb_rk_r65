// Copyright 2025 DV (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "process_tap_dance.h"
#include QMK_KEYBOARD_H

#include "indicator_queue.h"
#include "indicators.h"

/**
 * @brief Enum for custom tap dance keycodes.
 */
enum tap_hold_keys {
    TD_RESET,   // require >= 3 taps to reset board
    TD_CLEAR,   // require >= 3 taps to clear eeprom
    TD_MO_CAPS, // Tap: caps lock; Hold: MO(EXT_LYR); Double Tap: layer lock HRM_BASE_LYR; Double Hold: MO(NUM_LYR)
    TD_GRV,     // Tap: ESC; Double Tap: `; Hold: ``````
    TD_RALT     // Tap: KC_RALT; Hold: mo(MEDIA_LYR); Double Tap: layer lock MEDIA_LYR; Double Hold: KC_RALT
};

/**
 * @brief Resets the keyboard if the key is tapped more than three times.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
void safe_reset(tap_dance_state_t *state, void *user_data);

/**
 * @brief Clears the EEPROM if the key is tapped more than three times.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
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

/**
 * @brief Determines the current tap dance state.
 *
 * @param state Pointer to the tap dance state structure.
 * @return The current tap dance state.
 */
td_state_t cur_dance(tap_dance_state_t *state);

/**
 * @brief Handles the finished state of the `TD_MO_CAPS` tap dance.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
void mo_caps_finished(tap_dance_state_t *state, void *user_data);
/**
 * @brief Resets the state of the `TD_MO_CAPS` tap dance.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
void mo_caps_reset(tap_dance_state_t *state, void *user_data);

/**
 * @brief Handles the finished state of the `TD_GRV` tap dance.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
void grv_finished(tap_dance_state_t *state, void *user_data);
/**
 * @brief Resets the state of the `TD_GRV` tap dance.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
void grv_reset(tap_dance_state_t *state, void *user_data);

/**
 * @brief Handles the finished state of the `TD_RALT` tap dance.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
void ralt_finished(tap_dance_state_t *state, void *user_data);
/**
 * @brief Resets the state of the `TD_RALT` tap dance.
 *
 * @param state Pointer to the tap dance state structure.
 * @param user_data User-defined data.
 */
void ralt_reset(tap_dance_state_t *state, void *user_data);

/**
 * @brief Handles `LT(0, KC)` keycodes.
 *
 * @param keycode The keycode that was pressed or released.
 * @param record The keyrecord structure.
 * @return True if the keycode was handled, false otherwise.
 */
bool handle_lt_0(uint16_t keycode, keyrecord_t *record);
