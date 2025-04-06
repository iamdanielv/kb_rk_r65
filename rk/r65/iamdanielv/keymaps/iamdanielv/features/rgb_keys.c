#include "indicators.h"
#include "defines.h"
#include "rgb_keys.h"
#include "indicator_queue.h"

bool process_rgb_keys(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RM_TOGG:
            if (record->event.pressed) {
                switch (rgb_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        rgb_matrix_set_flags_noeeprom(LED_FLAG_INDICATOR);
                        // rgb_matrix_set_color_all(0, 0, 0);
                        indicator_enqueue(SPACE_KI, 200, 3, INDICATOR_RGB_DARK_RED);     // blink space too
                        indicator_enqueue(LEFT_ALT_KI, 200, 3, INDICATOR_RGB_DARK_RED);  // blink left alt
                        indicator_enqueue(RIGHT_ALT_KI, 200, 3, INDICATOR_RGB_DARK_RED); // blink right alt
                    } break;
                    default: {
                        HSV current_hsv = rgb_matrix_get_hsv();
                        RGB rgb         = hsv_to_rgb(current_hsv);
                        rgb_matrix_set_color_all(rgb.r, rgb.g, rgb.b);
                        rgb_matrix_set_flags_noeeprom(LED_FLAG_ALL);
                        blink_space(true);
                    } break;
                }
            }
            return false;
        case RM_NEXT:
            if (record->event.pressed) {
                rgb_matrix_step_noeeprom();
            }
            return false;
        case RM_PREV:
            if (record->event.pressed) {
                rgb_matrix_step_reverse_noeeprom();
            }
            return false;
        case RGB_M_P:
            if (record->event.pressed) {
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                blink_space(true);
            }
            return false;
        case RM_SPDU:
            if (record->event.pressed) {
                if (rgb_matrix_get_speed() >= (255 - RGB_MATRIX_SPD_STEP)) {
                    // this update would put us at max
                    blink_arrows();
                    indicator_enqueue(QUOT_KI, 200, 4, INDICATOR_RGB_DARK_RED); // ' - UP
                    indicator_enqueue(SCLN_KI, 200, 2, RGB_BLACK);              // ; - DOWN
                    blink_space(false);
                }
                rgb_matrix_increase_speed_noeeprom();
            }
            return false;
        case RM_SPDD:
            if (record->event.pressed) {
                if (rgb_matrix_get_speed() <= RGB_MATRIX_SPD_STEP) {
                    blink_arrows();
                    indicator_enqueue(QUOT_KI, 200, 2, RGB_BLACK);              // ' - UP
                    indicator_enqueue(SCLN_KI, 200, 4, INDICATOR_RGB_DARK_RED); // ; - DOWN
                    blink_space(false);
                    rgb_matrix_set_speed_noeeprom(RGB_MATRIX_SPD_STEP);
                }
                rgb_matrix_decrease_speed_noeeprom();
            }
            return false;
        case RM_HUEU:
            if (record->event.pressed) {
                recalculate_rgb = true;

                if (rgb_matrix_get_hue() >= (255 - RGB_MATRIX_HUE_STEP)) {
                    // this update would put us at max
                    indicator_enqueue(O_KI, 200, 4, INDICATOR_RGB_DARK_RED); // O - UP
                    indicator_enqueue(I_KI, 200, 2, RGB_BLACK);              // I - DOWN
                    blink_space(false);
                } else {
                    indicator_enqueue(O_KI, 150, 1, RGB_WHITE); // O - UP
                }
                rgb_matrix_increase_hue_noeeprom();
            }
            return false;
        case RM_HUED:
            if (record->event.pressed) {
                recalculate_rgb = true;

                if (rgb_matrix_get_hue() <= RGB_MATRIX_HUE_STEP) {
                    // this update would put us at min
                    indicator_enqueue(O_KI, 200, 2, RGB_BLACK);              // O - UP
                    indicator_enqueue(I_KI, 200, 4, INDICATOR_RGB_DARK_RED); // I - DOWN
                    blink_space(false);
                } else {
                    indicator_enqueue(I_KI, 150, 1, RGB_WHITE); // I - DOWN
                }
                rgb_matrix_decrease_hue_noeeprom();
            }
            return false;
        case RM_SATU:
            if (record->event.pressed) {
                recalculate_rgb = true;

                if (rgb_matrix_get_sat() >= (255 - RGB_MATRIX_SAT_STEP)) {
                    // this update would put us at max
                    indicator_enqueue(L_KI, 200, 4, INDICATOR_RGB_DARK_RED); // L - UP
                    indicator_enqueue(K_KI, 200, 2, RGB_BLACK);              // K - DOWN
                    blink_space(false);
                } else {
                    indicator_enqueue(L_KI, 150, 1, RGB_WHITE); // L - UP
                }
                rgb_matrix_increase_sat_noeeprom();
            }
            return false;
        case RM_SATD:
            if (record->event.pressed) {
                recalculate_rgb = true;

                if (rgb_matrix_get_sat() <= RGB_MATRIX_SAT_STEP) {
                    // this update would put us at min
                    indicator_enqueue(L_KI, 200, 2, RGB_BLACK);              // L - UP
                    indicator_enqueue(K_KI, 200, 4, INDICATOR_RGB_DARK_RED); // K - DOWN
                    blink_space(false);
                } else {
                    indicator_enqueue(K_KI, 150, 1, RGB_WHITE); // K - DOWN
                }
                rgb_matrix_decrease_sat_noeeprom();
            }
            return false;
        case RM_VALU:
            if (record->event.pressed) {
                recalculate_rgb = true;

                if (rgb_matrix_get_val() >= (RGB_MATRIX_MAXIMUM_BRIGHTNESS - RGB_MATRIX_VAL_STEP)) {
                    indicator_enqueue(DOT_KI, 200, 4, INDICATOR_RGB_DARK_RED); // . - UP
                    indicator_enqueue(COMM_KI, 200, 2, RGB_BLACK);             // , - DOWN
                    blink_space(false);
                    blink_arrows();
                } else {
                    indicator_enqueue(DOT_KI, 150, 1, RGB_WHITE); // . - UP
                }
                rgb_matrix_increase_val_noeeprom();
            }
            return false;
        case RM_VALD:
            if (record->event.pressed) {
                recalculate_rgb = true;

                if (rgb_matrix_get_val() <= RGB_MATRIX_VAL_STEP) {
                    indicator_enqueue(DOT_KI, 200, 2, RGB_BLACK);               // . - UP
                    indicator_enqueue(COMM_KI, 200, 4, INDICATOR_RGB_DARK_RED); // , - DOWN
                    blink_space(false);
                    blink_arrows();
                } else {
                    indicator_enqueue(COMM_KI, 150, 1, RGB_WHITE); // , - DOWN
                }
                rgb_matrix_decrease_val_noeeprom();
            }
            return false;
    }
    return true;
}
