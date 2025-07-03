#pragma once

#include "color.h"
#include QMK_KEYBOARD_H

#define INDICATOR_RGB_DARK_RED      0x80, 0x00, 0x00
#define INDICATOR_RGB_DARK_GREEN    0x00, 0x80, 0x00
#define INDICATOR_RGB_DARK_BLUE     0x00, 0x00, 0x80
#define INDICATOR_RGB_PASTEL_RED    0xCC, 0x66, 0x99
#define INDICATOR_RGB_PASTEL_GREEN  0x66, 0x99, 0x66
#define INDICATOR_RGB_LAYER_LOCK    0xAA, 0x22, 0x00
#define INDICATOR_RGB_KBCTL_CLEAR   0x55, 0x00, 0x55
#define INDICATOR_RGB_MEDIA_KEYS    0xFF, 0xFF, 0xF0
#define INDICATOR_RGB_PASTEL_BLUE   0x66, 0x99, 0xCC

// Blinking constants
#define INDICATOR_INTERVAL_FAST   150
#define INDICATOR_INTERVAL_NORMAL 200
#define INDICATOR_FLASHES_SHORT   2
#define INDICATOR_FLASHES_LONG    3
#define INDICATOR_FLASHES_EXTRA_LONG    4

// Color calculation offsets
#define HUE_OFFSET_INVERSE 128
#define HUE_OFFSET_SHIFTED 32
#define HUE_OFFSET_QUARTER 64
#define HUE_OFFSET_ACCENT 31

rgb_t get_complementary_rgb(rgb_t rgb_led, bool darken);
hsv_t get_hsv_color_shifted(hsv_t color, uint8_t offset, bool clockwise);
hsv_t get_base_hsv_color_inverse(void);
hsv_t get_base_hsv_color_shifted(bool clockwise);
hsv_t get_base_hsv_color_shifted_quarter(bool clockwise);

void blink_arrows(void);
void blink_space(bool);
void blink_NKRO(bool);
void blink_numbers(bool);
void highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max);

extern bool recalculate_rgb;
