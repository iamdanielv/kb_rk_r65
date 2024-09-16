#include "fn_mode.h"

const uint16_t PROGMEM number_to_function[] = {KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12};

bool process_fn_mode(uint16_t keycode, keyrecord_t *record) {
    if (fn_mode_enabled) {
        if ((keycode >= KC_1 && keycode <= KC_0) || keycode == KC_MINS || keycode == KC_EQL) {
            uint8_t index = keycode - KC_1;

            // '-' and '=' are not in numerical order aligned with the numbers
            // they need special handling
            if (keycode == KC_MINS) {
                index = 10;
            } else if (keycode == KC_EQL) {
                index = 11;
            }

            if (record->event.pressed) {
                register_code(pgm_read_word(&number_to_function[index]));
            } else {
                unregister_code(pgm_read_word(&number_to_function[index]));
            }
            return false;
        }
    }
    return true;
}
