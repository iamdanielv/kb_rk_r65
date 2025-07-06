#include "fn_mode.h"
#include "stdint.h"
#include "action.h"
#include "keycodes.h"
#include "quantum.h"

// range of keycodes that should not be auto-repeated
#define NO_AUTO_REPEAT_START_INDEX 12
#define NO_AUTO_REPEAT_END_INDEX 15

#define SHIFT_OFFSET 12

/**
 * @brief Defines the mapping of number keys to function, shortcut and multimedia keys
 *
 * This array contains two sets of 12 keycodes. The first set corresponds to
 * the standard function keys (F1-F12). The second set, activated by holding
 * Shift, corresponds to shortcut and multimedia keys.
 */
const uint16_t PROGMEM number_to_function[] = {
    KC_F1,        KC_F2,       KC_F3,       KC_F4,   KC_F5, KC_F6, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
    // Shifted versions
    LGUI(KC_TAB), MEH(KC_TAB), LCA(KC_TAB), C(KC_H), KC_F5, KC_F6, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU
};

/**
 * @brief Registers or unregisters the remapped keycode
 *
 * This helper function reads the appropriate keycode from the number_to_function
 * array and handles the registration or unregistration, including managing modifier
 * state. It also handles special cases for keys that should not auto-repeat.
 *
 * @param index The index into the number_to_function array.
 * @param pressed True if the key was pressed, false if released.
 */
static void register_remapped_key(uint8_t index, bool pressed) {
    uint16_t key_to_register = pgm_read_word(&number_to_function[index]);
    if (pressed) {
        const uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
        uint8_t shift_mods = (mods | get_oneshot_mods()) & MOD_MASK_SHIFT;
#else
        uint8_t shift_mods = mods & MOD_MASK_SHIFT;
#endif // NO_ACTION_ONESHOT
        if (shift_mods) { // At least one shift key is held.
            // If one shift is held, clear it from the mods. But if both
            // shifts are held, leave as is to send Shift + Key code.
            if (shift_mods != MOD_MASK_SHIFT) {
#ifndef NO_ACTION_ONESHOT
                del_oneshot_mods(MOD_MASK_SHIFT);
#endif // NO_ACTION_ONESHOT
                unregister_mods(MOD_MASK_SHIFT);
            }
        }
        if(index >= NO_AUTO_REPEAT_START_INDEX && index <= NO_AUTO_REPEAT_END_INDEX) {
            // we don't want the shifted keys in these indexes to auto repeat
            // since they act weird on repeat
            tap_code16(key_to_register);
        } else {
            register_code16(key_to_register);
        }
        set_mods(mods);
    } else {
        if(index >= NO_AUTO_REPEAT_START_INDEX && index <= NO_AUTO_REPEAT_END_INDEX) {
            // these were handled on key press with a tap
            // nothing to do here
            return;
        }
        wait_ms(TAP_CODE_DELAY); // wait a little bit, so programs don't filter the press
        unregister_code16(key_to_register);
    }
}

/**
 * @brief Handles key processing when fn_mode is enabled
 *
 * When fn_mode is active, this function remaps the number row keys (1 through =,
 * including -, and =) to their corresponding function keys (F1-F12).
 * If Shift is held, it maps to the second set of multimedia/shortcut keys.
 *
 * @param keycode The keycode that was pressed or released.
 * @param record The keyrecord structure containing information about the key event.
 * @return True if processing should continue, false if the keycode was handled.
 */
static bool process_fn_mode_enabled(uint16_t keycode, keyrecord_t *record) {
    if ((keycode >= KC_1 && keycode <= KC_0) || keycode == KC_MINS || keycode == KC_EQL) {
        uint8_t index = keycode - KC_1;

        // '-' and '=' are not in numerical order aligned with the numbers
        // they need special handling
        if (keycode == KC_MINS) { index = 10;}
        else if (keycode == KC_EQL) { index = 11;}

        if (get_mods() & MOD_MASK_SHIFT) {
            index += SHIFT_OFFSET;
        }

        register_remapped_key(index, record->event.pressed);
        return false; // Key handled.
    }
    // Not a key we handle, process normally
    return true;
}

/**
 * @brief Handles key processing when fn_mode is disabled
 *
 * When fn_mode is disabled, this function remaps Shift + F-keys (F1-F12)
 * to the second set of multimedia/shortcut keys from the number_to_function array.
 *
 * @param keycode The keycode that was pressed or released.
 * @param record The keyrecord structure containing information about the key event.
 * @return True if processing should continue, false if the keycode was handled.
 */
static bool process_fn_mode_disabled(uint16_t keycode, keyrecord_t *record) {
    if (!(get_mods() & MOD_MASK_SHIFT)) {
        // Shift not held, process key normally
        return true;
    }

    // we know shift is currently held, check if it's one of the function keys
    if (keycode >= KC_F1 && keycode <= KC_F12) {
        // since shift is held, we can use the second set of F key values
        uint8_t index = (keycode - KC_F1) + SHIFT_OFFSET;
        register_remapped_key(index, record->event.pressed);
        // Key handled here
        return false;
    }

    // Not a key we handle, process normally
    return true;
}

/**
 * @brief Processes key events to remap keys based on the fn_mode state and shift key.
 *
 * This is the main entry point for the fn_mode feature. It delegates processing
 * to either process_fn_mode_enabled or process_fn_mode_disabled based on the
 * current state of the fn_mode_enabled flag.
 *
 * @param keycode The keycode that was pressed or released.
 * @param record The keyrecord structure containing information about the key event.
 * @return True if processing should continue, false if the keycode was handled.
 */
bool process_fn_mode(uint16_t keycode, keyrecord_t *record) {
    if (fn_mode_enabled) {
        return process_fn_mode_enabled(keycode, record);
    } else {
        return process_fn_mode_disabled(keycode, record);
    }
}
