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

hsv_t get_hsv_color_shifted(hsv_t color, uint8_t offset,  bool clockwise){
    if(clockwise){
        if(color.h > offset){
            color.h = color.h - offset;
        } else {
            color.h = 255 - color.h;
        }
    } else {
        if(color.h < (255-offset)){
            color.h = color.h + offset;
        } else {
            color.h = color.h - (255-offset);
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

    //offset hue by a quarter
    return get_hsv_color_shifted(base_color, 64, clockwise);
}

void blink_numbers(bool isEnabling) {
    for (int i = 55; i >= 44; i--) // 1(55) to EQL(44)
    {
        if (isEnabling) {
            // enabling, flash white
            indicator_enqueue(i, 200, 2, RGB_WHITE);
        } else {
            // disabling, flash red
            indicator_enqueue(i, 150, 2, RGB_RED);
        }
    }
}

void blink_arrows(void) {
    indicator_enqueue(UP_KI, 200, 2, RGB_WHITE); // up
    indicator_enqueue(LEFT_KI, 200, 2, RGB_WHITE); // left
    indicator_enqueue(DOWN_KI, 200, 2, RGB_WHITE); // down
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

void highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max) {
    for (int i = 55; i >= 44; i--) { // 55 - 44 are the number keys and - =
        RGB_MATRIX_INDICATOR_SET_COLOR(i, color.r, color.g, color.b);
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
    hsv_t base_hsv_offset_qrt_cw = get_base_hsv_color_shifted_quarter(true);
    hsv_t base_hsv_offset = get_base_hsv_color_shifted(false);
    hsv_t base_hsv_inverse = get_base_hsv_color_inverse();

    rgb_t wfn_lyr_rgb = hsv_to_rgb(base_hsv_offset);
    rgb_t accent_lyr_rgb = hsv_to_rgb(base_hsv_offset_qrt_cw);
    rgb_t num_lyr_rgb = hsv_to_rgb(base_hsv_offset_qrt_ccw);
    rgb_t fn_swp_rgb = hsv_to_rgb(base_hsv_inverse);

    if (IS_LAYER_ON(_WIN_FN_LYR)) {
        // this layer has many functions, so just change the whole color
        for (int i = led_min; i <= led_max; i++) {
            // RGB_MATRIX_INDICATOR_SET_COLOR(i, 0xC0, 0x3D, 0x00);
            RGB_MATRIX_INDICATOR_SET_COLOR(i, wfn_lyr_rgb.r, wfn_lyr_rgb.g, wfn_lyr_rgb.b);
        }

        //highlight the arrow keys
        RGB_MATRIX_INDICATOR_SET_COLOR(I_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // up - I
        RGB_MATRIX_INDICATOR_SET_COLOR(J_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // left - J
        RGB_MATRIX_INDICATOR_SET_COLOR(K_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // down - K
        RGB_MATRIX_INDICATOR_SET_COLOR(L_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // right - L

        // higlight the f key to show home row
        RGB_MATRIX_INDICATOR_SET_COLOR(F_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // f key

        // swap FN key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_CTL_KI, fn_swp_rgb.r, fn_swp_rgb.g, fn_swp_rgb.b);

        // layer lock key
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_WIN_KI, 32,0x00, 0x00); // left GUI/win

        // turn off the left key led to make left win stand out more
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_ALT_KI, 0x00,0x00, 0x00);
    }

    // FN Key mode is done after the base win layer and the win fn layer
    // because they can both modify the state of the FN keys
    if (fn_mode_enabled) {
        highlight_fn_keys(fn_swp_rgb, led_min, led_max);
    }

    if (IS_LAYER_ON(_CTL_LYR)) {
        const uint8_t led_indexes[8] = {
            LEFT_CTL_KI, // lctl for Fn toggle
            FN_KI, // fn key
            39, // P for persistent color
            9  // N for NKRO
        };
        for (int i = 0; i < 8; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i],  128, 128, 128);
        }

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
        RGB_MATRIX_INDICATOR_SET_COLOR(HOME_KI,0x80, 0x00, 0x00); // Home/End key

        // turn off the left win and alt key leds to make left ctl stand out more
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_WIN_KI, 0x00,0x00, 0x00);
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_ALT_KI, 0x00,0x00, 0x00);

        // dim the arrow keys
        RGB_MATRIX_INDICATOR_SET_COLOR(UP_KI, 64,64,64);
        RGB_MATRIX_INDICATOR_SET_COLOR(LEFT_KI,64,64,64);
        RGB_MATRIX_INDICATOR_SET_COLOR(DOWN_KI, 64,64,64);
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_KI, 64,64,64);
    }

    if (IS_LAYER_ON(_NUM_LYR)) {
        // clear out all the leds so we only light up the ones we care about
        // this will make our custom colors stand out more
        for (int i = led_min; i <= led_max; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(i, 0, 0, 0);
        }

        const uint8_t num_led_indexes[19] = {
            // Light up the numpad to make it easier to see
            // 6 is used as numlock and starts the numpad
            50, 49, 48, 47, 46, 45, 44, // 6, 7, 8, 9, Asterisk, minus, equals
            36, 37, 38, 39,             // U, I, O, P = 4, 5, 6, Plus
            21, 20, 19, 18,             // J, K, L, ; = 1, 2, 3, Enter
            10, 11, 12, 13              // M, ,, ., / = 0, dot, dot, slash
        };
        for (int i = 0; i < 19; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(num_led_indexes[i], num_lyr_rgb.r, num_lyr_rgb.g, num_lyr_rgb.b);
            // RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0, 255, 0);
        }

        //highlight the mouse keys
        RGB_MATRIX_INDICATOR_SET_COLOR(W_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // up - W
        RGB_MATRIX_INDICATOR_SET_COLOR(A_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // left - A
        RGB_MATRIX_INDICATOR_SET_COLOR(S_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // down - S
        RGB_MATRIX_INDICATOR_SET_COLOR(D_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b); // right - D

        // layer toggle keys
        RGB_MATRIX_INDICATOR_SET_COLOR(RIGHT_ALT_KI, 32,0x00, 0x00);
        RGB_MATRIX_INDICATOR_SET_COLOR(PGUP_KI, 32,0x00, 0x00);
    }

    if (IS_LAYER_ON(_FN_LYR)) {

        // highlight the aux buttons on right of keyboard
        const uint8_t led_indexes[6] = {

            49, 48, 47, 46, 45, 44 // used for media keys = 6 keys
        };

        for (int i = 0; i < 6; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 128, 128, 128);
        }

        // highlight page down as toggle Win Fn Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(PGDN_KI, accent_lyr_rgb.r, accent_lyr_rgb.g, accent_lyr_rgb.b);

        // highlight page up as toggle Num Layer
        RGB_MATRIX_INDICATOR_SET_COLOR(PGUP_KI, num_lyr_rgb.r, num_lyr_rgb.g, num_lyr_rgb.b);

        // layer lock key
        RGB_MATRIX_INDICATOR_SET_COLOR(HOME_KI,0x80, 0x00, 0x00); // Home/End key
    }

    process_indicator_queue(led_min, led_max);

    return true;
}
