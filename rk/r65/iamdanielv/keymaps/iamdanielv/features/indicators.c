#include "quantum.h"
#include "rgb_matrix.h"
#include "action_layer.h"
#include "config.h"

#include "indicators.h"
#include "defines.h"
#include "indicator_queue.h"
#include "fn_mode.h"

/******************
 * RGB Indicators *
 ******************/

rgb_t get_complementary_rgb(rgb_t rgb_led, bool darken) {
    uint8_t new_r = 0xFF - rgb_led.r;
    uint8_t new_g = 0xFF - rgb_led.g;
    uint8_t new_b = 0xFF - rgb_led.b;

    if (darken) {
        // darken the new color by shifting all values down
        if (new_r > 0x80) { new_r = new_r - 0x80;}
        if (new_g > 0x80) { new_g = new_g - 0x80;}
        if (new_b > 0x80) { new_b = new_b - 0x80;}
    }

    return (rgb_t){.r = new_r, .g = new_g, .b = new_b};
}

hsv_t get_hsv_color_shifted(hsv_t color, uint8_t offset, bool clockwise) {
    if (clockwise) {
        if (color.h > offset) {
            color.h = color.h - offset;
        } else {
            color.h = 255 - color.h;
        }
    } else {
        if (color.h < (255 - offset)) {
            color.h = color.h + offset;
        } else {
            color.h = color.h - (255 - offset);
        }
    }
    return color;
}

hsv_t get_base_hsv_color_inverse(void) {
    // get the current base hsv value
    hsv_t base_color = rgb_matrix_get_hsv();

    return get_hsv_color_shifted(base_color, 128, false);
}

hsv_t get_base_hsv_color_shifted(bool clockwise) {
    // get the current base hsv value
    hsv_t base_color = rgb_matrix_get_hsv();

    return get_hsv_color_shifted(base_color, 21, clockwise);
}

hsv_t get_base_hsv_color_shifted_quarter(bool clockwise) {
    // get the current base hsv value
    hsv_t base_color = rgb_matrix_get_hsv();

    // offset hue by a quarter
    return get_hsv_color_shifted(base_color, 64, clockwise);
}


const uint8_t numbers_keys[] = {N1_KI, N2_KI, N3_KI, N4_KI, N5_KI, N6_KI, N7_KI, N8_KI, N9_KI, N0_KI, MINS_KI, EQL_KI};

void blink_numbers(bool isEnabling) {
    for (int num = 0; num < 12; num++) {
        if (isEnabling) {
            // enabling, flash white
            indicator_enqueue(numbers_keys[num], 200, 2, RGB_WHITE);
        } else {
            // disabling, flash red
            indicator_enqueue(numbers_keys[num], 150, 2, RGB_RED);
        }
    }
}

void blink_arrows(void) {
    indicator_enqueue(UP_KI, 200, 2, RGB_WHITE);    // up
    indicator_enqueue(LEFT_KI, 200, 2, RGB_WHITE);  // left
    indicator_enqueue(DOWN_KI, 200, 2, RGB_WHITE);  // down
    indicator_enqueue(RIGHT_KI, 200, 2, RGB_WHITE); // right
}

void blink_space(bool extended) {
    indicator_enqueue(SPACE_KI, 200, 2, RGB_WHITE); // blink space
    if (extended) {
        indicator_enqueue(LEFT_ALT_KI, 200, 2, RGB_BLACK);  // blink left alt
        indicator_enqueue(RIGHT_ALT_KI, 200, 2, RGB_BLACK); // blink right alt
    }
}

void blink_NKRO(bool isEnabling) {
    if (isEnabling) {
        const uint8_t led_indexes[12] = {
            V_KI, B_KI, N_KI, M_KI, COMM_KI, // V B N M ,
            G_KI, H_KI, J_KI, K_KI,          // G H J K
            Y_KI, U_KI, I_KI                 // Y U I
        };

        for (int i = 0; i < 12; i++) {
            indicator_enqueue(led_indexes[i], 200, 3, RGB_WHITE);
        }
    } else {
        const uint8_t led_indexes[4] = {
            B_KI, M_KI, // B M
            H_KI, J_KI  // H J
        };

        for (int i = 0; i < 4; i++) {
            indicator_enqueue(led_indexes[i], 150, 3, RGB_RED);
        }
    }
}

void highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max) {
    for (int num = 0; num < 12; num++) {
        RGB_MATRIX_INDICATOR_SET_COLOR(numbers_keys[num], color.r, color.g, color.b);
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t current_layer = get_highest_layer(layer_state);
    if (current_layer == _WIN_LYR) {
        if (rgb_matrix_get_flags() == LED_FLAG_INDICATOR) {
            for (int i = led_min; i < led_max; i++) {
                rgb_matrix_set_color(i, 0, 0, 0);
            }
            // we could also do this, but using the min max
            // updates a smaller subset at once
            // rgb_matrix_set_color_all(0, 0, 0);
        }
    }

    // determine the colors to use for each of the layers
    hsv_t base_hsv_offset_qrt_ccw = get_base_hsv_color_shifted_quarter(false);
    hsv_t base_hsv_offset_qrt_cw  = get_base_hsv_color_shifted_quarter(true);
    hsv_t base_hsv_offset         = get_base_hsv_color_shifted(false);
    hsv_t base_hsv_inverse        = get_base_hsv_color_inverse();

    rgb_t wfn_lyr_rgb    = hsv_to_rgb(base_hsv_offset);
    rgb_t accent_lyr_rgb = hsv_to_rgb(base_hsv_offset_qrt_cw);
    rgb_t num_lyr_rgb    = hsv_to_rgb(base_hsv_offset_qrt_ccw);
    rgb_t fn_swp_rgb     = hsv_to_rgb(base_hsv_inverse);

    if (IS_LAYER_ON(_WIN_FN_LYR)) {
        // this layer has many functions, so just change the whole color
        for (int i = led_min; i <= led_max; i++) {
            // RGB_MATRIX_INDICATOR_SET_COLOR(i, 0xC0, 0x3D, 0x00);
            RGB_MATRIX_INDICATOR_SET_COLOR(i, wfn_lyr_rgb.r, wfn_lyr_rgb.g, wfn_lyr_rgb.b);
        }

        // highlight the arrow keys
        RGB_MATRIX_INDICATOR_SET_COLOR(I_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // up - I
        RGB_MATRIX_INDICATOR_SET_COLOR(J_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // left - J
        RGB_MATRIX_INDICATOR_SET_COLOR(K_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // down - K
        RGB_MATRIX_INDICATOR_SET_COLOR(L_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // right - L

        // higlight the f key to show home row
        RGB_MATRIX_INDICATOR_SET_COLOR(F_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // f key

        // swap FN key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_CTL_KI, fn_swp_rgb.r, fn_swp_rgb.g, fn_swp_rgb.b);

        // layer lock key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_WIN_KI, 32, 0x00, 0x00); // left GUI/win

        // turn off the left alt key led to make left win stand out more
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_ALT_KI, 0x00, 0x00, 0x00);
    }

    // FN Key mode is done after the base win layer and the win fn layer
    // because they can both modify the state of the FN keys
    if (fn_mode_enabled) {
        highlight_fn_keys(fn_swp_rgb, led_min, led_max);
    }

    if (IS_LAYER_ON(_CTL_LYR)) {
        // keys specific to this layer
        RGB_MATRIX_INDICATOR_SET_COLOR(P_KI, 128, 128, 128);  // P for persistent color
        RGB_MATRIX_INDICATOR_SET_COLOR(N_KI, 128, 128, 128);  // N for NKRO
        RGB_MATRIX_INDICATOR_SET_COLOR(FN_KI, 128, 128, 128); // fn key

        // turn off the left win and alt key leds to make left ctl stand out more
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_WIN_KI, 0x00, 0x00, 0x00);
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_ALT_KI, 0x00, 0x00, 0x00);

        // swap FN key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_CTL_KI, fn_swp_rgb.r, fn_swp_rgb.g, fn_swp_rgb.b);

        // highlight Q as reset
        RGB_MATRIX_INDICATOR_SET_COLOR(Q_KI, 0xFF, 0x00, 0x00);

        // highlight Z as clear eeprom
        RGB_MATRIX_INDICATOR_SET_COLOR(Z_KI, 0x7A, 0x00, 0xFF);

        // highlight right shift as toggle Win Fn Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_SFT_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b);
        // highlight page down as toggle Win Fn Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(PGDN_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b);

        // highlight right alt as toggle Num Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_ALT_KI, num_lyr_rgb.r, num_lyr_rgb.g, num_lyr_rgb.b);
        // highlight page up as toggle Num Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(PGUP_KI, num_lyr_rgb.r, num_lyr_rgb.g, num_lyr_rgb.b);

        // layer lock key
        RGB_MATRIX_INDICATOR_SET_COLOR(HOME_KI, 0x80, 0x00, 0x00); // Home/End key

        // dim the arrow keys
        RGB_MATRIX_INDICATOR_SET_COLOR(UP_KI, 32, 32, 32);
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_KI, 32, 32, 32);
        RGB_MATRIX_INDICATOR_SET_COLOR(DOWN_KI, 32, 32, 32);
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_KI, 32, 32, 32);
    }

    if (IS_LAYER_ON(_NUM_LYR)) {
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
        for (int i = 0; i < 19; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(numpad_keys[i], num_lyr_rgb.r, num_lyr_rgb.g, num_lyr_rgb.b);
        }

        // highlight the mouse keys
        RGB_MATRIX_INDICATOR_SET_COLOR(W_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // up - W
        RGB_MATRIX_INDICATOR_SET_COLOR(A_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // left - A
        RGB_MATRIX_INDICATOR_SET_COLOR(S_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // down - S
        RGB_MATRIX_INDICATOR_SET_COLOR(D_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // right - D

        // layer toggle keys
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_ALT_KI, 32, 0x00, 0x00);
        RGB_MATRIX_INDICATOR_SET_COLOR(PGUP_KI, 32, 0x00, 0x00);
    }

    if (IS_LAYER_ON(_FN_LYR)) {
        const uint8_t media_keys[6] = {
            // KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU
            FN7_KI, FN8_KI, FN9_KI, FN10_KI, FN11_KI, FN12_KI};

        for (int i = 0; i < 6; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(media_keys[i], 128, 128, 128);
        }

        // highlight the RALT button
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_ALT_KI, 128, 128, 128);

        // highlight page down as toggle Win Fn Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(PGDN_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b);

        // highlight page up as toggle Num Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(PGUP_KI, num_lyr_rgb.r, num_lyr_rgb.g, num_lyr_rgb.b);

        // layer lock key
        RGB_MATRIX_INDICATOR_SET_COLOR(HOME_KI, 0x80, 0x00, 0x00); // Home/End key
    }

    process_indicator_queue(led_min, led_max);

    return true;
}
