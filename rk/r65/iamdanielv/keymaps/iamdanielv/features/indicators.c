#include "color.h"
#include "quantum.h"
#include "rgb_matrix.h"
#include "action_layer.h"
#include "config.h"

#include "indicators.h"
#include "defines.h"
#include "indicator_queue.h"
#include "fn_mode.h"

// clang-format off
/*  LED Matrix
    ESC  1    2    3    4    5    6    7    8    9    0    -    =    BSPC MUTE
    56   55   54   53   52   51   50   49   48   47   46   45   44   43

    TAB  Q    W    E    R    T    Y    U    I    O    P    [    ]    BSLS HOME
    29   30   31   32   33   34   35   36   37   38   39   40   41   42   57

    CAPS A    S    D    F    G    H    J    K    L    ;    '         ENT  PGUP
    28   27   26   25   24   23   22   21   20   19   18   17        16   58

    LSFT Z    X    C    V    B    N    M    ,    .    /    RSFT      UP   PGDN
    3    4    5    6    7    8    9    10   11   12   13   14        15   59

    LCTL LG   LALT           SPC            RALT Fn       LEFT      DOWN RGHT
    2    1    0              65             64   63        62        61   60
*/
// clang-format on

/******************
 * RGB Indicators *
 ******************/

 rgb_t get_complementary_color(rgb_t rgb_led, bool darken) {
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

hsv_t get_base_hsv_color_shifted(bool clockwise) {
    // get the current base hsv value
    hsv_t base_color = rgb_matrix_get_hsv();
    //base_color.v = 255;
    if(clockwise){
        if(base_color.h > 21){
            base_color.h = base_color.h - 21;
        } else {
            base_color.h = 255 - base_color.h;
        }
    } else {
        if(base_color.h < 234){
            base_color.h = base_color.h + 21;
        } else {
            base_color.h = base_color.h - 234;
        }
    }
    return base_color;
}

hsv_t get_base_hsv_color_shifted_quarter(bool clockwise) {
    // get the current base hsv value
    hsv_t base_color = rgb_matrix_get_hsv();

    // maximize brightness
    //base_color.v = 255;
    //offset hue by a quarter
    if(clockwise){
        if(base_color.h > 64){
            base_color.h = base_color.h - 64;
        } else {
            base_color.h = 255 - base_color.h;
        }
    } else {
        if(base_color.h < 191){
            base_color.h = base_color.h + 64;
        } else {
            base_color.h = base_color.h - 191;
        }
    }
    return base_color;
}

void blink_numbers(bool isEnabling) {
    for (int i = 55; i >= 44; i--) // 1(55) to EQL(44)
    {
        if (isEnabling) {
            // enabling, flash white
            indicator_enqueue(i, 200, 3, RGB_WHITE);
        } else {
            // disabling, flash red
            indicator_enqueue(i, 150, 4, RGB_RED);
        }
    }
}

void blink_arrows(void) {
    indicator_enqueue(62, 200, 3, RGB_WHITE); // left
    indicator_enqueue(61, 200, 3, RGB_WHITE); // down
    indicator_enqueue(15, 200, 3, RGB_WHITE); // up
    indicator_enqueue(60, 200, 3, RGB_WHITE); // right
}

void blink_space(bool extended) {
    indicator_enqueue(65, 200, 3, RGB_WHITE); // blink space too
    if (extended) {
        indicator_enqueue(0, 200, 3, RGB_BLACK);  // blink left alt
        indicator_enqueue(64, 200, 3, RGB_BLACK); // blink right alt
    }
}

void blink_NKRO(bool isEnabling) {
    if (isEnabling) {
        const uint8_t led_indexes[12] = {
            7,  8,  9,  10, 11, // V B N M ,
            20, 21, 22, 23,     // K J H G
            35, 36, 37          // Y U I
        };

        for (int i = 0; i < 12; i++) {
            indicator_enqueue(led_indexes[i], 200, 3, RGB_WHITE);
        }
    } else {
        const uint8_t led_indexes[4] = {
            8, 10, // B M
            21, 22 // H J
        };

        for (int i = 0; i < 4; i++) {
            indicator_enqueue(led_indexes[i], 150, 3, RGB_RED);
        }
    }
}

void highlight_fn_keys(uint8_t led_min, uint8_t led_max) {
    // get the current hsv value
    HSV current_hsv = rgb_matrix_get_hsv();
    // maximize brightness
    current_hsv.v = 255;

    rgb_led_t rgb     = hsv_to_rgb(current_hsv);
    rgb_led_t new_rgb = get_complementary_color(rgb, false);
    for (int i = 55; i >= 44; i--) { // 55 - 44 are the number keys and - =
        RGB_MATRIX_INDICATOR_SET_COLOR(i, new_rgb.r, new_rgb.g, new_rgb.b);
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

    if (IS_LAYER_ON(_WIN_FN_LYR) ||
        // IS_LAYER_ON(_CTL_LYR) ||  //ignore the CTL layer since we want to see RGB effects on that layer
        IS_LAYER_ON(_NUM_LYR) ||
        IS_LAYER_ON(_NAV_LYR) ||
        IS_LAYER_ON(_FN_LYR)) {
        // we are in a custom layer, clear all background colors
        // this will make our custom colors stand out more
        for (int i = led_min; i <= led_max; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(i, 0, 0, 0);
        }
    }

    if (fn_mode_enabled) {
        highlight_fn_keys(led_min, led_max);
    }

     // determine the colors to use for each of the layers
    hsv_t base_color_offset_qrt_ccw = get_base_hsv_color_shifted_quarter(false);
    hsv_t base_color_offset_qrt_cw = get_base_hsv_color_shifted_quarter(true);
    hsv_t base_color_offset = get_base_hsv_color_shifted(false);
    // make the color a little darker
    // if(base_color_offset.v > 128)
    //     base_color_offset.v = base_color_offset.v - 64;

    rgb_t wfn_lyr_color = hsv_to_rgb(base_color_offset);
    rgb_t accent_lyr_color = hsv_to_rgb(base_color_offset_qrt_cw);
    rgb_t num_lyr_color = hsv_to_rgb(base_color_offset_qrt_ccw);

    if (IS_LAYER_ON(_WIN_FN_LYR)) {
        // this layer has many functions, so just change the whole color
        for (int i = led_min; i <= led_max; i++) {
            // RGB_MATRIX_INDICATOR_SET_COLOR(i, 0xC0, 0x3D, 0x00);
            RGB_MATRIX_INDICATOR_SET_COLOR(i, wfn_lyr_color.r, wfn_lyr_color.g, wfn_lyr_color.b);
        }

        // no matter what, this layer also uses fn keys
        highlight_fn_keys(led_min, led_max);

        //highlight the arrow keys
        RGB_MATRIX_INDICATOR_SET_COLOR(I_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // up - I
        RGB_MATRIX_INDICATOR_SET_COLOR(J_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // left - J
        RGB_MATRIX_INDICATOR_SET_COLOR(K_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // down - K
        RGB_MATRIX_INDICATOR_SET_COLOR(L_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // right - L

        // higlight the f key to show home row
        RGB_MATRIX_INDICATOR_SET_COLOR(F_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // f key

        // layer lock key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_WIN_KI, 0xFF,0x00, 0x00); // left GUI/win
    }

    if (IS_LAYER_ON(_CTL_LYR)) {
        const uint8_t led_indexes[3] = {
            LEFT_CTL_KI, // lctl for Fn toggle
            39, // P for persistent color
            9   // N for NKRO
        };
        for (int i = 0; i < 3; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0x00, 0x80, 0x80);
        }

        // turn off some of the LEDS to make it easier to see our indicators
        const uint8_t led_off_indexes[15] = {
            A_KI, HOME_KI, PGUP_KI,  PGDN_KI,   // A, Home, PgUp, PgDN
            UP_KI, LEFT_KI, DOWN_KI, RIGHT_KI, // Arrow keys
            TAB_KI, CAPS_KI, LEFT_SFT_KI,  // TAB, CAPS, LSFT
            LEFT_ALT_KI,  SPACE_KI, RIGHT_ALT_KI, FN_KI  // LALT, SPC, RALT, Fn
        };
        for (int i = 0; i < 15; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_off_indexes[i], 0x00, 0x00, 0x00);
        }

        // highlight Q as reset
        RGB_MATRIX_INDICATOR_SET_COLOR(Q_KI, 0xFF, 0x00, 0x00);

        // highlight Z as clear eeprom
        RGB_MATRIX_INDICATOR_SET_COLOR(Z_KI, 0x7A, 0x00, 0xFF);

        // highlight right shift as toggle Win Fn Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_SFT_KI, wfn_lyr_color.r, wfn_lyr_color.g, wfn_lyr_color.b);

        // highlight right alt as toggle Num Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_SFT_KI, num_lyr_color.r, num_lyr_color.g, num_lyr_color.b);

        // layer lock key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_WIN_KI, 0xFF,0x00, 0x00); // left GUI/win
    }

    if (IS_LAYER_ON(_NUM_LYR)) {
        const uint8_t num_led_indexes[19] = {
            // Light up the numpad to make it easier to see
            // 6 is used as numlock and starts the numpad
            50, 49, 48, 47, 46, 45, 44, // 6, 7, 8, 9, Asterisk, minus, equals
            36, 37, 38, 39,             // U, I, O, P = 4, 5, 6, Plus
            21, 20, 19, 18,             // J, K, L, ; = 1, 2, 3, Enter
            10, 11, 12, 13              // M, ,, ., / = 0, dot, dot, slash
        };

        for (int i = 0; i < 19; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(num_led_indexes[i], num_lyr_color.r, num_lyr_color.g, num_lyr_color.b);
            // RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0, 255, 0);
        }

        //highlight the mouse keys
        RGB_MATRIX_INDICATOR_SET_COLOR(W_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // up - W
        RGB_MATRIX_INDICATOR_SET_COLOR(A_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // left - A
        RGB_MATRIX_INDICATOR_SET_COLOR(S_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // down - S
        RGB_MATRIX_INDICATOR_SET_COLOR(D_KI, accent_lyr_color.r, accent_lyr_color.g, accent_lyr_color.b); // right - D

        // layer toggle key
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_ALT_KI, 0xFF,0x00, 0x00); // use Right alt as indicator
    }

    if (IS_LAYER_ON(_NAV_LYR)) {
        // this layer has many functions, so just change the whole color
        for (int i = led_min; i <= led_max; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(i, 0x00, 0x00, 0xFF);
        }

        RGB_MATRIX_INDICATOR_SET_COLOR(57, 0xFF, 0xFF, 0xFF); // use home key as toggle indicator
    }

    if (IS_LAYER_ON(_FN_LYR)) {
        // highlight the toggle buttons
        RGB_MATRIX_INDICATOR_SET_COLOR(HOME_KI, 0, 0, 255);   // NAV = Home
        RGB_MATRIX_INDICATOR_SET_COLOR(PGUP_KI, 0, 255, 0);   // NUM = PgUp
        RGB_MATRIX_INDICATOR_SET_COLOR(PGDN_KI, 0, 255, 255); // CTL = PgDn

        // highlight right shift as moving to ctl layer
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_SFT_KI, 0, 255, 255);

        // highlight the aux buttons on right of keyboard
        const uint8_t led_indexes[7] = {
            RIGHT_ALT_KI,                    // highlight the RALT button
            49, 48, 47, 46, 45, 44 // used for media keys = 6 keys
        };

        for (int i = 0; i < 7; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 128, 128, 128);
        }
    }

    process_indicator_queue(led_min, led_max);

    return true;
}
