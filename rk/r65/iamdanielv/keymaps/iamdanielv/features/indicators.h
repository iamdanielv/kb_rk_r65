#pragma once

#include "color.h"
#include QMK_KEYBOARD_H

#define INDICATOR_RGB_DARK_RED      0x80, 0x00, 0x00
#define INDICATOR_RGB_DARK_GREEN    0x00, 0x80, 0x00
#define INDICATOR_RGB_DARK_BLUE     0x00, 0x00, 0x80
#define INDICATOR_RGB_PASTEL_RED    0xCC, 0x66, 0x99
#define INDICATOR_RGB_PASTEL_GREEN  0x66, 0x99, 0x66
#define INDICATOR_RGB_PASTEL_BLUE   0x66, 0x99, 0xCC

rgb_t get_complementary_rgb(rgb_t rgb_led, bool darken);
hsv_t get_hsv_color_shifted(hsv_t color, uint8_t offset, bool clockwise);
hsv_t get_base_hsv_color_inverse(void);
hsv_t get_base_hsv_color_shifted(bool clockwise);
hsv_t get_base_hsv_color_shifted_quarter(bool clockwise);

void  blink_arrows(void);
void  blink_space(bool);
void  blink_NKRO(bool);
void  blink_numbers(bool);
void  highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max);
