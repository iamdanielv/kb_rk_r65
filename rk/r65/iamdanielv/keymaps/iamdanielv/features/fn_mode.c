#include "fn_mode.h"
#include "stdint.h"     // For uint8_t, uint16_t
#include "action.h"     // For register_code, unregister_code
#include "keycodes.h"   // For KC_F1, KC_1, etc.
#include "quantum.h"    // For get_mods

// Contains two sets of 12 keycodes.
// The first set is for standard function keys (F1-F12).
// The second set is for multimedia and function keys, activated by holding Shift.
const uint16_t PROGMEM number_to_function[] = {
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
    // Shifted versions
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU
};

// Helper to register/unregister the remapped keycode
static void register_remapped_key(uint8_t index, bool pressed) {
    uint16_t key_to_register = pgm_read_word(&number_to_function[index]);
    if (pressed) {
        register_code(key_to_register);
    } else {
        unregister_code(key_to_register);
    }
}

bool process_fn_mode(uint16_t keycode, keyrecord_t *record) {
    if (fn_mode_enabled) {
        if ((keycode >= KC_1 && keycode <= KC_0) || keycode == KC_MINS || keycode == KC_EQL) {
            uint8_t index = keycode - KC_1;

            // '-' and '=' are not in numerical order aligned with the numbers
            // they need special handling
            if (keycode == KC_MINS) { index = 10;}
            else if (keycode == KC_EQL) { index = 11;}

            if (get_mods() & MOD_MASK_SHIFT) {
                index += 12;
            }

            register_remapped_key(index, record->event.pressed);
            return false; // Key handled.
        }
        // Not a key we handle, process normally
        return true;
     } else {
        // When fn_mode_enabled is off, check for Shift + F-key combinations
        if (!(get_mods() & MOD_MASK_SHIFT)) {
            // Shift not held, process key normally
            return true;
        }

        // we know shift is currently held, check if it's one of the function keys
        if (keycode >= KC_F1 && keycode <= KC_F12) {
            // since shift is held, we can use the second set of F key values
            uint8_t index = (keycode - KC_F1) + SHIFT_F_OFFSET;
            register_remapped_key(index, record->event.pressed);
            // Key handled here
            return false;
        }

        // Not a key we handle, process normally
        return true;
    }
}
