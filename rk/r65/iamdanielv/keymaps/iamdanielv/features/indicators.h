#pragma once

#include "color.h"
#include QMK_KEYBOARD_H

rgb_t get_complementary_rgb(rgb_t rgb_led, bool darken);
hsv_t get_hsv_color_shifted(hsv_t color, uint8_t offset,  bool clockwise);
hsv_t get_base_hsv_color_inverse(void);
hsv_t get_base_hsv_color_shifted(bool clockwise);
hsv_t get_base_hsv_color_shifted_quarter(bool clockwise);
void blink_arrows(void);
void blink_space(bool);
void blink_NKRO(bool);
void blink_numbers(bool);
void highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max);
