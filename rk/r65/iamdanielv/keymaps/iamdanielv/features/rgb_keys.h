#pragma once

#include QMK_KEYBOARD_H

/**
 * @brief Processes RGB keycodes.
 *
 * This function handles various RGB matrix related keycodes, such as toggling
 * RGB modes, changing speed, hue, saturation, and value.
 *
 * @param keycode The keycode that was pressed or released.
 * @param record The keyrecord structure containing information about the key event.
 * @return True if processing should continue, false if the keycode was handled here
 */
bool process_rgb_keys(uint16_t keycode, keyrecord_t *record);

/**
 * @brief Flag to indicate if RGB values need to be recalculated.
 */
extern bool recalculate_rgb;
