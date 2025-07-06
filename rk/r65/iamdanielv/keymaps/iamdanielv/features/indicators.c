#include "defines.h"
#include "indicators.h"
#include "dv_layer_lock.h"
#include "indicator_queue.h"
#include "fn_mode.h"
#include "color.h"
#include "quantum.h"
#include "rgb_matrix.h"

/**
 * @brief Calculates the complementary RGB color.
 *
 * @param rgb_led The original RGB color.
 * @param darken If true, darkens the complementary color.
 * @return The complementary RGB color.
 */
rgb_t get_complementary_rgb(rgb_t rgb_led, bool darken) {
    uint8_t new_r = 0xFF - rgb_led.r;
    uint8_t new_g = 0xFF - rgb_led.g;
    uint8_t new_b = 0xFF - rgb_led.b;

    if (darken) {
        // darken the new color by shifting all values down
        if( new_r > 0x80) { new_r = new_r - 0x80;}
        if( new_g > 0x80) { new_g = new_g - 0x80;}
        if( new_b > 0x80) { new_b = new_b - 0x80;}
    }

    return (rgb_t){.r = new_r, .g = new_g, .b = new_b};
}

/**
 * @brief Shifts the hue of an HSV color.
 *
 * @param color The original HSV color.
 * @param offset The amount to shift the hue.
 * @param clockwise If true, shifts the hue clockwise; otherwise, counter-clockwise.
 * @return The HSV color with the shifted hue.
 */
hsv_t get_hsv_color_shifted(hsv_t color, uint8_t offset, bool clockwise) {
    // it's ok if it overflows, the byte will handle it
    if (clockwise) {
        color.h = color.h - offset;
    } else {
        color.h = color.h + offset;
    }
    return color;
}

/**
 * @brief Gets the inverse of the base HSV color.
 *
 * @return The inverse of the base HSV color.
 */
hsv_t get_base_hsv_color_inverse(void) {
    // get the current base hsv value
    hsv_t base_color = rgb_matrix_get_hsv();

    return get_hsv_color_shifted(base_color, HUE_INVERSE, false);
}

/**
 * @brief Shifts the base HSV color.
 *
 * @param clockwise If true, shifts the hue clockwise; otherwise, counter-clockwise.
 * @return The shifted base HSV color.
 */
hsv_t get_base_hsv_color_shifted(bool clockwise) {
    hsv_t base_color = rgb_matrix_get_hsv();

    return get_hsv_color_shifted(base_color, HUE_SHIFTED, clockwise);
}

/**
 * @brief Shifts the base HSV color by a quarter turn.
 *
 * @param clockwise If true, shifts the hue clockwise; otherwise, counter-clockwise.
 * @return The shifted base HSV color.
 */
hsv_t get_base_hsv_color_shifted_quarter(bool clockwise) {
    // get the current base hsv value
    hsv_t base_color = rgb_matrix_get_hsv();

    // offset hue by a quarter
    return get_hsv_color_shifted(base_color, HUE_QUARTER, clockwise);
}

const uint8_t numbers_keys[] = {N1_KI, N2_KI, N3_KI, N4_KI, N5_KI, N6_KI, N7_KI, N8_KI, N9_KI, N0_KI, MINS_KI, EQL_KI};

/**
 * @brief Blinks the number keys.
 *
 * @param isEnabling If true, blinks the keys with a white color; otherwise, blinks with a red color.
 */
void blink_numbers(bool isEnabling) {
    for (int num = 0; num < 12; num++) {
        if (isEnabling) {
            // enabling, flash white
            indicator_enqueue(numbers_keys[num], INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_WHITE);
        } else {
            // disabling, flash red
            indicator_enqueue(numbers_keys[num], INDCTR_INTVL_FAST, INDCTR_FLSH_DOUBLE, RGB_RED);
        }
    }
}

/**
 * @brief Blinks the arrow keys (up, left, down, right) with white color.
 */
void blink_arrows(void) {
    indicator_enqueue(UP_KI, INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_WHITE);    // up
    indicator_enqueue(LEFT_KI, INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_WHITE);  // left
    indicator_enqueue(DOWN_KI, INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_WHITE);  // down
    indicator_enqueue(RIGHT_KI, INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_WHITE); // right
}

/**
 * @brief Blinks the space key, and optionally the left and right Alt keys.
 *
 * @param extended If true, also blinks the left and right Alt keys.
 */
void blink_space(bool extended) {
    indicator_enqueue(SPACE_KI, INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_WHITE); // blink space
    if (extended) {
        indicator_enqueue(LEFT_ALT_KI, INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_BLACK);  // blink left alt
        indicator_enqueue(RIGHT_ALT_KI, INDCTR_INTVL_NORMAL, INDCTR_FLSH_DOUBLE, RGB_BLACK); // blink right alt
    }
}

/**
 * @brief Blinks the NKRO keys.
 *
 * @param isEnabling If true, blinks the keys with a white color; otherwise, blinks with a red color.
 */
void blink_NKRO(bool isEnabling) {
    if (isEnabling) {
        const uint8_t led_indexes[12] = {
            V_KI, B_KI, N_KI, M_KI, COMM_KI, // V B N M ,
            G_KI, H_KI, J_KI, K_KI,          // G H J K
            Y_KI, U_KI, I_KI                 // Y U I
        };

        for (int i = 0; i < 12; i++) {
            indicator_enqueue(led_indexes[i], INDCTR_INTVL_FAST, INDCTR_FLSH_DOUBLE, RGB_WHITE);
        }
    } else {
        const uint8_t led_indexes[4] = {
            B_KI, M_KI, // B M
            H_KI, J_KI  // H J
        };

        for (int i = 0; i < 4; i++) {
            indicator_enqueue(led_indexes[i], INDCTR_INTVL_FAST, INDCTR_FLSH_TRIPLE, RGB_RED);
        }
    }
}

/**
 * @brief Highlights the function keys with a specified color.
 *
 * @param color The RGB color to use for highlighting.
 * @param led_min The minimum LED index to consider.
 * @param led_max The maximum LED index to consider.
 */
void highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max) {
    for (int num = 0; num < 12; num++) {
        RGB_MATRIX_INDICATOR_SET_COLOR(numbers_keys[num], color.r, color.g, color.b);
    }
}

/**
 * @brief Sets the color of a list of keys.
 *
 * @param color The RGB color to set.
 * @param led_min The minimum LED index to consider.
 * @param led_max The maximum LED index to consider.
 * @param key_indexes An array of key indexes to set the color for.
 * @param num_keys The number of keys in the `key_indexes` array.
 */
void set_keys_color(rgb_t color, uint8_t led_min, uint8_t led_max, const uint8_t key_indexes[], int num_keys) {
    for (int i = 0; i < num_keys; i++) {
        RGB_MATRIX_INDICATOR_SET_COLOR(key_indexes[i], color.r, color.g, color.b);
    }
}

/**
 * @brief Sets the color of a layer indicator LED.
 *
 * @param layer The layer for which to set the indicator, also checks if it's locked.
 * @param led_index The LED index to use as the indicator.
 * @param color The RGB color to set for the indicator.
 * @param led_min The minimum LED index to consider.
 * @param led_max The maximum LED index to consider.
 */
void set_layer_color_indicator(uint8_t layer, uint8_t led_index, rgb_t color, uint8_t led_min, uint8_t led_max) {
    if (dv_is_layer_locked(layer)) {
        // no matter what, always use the left shift as an indicator for layer lock
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_SFT_KI, 0xAA, 0x22, 0x00);
    }

    // we use the passed in led_index to show the layer color
    RGB_MATRIX_INDICATOR_SET_COLOR(led_index, color.r, color.g, color.b);
}

/**
 * @brief Advanced user function for RGB matrix indicators.
 *
 * This function is called by the RGB matrix code to allow for custom LED indicator behavior.
 * It handles various layer indicators, caps lock, and NKRO status.

 * @param led_min The minimum LED index to consider.
 * @param led_max The maximum LED index to consider.
 * @return True if the pipeline should continue processing, false if processing should stop.
 */
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // check for caps lock
    if (host_keyboard_led_state().caps_lock) {
        // we can use the LED Indicator for CAPS_LOCK as well
        RGB_MATRIX_INDICATOR_SET_COLOR(CAPS_KI, 255, 0, 0);
    }

    uint8_t current_layer = get_highest_layer(layer_state);
    if (current_layer == BASE_LYR) {
        if (rgb_matrix_get_flags() == LED_FLAG_INDICATOR) {
            for (int i = led_min; i < led_max; i++) {
                rgb_matrix_set_color(i, 0, 0, 0);
            }
            // we could also do this, but using the min max
            // updates a smaller subset at once
            // rgb_matrix_set_color_all(0, 0, 0);
        }
    }

    static rgb_t ext_lyr_rgb;
    static rgb_t accent_lyr_rgb;
    static rgb_t num_lyr_rgb;
    static rgb_t dual_role_rgb;

    if (recalculate_rgb) {
        // determine the colors to use for each of the layers
        // hsv_t base_hsv_offset_qrt_ccw = get_base_hsv_color_shifted_quarter(false);
        hsv_t base_hsv_offset_qrt_cw   = get_base_hsv_color_shifted_quarter(true);
        hsv_t base_hsv_offset          = get_base_hsv_color_shifted(false);
        hsv_t base_hsv_inverse         = get_base_hsv_color_inverse();
        hsv_t base_hsv_inverse_shifted = get_hsv_color_shifted(base_hsv_inverse, HUE_ACCENT, false);

        // recalculate the colors
        ext_lyr_rgb    = hsv_to_rgb(base_hsv_offset);
        accent_lyr_rgb = hsv_to_rgb(base_hsv_inverse_shifted);
        num_lyr_rgb    = hsv_to_rgb(base_hsv_offset_qrt_cw);
        dual_role_rgb  = hsv_to_rgb(base_hsv_inverse);

        recalculate_rgb = false;
    }

    if (IS_LAYER_ON(HRM_BASE_LYR)) {
        // highlight the home row
        const uint8_t accent_key_indexes[7] = {A_KI, S_KI, D_KI, F_KI, J_KI, K_KI, L_KI};
        set_keys_color(ext_lyr_rgb, led_min, led_max, accent_key_indexes, 7);

        // highlight the dual role keys
        const uint8_t dual_role_indexes[9] = {C_KI, R_KI, T_KI, G_KI, SCLN_KI, QUOT_KI, ENTER_KI, COMM_KI, DOT_KI};
        set_keys_color(dual_role_rgb, led_min, led_max, dual_role_indexes, 9);

        set_layer_color_indicator(HRM_BASE_LYR, LEFT_WIN_KI, dual_role_rgb, led_min, led_max);
    }

    if (IS_LAYER_ON(EXT_LYR)) {
        // this layer has many functions, so just change the whole color
        for (int i = led_min; i <= led_max; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(i, ext_lyr_rgb.r, ext_lyr_rgb.g, ext_lyr_rgb.b);
        }

        // this layer has all fn keys so highlight them to show that they are not numbers
        highlight_fn_keys(accent_lyr_rgb, led_min, led_max);

        const uint8_t accent_key_indexes[9] = {// arrow keys
                                               I_KI, J_KI, K_KI, L_KI,
                                               // home row on left hand
                                               F_KI,
                                               // arrows now act as volume control and back and forward buttons
                                               UP_KI, LEFT_KI, DOWN_KI, RIGHT_KI};
        set_keys_color(accent_lyr_rgb, led_min, led_max, accent_key_indexes, 9);

        set_layer_color_indicator(EXT_LYR, LEFT_WIN_KI, ext_lyr_rgb, led_min, led_max);
    }

    // FN Key mode is done after the base win layer and the win fn layer
    // because they can both modify the state of the FN keys
    if (fn_mode_enabled) {
        highlight_fn_keys(accent_lyr_rgb, led_min, led_max);
    }

    if (IS_LAYER_ON(KBCTL_LYR)) {
        // keys specific to this layer
        const uint8_t accent_key_indexes[7] = {P_KI,  // P for persistent color
                                               N_KI,  // N for NKRO
                                               FN_KI, // fn key
                                               // arrows
                                               UP_KI, LEFT_KI, DOWN_KI, RIGHT_KI};
        set_keys_color((rgb_t){RGB_WHITE}, led_min, led_max, accent_key_indexes, 7);

        const uint8_t led_off_indexes[4] = {// turn off some of the LEDS to make it easier to see our indicators
                                            A_KI, TAB_KI, CAPS_KI, LEFT_SFT_KI};
        set_keys_color((rgb_t){RGB_BLACK}, led_min, led_max, led_off_indexes, 4);

        // swap FN key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_CTL_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b);

        // highlight right shift as toggle HRM Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_SFT_KI, dual_role_rgb.r, dual_role_rgb.g, dual_role_rgb.b);

        // highlight Slash as EXT layer toggle
        RGB_MATRIX_INDICATOR_SET_COLOR(SLSH_KI, ext_lyr_rgb.r, ext_lyr_rgb.g, ext_lyr_rgb.b);

        // highlight right alt as toggle Num Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_ALT_KI, num_lyr_rgb.r, num_lyr_rgb.g, num_lyr_rgb.b);

        // highlight Q as reset
        RGB_MATRIX_INDICATOR_SET_COLOR(Q_KI, 0xFF, 0x00, 0x00);

        // highlight Z as clear eeprom
        RGB_MATRIX_INDICATOR_SET_COLOR(Z_KI, 0x55, 0x00, 0x55);

        set_layer_color_indicator(KBCTL_LYR, LEFT_WIN_KI, (rgb_t){RGB_WHITE}, led_min, led_max);
    }

    if (IS_LAYER_ON(NUM_LYR)) {
        // clear out all the leds so we only light up the ones we care about
        // this will make our custom colors stand out more
        for (int i = led_min; i <= led_max; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(i, 0, 0, 0);
        }

        const uint8_t numpad_keys[19] = {
            //  Light up the numpad to make it easier to see
            N6_KI,                                            // 6 = 6 is used as numlock indicator and starts the numpad
            N7_KI, N8_KI,   N9_KI,  N0_KI,   MINS_KI, EQL_KI, // 7, 8, 9, 0 = 7, 8, 9, Asterisk, minus, equals
            U_KI,  I_KI,    O_KI,   P_KI,                     // U, I, O, P = 4, 5, 6, Plus
            J_KI,  K_KI,    L_KI,   SCLN_KI,                  // J, K, L, ; = 1, 2, 3, Enter
            M_KI,  COMM_KI, DOT_KI, SLSH_KI                   // M, ,, ., / = 0, dot, dot, slash
        };
        set_keys_color(num_lyr_rgb, led_min, led_max, numpad_keys, 19);

        // check for num lock
        if (host_keyboard_led_state().num_lock) {
            // we can use the LED Indicator for NUM_LOCK as well
            RGB_MATRIX_INDICATOR_SET_COLOR(N6_KI, 128, 128, 128);
        }

        // highlight the mouse keys
        RGB_MATRIX_INDICATOR_SET_COLOR(W_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // up - W
        RGB_MATRIX_INDICATOR_SET_COLOR(A_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // left - A
        RGB_MATRIX_INDICATOR_SET_COLOR(S_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // down - S
        RGB_MATRIX_INDICATOR_SET_COLOR(D_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // right - D

        // layer toggle keys
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_ALT_KI, 0xAA, 0x22, 0x00);

        set_layer_color_indicator(NUM_LYR, LEFT_WIN_KI, num_lyr_rgb, led_min, led_max);
    }

    if (IS_LAYER_ON(MEDIA_LYR)) {
        const uint8_t media_keys[7] = {// KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU
                                       FN7_KI, FN8_KI, FN9_KI, FN10_KI, FN11_KI, FN12_KI, RIGHT_ALT_KI};
        set_keys_color((rgb_t){RGB_MEDIA_KEYS}, led_min, led_max, media_keys, 7);

        set_layer_color_indicator(MEDIA_LYR, LEFT_WIN_KI, (rgb_t){RGB_MEDIA_KEYS}, led_min, led_max);
    }

    process_indicator_queue(led_min, led_max);

    return true;
}
