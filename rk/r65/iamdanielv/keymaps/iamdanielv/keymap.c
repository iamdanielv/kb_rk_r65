// Copyright 2024 iamdanielv (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "features/indicator_queue.h"
#include "features/fn_mode.h"

enum layer_names {
    _WIN_LYR,     // 0
    _WIN_FN_LYR,  // 1
    _CTL_LYR,     // 2
    _NUM_LYR,     // 3
    _NAV_LYR,     // 4
    _FN_LYR,      // 5
};

void blink_arrows(void);
void blink_NKRO(bool);
void blink_numbers(bool);
void highlight_fn_keys(uint8_t led_min, uint8_t led_max);

// ***********************
// * Keyboard Management *
// ***********************
void housekeeping_task_user(void) {
    // Note: We can decide what to do with the MAC Led in this function
    if (IS_LAYER_ON(_CTL_LYR)) {         // if the Ctl layer is active
        gpio_write_pin_low(LED_MAC_PIN); // low means turn on
    } else {
        gpio_write_pin_high(LED_MAC_PIN); // high means turn off
    }

    if (!keymap_config.no_gui) {
        // we have NOT enabled the no_gui,
        // we can re-use the Win Lock LED as NumLock indicator
        if (IS_LAYER_ON(_NUM_LYR)) { // if the Num layer is active
            // get the current LED state
            led_t led_state = host_keyboard_led_state();
            if (led_state.num_lock) {
                // Num/Win lock should be on
                gpio_write_pin_low(LED_WIN_LOCK_PIN);
            } else {
                // Num/Win lock should be off
                gpio_write_pin_high(LED_WIN_LOCK_PIN);
            }
        } // else we are not on the num layer, ignore
    } // else we have enabled no_gui, skip re-using the LED
}

// from https://thomasbaart.nl/2018/12/13/qmk-basics-tap-dance/
void safe_reset(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 3) {
        // Reset the keyboard if you tap the key more than three times
        reset_keyboard();
        reset_tap_dance(state);
    }
}

void safe_clear(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 3) {
        // clear eprom if you tap the key more than three times
        eeconfig_init();
        soft_reset_keyboard();
        reset_tap_dance(state);
    }
}

// *************
// * Tap Dance *
// *************
enum tap_dance_keys {
    TD_RESET, // require 3 taps to reset board
    TD_CLEAR, // require 3 taps to clear eeprom
    TD_CTL_TG // require double tap to enable ctl layer
};

//#define TAP_HOLD_ENABLE
#ifdef TAP_HOLD_ENABLE
typedef struct {
    uint16_t on_tap;  // key to send on tap
    uint16_t on_hold; // key to send on hold
    uint16_t chosen;  // key to "release" when finished
                      // the value for it is set in td_tap_hold_finished
} td_tap_hold_t;

void td_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    td_tap_hold_t *tap_hold = (td_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            // "press" the key specified on_hold
            register_code16(tap_hold->on_hold);
            // keep track of which key was used
            tap_hold->chosen = tap_hold->on_hold;
        } else {
            // "press" the key specified on_tap
            register_code16(tap_hold->on_tap);
            // keep track of which key was used
            tap_hold->chosen = tap_hold->on_tap;
        }
    }
}

void td_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    td_tap_hold_t *tap_hold = (td_tap_hold_t *)user_data;

    if (tap_hold->chosen) {
        // there is something to cleanup
        // release the key that was "pressed"
        unregister_code16(tap_hold->chosen);

        // reset the key to 0 to signify nothing is selected
        tap_hold->chosen = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(on_tap, on_hold) \
    { .fn = {NULL, td_tap_hold_finished, td_tap_hold_reset}, .user_data = (void *)&((td_tap_hold_t){on_tap, on_hold, 0}), }

#endif

// *****************************
// * Custom processing of keys *
// *****************************
enum custom_keycodes {
    KC_SWP_FN = SAFE_RANGE
};

// clang-format off
tap_dance_action_t tap_dance_actions[] = {
    [TD_RESET]  = ACTION_TAP_DANCE_FN(safe_reset),
    [TD_CLEAR]  = ACTION_TAP_DANCE_FN(safe_clear),
    [TD_CTL_TG] = ACTION_TAP_DANCE_LAYER_TOGGLE(KC_RCTL, _CTL_LYR)
};
// clang-format on

bool fn_mode_enabled = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    if (keycode == KC_SWP_FN) {
        if (record->event.pressed) {
            fn_mode_enabled = !fn_mode_enabled;
            blink_numbers(fn_mode_enabled);
            indicator_enqueue(65, 200, 3, RGB_WHITE );  // blink space too
            indicator_enqueue(0, 200, 3, RGB_BLACK );   // blink left alt
            indicator_enqueue(64, 200, 3, RGB_BLACK );  // blink right alt
        }
        return false;
    }

    if (!process_fn_mode(keycode, record)) { return false; }

    switch (keycode) {
        case QK_MAGIC_TOGGLE_NKRO:
            if (record->event.pressed) {
                clear_keyboard(); // clear first buffer to prevent stuck keys
                wait_ms(50);
                keymap_config.nkro = !keymap_config.nkro;
                blink_NKRO(keymap_config.nkro);
                wait_ms(50);
                clear_keyboard(); // clear first buffer to prevent stuck keys
                wait_ms(50);
            }
            return false;
        case RGB_TOG:
            if (record->event.pressed) {
                switch (rgb_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        rgb_matrix_set_flags_noeeprom(LED_FLAG_INDICATOR);
                        //rgb_matrix_set_color_all(0, 0, 0);
                        indicator_enqueue(65, 200, 3, INDICATOR_RGB_DARK_RED );  // blink space too
                        indicator_enqueue(0, 200, 3, INDICATOR_RGB_DARK_RED );   // blink left alt
                        indicator_enqueue(64, 200, 3, INDICATOR_RGB_DARK_RED );  // blink right alt
                    } break;
                    default: {
                        HSV current_hsv = rgb_matrix_get_hsv();
                        RGB rgb = hsv_to_rgb(current_hsv);
                        rgb_matrix_set_color_all(rgb.r, rgb.g, rgb.b);
                        rgb_matrix_set_flags_noeeprom(LED_FLAG_ALL);
                        indicator_enqueue(65, 200, 3, RGB_WHITE );  // blink space too
                        indicator_enqueue(0, 200,  3, RGB_WHITE );  // blink left alt
                        indicator_enqueue(64, 200, 3, RGB_WHITE );  // blink right alt
                    } break;
                }
            }
            return false;
        case RGB_MOD:
            if (record->event.pressed) {
                rgb_matrix_step_noeeprom();
            }
            return false;
        case RGB_RMOD:
            if (record->event.pressed) {
                rgb_matrix_step_reverse_noeeprom();
            }
            return false;
        case RGB_M_P:
            if (record->event.pressed) {
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
                indicator_enqueue(65, 200, 3, RGB_WHITE );  // blink space too
                indicator_enqueue(0, 200, 3, RGB_WHITE );   // blink left alt
                indicator_enqueue(64, 200, 3, RGB_WHITE );  // blink right alt
            }
            return false;
        case RGB_SPI:
            if (record->event.pressed) {
                if ( rgb_matrix_get_speed() >= (255 - RGB_MATRIX_SPD_STEP)) {
                    // this update would put us at max
                    blink_arrows();
                    indicator_enqueue(17, 200, 4, RGB_RED );    // ' - SPI
                    indicator_enqueue(18, 200, 2, RGB_BLACK );  // ; - SPD
                    indicator_enqueue(65, 200, 3, RGB_WHITE );  // blink space too
                }
                rgb_matrix_increase_speed_noeeprom();
            }
            return false;
        case RGB_SPD:
            if (record->event.pressed) {
                if (rgb_matrix_get_speed() <= RGB_MATRIX_SPD_STEP) {
                    blink_arrows();
                    indicator_enqueue(17, 200, 2, RGB_BLACK ); // ' - SPI
                    indicator_enqueue(18, 200, 4, RGB_RED );   // ; - SPD
                    indicator_enqueue(65, 200, 3, RGB_WHITE ); // blink space too
                    rgb_matrix_set_speed_noeeprom(RGB_MATRIX_SPD_STEP);
                }
                rgb_matrix_decrease_speed_noeeprom();
            }
            return false;
        case RGB_HUI:
            if (record->event.pressed) {
                if ( rgb_matrix_get_hue() >= (255 - RGB_MATRIX_HUE_STEP)) {
                    // this update would put us at max
                    indicator_enqueue(38, 200, 4, INDICATOR_RGB_DARK_RED ); // O - HUI
                    indicator_enqueue(37, 200, 2, RGB_BLACK );              // I - HUD
                    indicator_enqueue(65, 200, 3, RGB_WHITE );              // blink space too
                }
                rgb_matrix_increase_hue_noeeprom();
            }
            return false;
        case RGB_HUD:
            if (record->event.pressed) {
                if ( rgb_matrix_get_hue() <= RGB_MATRIX_HUE_STEP ) {
                    // this update would put us at min
                    indicator_enqueue(38, 200, 2, RGB_BLACK );              // O - HUI
                    indicator_enqueue(37, 200, 4, INDICATOR_RGB_DARK_RED ); // I - HUD
                    indicator_enqueue(65, 200, 3, RGB_WHITE );              // blink space too
                }
                rgb_matrix_decrease_hue_noeeprom();
            }
            return false;
        case RGB_SAI:
            if (record->event.pressed) {
                if ( rgb_matrix_get_sat() >= (255 - RGB_MATRIX_SAT_STEP)) {
                    // this update would put us at max
                    indicator_enqueue(19, 200, 4, INDICATOR_RGB_DARK_RED ); // L - SAI
                    indicator_enqueue(20, 200, 2, RGB_BLACK );              // K - SAD
                    indicator_enqueue(65, 200, 3, RGB_WHITE );              // blink space too
                }
                rgb_matrix_increase_sat_noeeprom();
            }
            return false;
        case RGB_SAD:
            if (record->event.pressed) {
                if ( rgb_matrix_get_sat() <= RGB_MATRIX_SAT_STEP ) {
                    // this update would put us at min
                    indicator_enqueue(19, 200, 2, RGB_BLACK );              // L - SAI
                    indicator_enqueue(20, 200, 4, INDICATOR_RGB_DARK_RED ); // K - SAD
                    indicator_enqueue(65, 200, 3, RGB_WHITE );              // blink space too
                }
                rgb_matrix_decrease_sat_noeeprom();
            }
            return false;
        case RGB_VAI:
            if (record->event.pressed) {
                if (rgb_matrix_get_val() >= (RGB_MATRIX_MAXIMUM_BRIGHTNESS - RGB_MATRIX_VAL_STEP)) {
                    indicator_enqueue(12, 200, 4, RGB_RED );    // . - VAI
                    indicator_enqueue(11, 200, 2, RGB_BLACK );  // , - VAD
                    indicator_enqueue(65, 200, 3, RGB_WHITE );  // blink space too
                    blink_arrows();
                }
                rgb_matrix_increase_val_noeeprom();
            }
            return false;
        case RGB_VAD:
            if (record->event.pressed) {
                if (rgb_matrix_get_val() <= RGB_MATRIX_VAL_STEP) {
                    indicator_enqueue(12, 200, 2, RGB_BLACK );  // . - VAI
                    indicator_enqueue(11, 200, 4, RGB_RED );    // , - VAD
                    indicator_enqueue(65, 200, 3, RGB_WHITE );  // blink space too
                    blink_arrows();
                }
                rgb_matrix_decrease_val_noeeprom();
            }
            return false;
        default:
            return true;
    }

    return true;
}


// ******************************
// * Aliases to simplify keymap *
// ******************************
#define W_ALT_CAPS LT(_WIN_FN_LYR, KC_CAPS)
#define FN_RALT LT(_FN_LYR, KC_RALT)

#define MY_UNDO   C(KC_Z)
#define MY_CUT    C(KC_X)
#define MY_COPY   C(KC_INS)
#define MY_PASTE  S(KC_INS)

#define TG_NAV    TG(_NAV_LYR)
#define TG_NUM    TG(_NUM_LYR)
#define TG_CTL    TG(_CTL_LYR)
#define MO_CTL    MO(_CTL_LYR)
#define MO_FN     MO(_WIN_FN_LYR)
#define TD_TG_CTL TD(TD_CTL_TG)
#define TD_KB_RST TD(TD_RESET)
#define TD_KB_CLR TD(TD_CLEAR)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_WIN_LYR] = LAYOUT( // 0
        KC_ESC,    KC_1,      KC_2,      KC_3,      KC_4,      KC_5,     KC_6,     KC_7,     KC_8,      KC_9,     KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,   KC_MUTE,
        KC_TAB,    KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,     KC_Y,     KC_U,     KC_I,      KC_O,     KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,   KC_HOME,
        W_ALT_CAPS,KC_A,      KC_S,      KC_D,      KC_F,      KC_G,     KC_H,     KC_J,     KC_K,      KC_L,     KC_SCLN,    KC_QUOT,            KC_ENT,    KC_PGUP,
        KC_LSFT,   KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,   KC_DOT,   KC_SLSH,    KC_RSFT,            KC_UP,     KC_PGDN,
        KC_LCTL,   KC_LGUI,   KC_LALT,                         KC_SPC,                       FN_RALT,   MO_FN,                KC_LEFT,            KC_DOWN,   KC_RGHT
    ),
    [_WIN_FN_LYR] = LAYOUT( // 1
        KC_GRV,    KC_F1,     KC_F2,     KC_F3,     KC_F4,     KC_F5,    KC_F6,    KC_F7,    KC_F8,     KC_F9,    KC_F10,     KC_F11,   KC_F12,   KC_DEL,    _______,
        KC_GRV,    KC_HOME,   KC_UP,     KC_END,    C(KC_R),   KC_PGUP,  _______,  _______,  _______,   _______,  KC_PSCR,    KC_SCRL,  KC_PAUS,  KC_INS,    KC_END,
        _______,   KC_LEFT,   KC_DOWN,   KC_RIGHT,  C(KC_F),   KC_PGDN,  _______,  _______,  _______,   _______,  KC_HOME,    KC_END,             _______,   KC_SCRL,
        _______,   MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   _______,  _______,  KC_PGDN,   KC_PGUP,  _______,    MO_CTL,             _______,   KC_RCTL,
        _______,   _______,   _______,                         _______,                      KC_RCTL,   _______,              _______,            _______,   _______
    ),
    [_CTL_LYR] = LAYOUT( // 2
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,   _______,  _______,    _______,  _______,  _______,   _______,
        _______,   TD_KB_RST, _______,   _______,   _______,   _______,  _______,  _______,  RGB_HUD,   RGB_HUI,  RGB_M_P,    RGB_RMOD, RGB_MOD,  RGB_TOG,   _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  RGB_SAD,   RGB_SAI,  RGB_SPD,    RGB_SPI,            _______,   _______,
        _______,   TD_KB_CLR, _______,   _______,   _______,   _______,  NK_TOGG,  _______,  RGB_VAD,   RGB_VAI,  _______,    _______,            RGB_VAI,   TG_CTL,
        KC_SWP_FN, _______,   _______,                         _______,                      _______,   _______,              RGB_SPD,            RGB_VAD,   RGB_SPI
    ),
    [_NUM_LYR] = LAYOUT( // 3
        _______,   _______,   _______,   _______,   _______,   _______,  KC_NUM,   KC_P7,    KC_P8,     KC_P9,   KC_PAST,    _______,  _______,  _______,   _______,
        _______,   KC_BTN1,   KC_MS_U,   KC_BTN2,   _______,   _______,  XXXXXXX,  KC_P4,    KC_P5,     KC_P6,   KC_PPLS,    _______,  _______,  _______,   _______,
        _______,   KC_MS_L,   KC_MS_D,   KC_MS_R,   _______,   _______,  XXXXXXX,  KC_P1,    KC_P2,     KC_P3,   KC_PENT,    _______,            _______,   TG_NUM,
        _______,   _______,   _______,   _______,   _______,   _______,  XXXXXXX,  KC_P0,    KC_PDOT,   KC_PDOT, KC_PSLS,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      _______,   _______,             _______,            _______,   _______
    ),
    [_NAV_LYR] = LAYOUT( // 4
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   _______,
        _______,   KC_HOME,   KC_UP,     KC_END,    C(KC_R),   KC_PGUP,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   TG_NAV,
        _______,   KC_LEFT,   KC_DOWN,   KC_RIGHT,  C(KC_F),   KC_PGDN,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,   MY_UNDO,   MY_CUT,    MY_COPY,   MY_PASTE,  KC_SPC,   _______,  _______,  _______,  _______,  _______,    _______,            _______,   _______,
        _______,   _______,   _______,                         _______,                      _______,  _______,              _______,            _______,   _______
    ),
    [_FN_LYR] = LAYOUT( // 5
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,    KC_VOLD,  KC_VOLU,  KC_DEL,    _______,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,   TG_NAV,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,            _______,   TG_NUM,
        _______,   _______,   _______,   _______,   _______,   _______,  _______,  _______,  _______,  _______,  _______,    MO_CTL,             _______,   TD_TG_CTL,
        _______,   _______,   _______,                         _______,                      KC_RCTL,  _______,              _______,            _______,   _______
    )
};


#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_WIN_LYR]      = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_WIN_FN_LYR]  = {ENCODER_CCW_CW(_______, _______)},
    [_CTL_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_NUM_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_NAV_LYR]      = {ENCODER_CCW_CW(_______, _______)},
    [_FN_LYR]       = {ENCODER_CCW_CW(_______, _______)}
};
#endif
// clang-format on

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
void blink_numbers(bool isEnabling){
    for( int i = 55; i >= 44; i--) // 1(55) to EQL(44)
    {
        if(isEnabling){
            // enabling, flash white
            indicator_enqueue(i, 200, 3, RGB_WHITE);
        } else {
            // disabling, flash red
            indicator_enqueue(i, 150, 4, RGB_RED);
        }
    }
}

void blink_arrows(void){
    indicator_enqueue(62, 200, 3, RGB_WHITE );  // left
    indicator_enqueue(61, 200, 3, RGB_WHITE );  // down
    indicator_enqueue(15, 200, 3, RGB_WHITE );  // up
    indicator_enqueue(60, 200, 3, RGB_WHITE );  // right
}

void blink_NKRO(bool isEnabling){
    if(isEnabling){
        const uint8_t led_indexes[12] = {
            7, 8, 9, 10, 11, // V B N M ,
            20, 21, 22, 23, // K J H G
            35, 36, 37 // Y U I
        };

        for (int i = 0; i < 12; i++) {
            indicator_enqueue(led_indexes[i], 200, 3, RGB_WHITE );
        }
    }
    else {
        const uint8_t led_indexes[4] = {
            8, 10, // B M
            21, 22  // H J
        };

        for (int i = 0; i < 4; i++) {
            indicator_enqueue(led_indexes[i], 150, 3, RGB_RED );
        }
    }
}

void highlight_fn_keys(uint8_t led_min, uint8_t led_max)
{
    // get the current hsv value
    HSV current_hsv = rgb_matrix_get_hsv();
    // maximize brightness
    current_hsv.v = 255;

    rgb_led_t rgb = hsv_to_rgb(current_hsv);
    rgb_led_t new_rgb = get_complementary_color(rgb, false);
    for( int i = 55; i >= 44; i--){ // 55 - 44 are the number keys and - =
        RGB_MATRIX_INDICATOR_SET_COLOR(i, new_rgb.r, new_rgb.g, new_rgb.b);
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {

    uint8_t current_layer = get_highest_layer(layer_state);
    if(current_layer == _WIN_LYR)
    {
        if (rgb_matrix_get_flags() == LED_FLAG_INDICATOR) {
            for (int i = led_min; i < led_max; i++) {
                rgb_matrix_set_color(i, 0, 0, 0);
            }
            //rgb_matrix_set_color_all(0, 0, 0);
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

    if(fn_mode_enabled){
        highlight_fn_keys(led_min, led_max);
    }

    if (IS_LAYER_ON(_WIN_FN_LYR)) {
        if(!fn_mode_enabled){
            // we are not in fn_mode, but this layer also uses fn keys
            highlight_fn_keys(led_min, led_max);
        }

        const uint8_t led_indexes[4] = {
            28, // use caps as indicator
            63, // use fn as indicator
            56, 43 // ESC and BSPC
        };

        for (int i = 0; i < 4; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 255, 255, 0);
        }

        // highlight right shift as moving to ctl layer
        RGB_MATRIX_INDICATOR_SET_COLOR(14, 0, 255, 255);
    }

    if (IS_LAYER_ON(_CTL_LYR)) {
        const uint8_t led_indexes[4] = {
            59, // use PgDn as indicator
            39, // P for persistent color
            9,  // N for NKRO
            2   // lctl for Fn toggle
        };
        for (int i = 0; i < 4; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0x00, 0x80, 0x80);
        }

        const uint8_t led_off_indexes[15] = {
            // turn off some of the LEDS to make it easier to see our indicators
            // A, Home, PgUp
            27, 57, 58,
            // Arrow keys
            60, 61, 62, 15,
             // TAB, CAPS, LSFT, LG
            29, 28, 3, 1,
            // LALT, SPC, RALT, Fn
            0, 65, 64, 63
        };
        for (int i = 0; i < 15; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_off_indexes[i], 0x00, 0x00, 0x00);
        }

        // highlight Q as reset
        RGB_MATRIX_INDICATOR_SET_COLOR(30, 0xFF, 0x00, 0x00);

        // highlight Z as clear eeprom
        RGB_MATRIX_INDICATOR_SET_COLOR(4, 0x7A, 0x00, 0xFF);
    }

    if (IS_LAYER_ON(_NUM_LYR)) {
        const uint8_t led_indexes[9] = {
            58, // use PgUp as indicator

            // Light up around the numpad to make it easier to see
            // 6 is used as numlock and starts the numpad
            50, 35, 22, 9, // 6 y h n = 4 keys
            // right side
            46, 39, 18, 13 // 0 p ; / = 4 keys
        };

        for (int i = 0; i < 9; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0, 255, 0);
        }
    }

    if (IS_LAYER_ON(_NAV_LYR)) {
        const uint8_t led_indexes[16] = {
            57, // use home key as indicator

            // 2nd row
            30, 31, 32, 33, 34, // = 5 keys

            // 3rd row
            27, 26, 25, 24, 23, // = 5 keys

            // 4th row
            4, 5, 6, 7, 8 // = 5 keys
        };

        for (int i = 0; i < 16; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 0, 0, 255);
        }
    }

    if (IS_LAYER_ON(_FN_LYR)) {
        // highlight the toggle buttons
        RGB_MATRIX_INDICATOR_SET_COLOR(57, 0, 0, 255);   // NAV = Home
        RGB_MATRIX_INDICATOR_SET_COLOR(58, 0, 255, 0);   // NUM = PgUp
        RGB_MATRIX_INDICATOR_SET_COLOR(59, 0, 255, 255); // CTL = PgDn

        // highlight right shift as moving to ctl layer
        RGB_MATRIX_INDICATOR_SET_COLOR(14, 0, 255, 255);

        // highlight the aux buttons on right of keyboard
        const uint8_t led_indexes[7] = {
            64, //highlight the RALT button
            49, 48, 47, 46, 45, 44 // used for media keys = 6 keys
        };

        for (int i = 0; i < 7; i++) {
            RGB_MATRIX_INDICATOR_SET_COLOR(led_indexes[i], 128, 128, 128);
        }
    }

    process_indicator_queue(led_min, led_max);

    return true;
}
