#include "quantum_keycodes_legacy.h"
#include "rgb_matrix.h"

#include "indicators.h"
#include "rgb_keys.h"
#include "indicator_queue.h"


bool process_rgb_keys(uint16_t keycode, keyrecord_t *record){

    // we only care about pressed events, return as soon as possible
    if (!record->event.pressed) { return true;}

    switch (keycode) {
        case RGB_TOG:
            switch (rgb_matrix_get_flags()) {
                case LED_FLAG_ALL: {
                    rgb_matrix_set_flags_noeeprom(LED_FLAG_INDICATOR);
                    // rgb_matrix_set_color_all(0, 0, 0);
                    indicator_enqueue(65, 200, 3, INDICATOR_RGB_DARK_RED); // blink space too
                    indicator_enqueue(0, 200, 3, INDICATOR_RGB_DARK_RED);  // blink left alt
                    indicator_enqueue(64, 200, 3, INDICATOR_RGB_DARK_RED); // blink right alt
                } break;
                default: {
                    HSV current_hsv = rgb_matrix_get_hsv();
                    RGB rgb         = hsv_to_rgb(current_hsv);
                    rgb_matrix_set_color_all(rgb.r, rgb.g, rgb.b);
                    rgb_matrix_set_flags_noeeprom(LED_FLAG_ALL);
                    blink_space(true);
                } break;
            }
            return false;
        case RGB_MOD:
            rgb_matrix_step_noeeprom();
            return false;
        case RGB_RMOD:
            rgb_matrix_step_reverse_noeeprom();
            return false;
        case RGB_M_P:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            blink_space(true);
            return false;
        case RGB_SPI:
            if (rgb_matrix_get_speed() >= (255 - RGB_MATRIX_SPD_STEP)) {
                // this update would put us at max
                blink_arrows();
                indicator_enqueue(17, 200, 4, RGB_RED);   // ' - SPI
                indicator_enqueue(18, 200, 2, RGB_BLACK); // ; - SPD
                blink_space(false);
            }
            rgb_matrix_increase_speed_noeeprom();
            return false;
        case RGB_SPD:
            if (rgb_matrix_get_speed() <= RGB_MATRIX_SPD_STEP) {
                blink_arrows();
                indicator_enqueue(17, 200, 2, RGB_BLACK); // ' - SPI
                indicator_enqueue(18, 200, 4, RGB_RED);   // ; - SPD
                blink_space(false);
                rgb_matrix_set_speed_noeeprom(RGB_MATRIX_SPD_STEP);
            }
            rgb_matrix_decrease_speed_noeeprom();
            return false;
        case RGB_HUI:
            if (rgb_matrix_get_hue() >= (255 - RGB_MATRIX_HUE_STEP)) {
                // this update would put us at max
                indicator_enqueue(38, 200, 4, INDICATOR_RGB_DARK_RED); // O - HUI
                indicator_enqueue(37, 200, 2, RGB_BLACK);              // I - HUD
                blink_space(false);
            }
            rgb_matrix_increase_hue_noeeprom();
            return false;
        case RGB_HUD:
            if (rgb_matrix_get_hue() <= RGB_MATRIX_HUE_STEP) {
                // this update would put us at min
                indicator_enqueue(38, 200, 2, RGB_BLACK);              // O - HUI
                indicator_enqueue(37, 200, 4, INDICATOR_RGB_DARK_RED); // I - HUD
                blink_space(false);
            }
            rgb_matrix_decrease_hue_noeeprom();
            return false;
        case RGB_SAI:
            if (rgb_matrix_get_sat() >= (255 - RGB_MATRIX_SAT_STEP)) {
                // this update would put us at max
                indicator_enqueue(19, 200, 4, INDICATOR_RGB_DARK_RED); // L - SAI
                indicator_enqueue(20, 200, 2, RGB_BLACK);              // K - SAD
                blink_space(false);
            }
            rgb_matrix_increase_sat_noeeprom();
            return false;
        case RGB_SAD:
            if (rgb_matrix_get_sat() <= RGB_MATRIX_SAT_STEP) {
                // this update would put us at min
                indicator_enqueue(19, 200, 2, RGB_BLACK);              // L - SAI
                indicator_enqueue(20, 200, 4, INDICATOR_RGB_DARK_RED); // K - SAD
                blink_space(false);
            }
            rgb_matrix_decrease_sat_noeeprom();
            return false;
        case RGB_VAI:
            if (rgb_matrix_get_val() >= (RGB_MATRIX_MAXIMUM_BRIGHTNESS - RGB_MATRIX_VAL_STEP)) {
                indicator_enqueue(12, 200, 4, RGB_RED);   // . - VAI
                indicator_enqueue(11, 200, 2, RGB_BLACK); // , - VAD
                blink_space(false);
                blink_arrows();
            }
            rgb_matrix_increase_val_noeeprom();
            return false;
        case RGB_VAD:
            if (rgb_matrix_get_val() <= RGB_MATRIX_VAL_STEP) {
                indicator_enqueue(12, 200, 2, RGB_BLACK); // . - VAI
                indicator_enqueue(11, 200, 4, RGB_RED);   // , - VAD
                blink_space(false);
                blink_arrows();
            }
            rgb_matrix_decrease_val_noeeprom();
            return false;
    }
    return true;
}
