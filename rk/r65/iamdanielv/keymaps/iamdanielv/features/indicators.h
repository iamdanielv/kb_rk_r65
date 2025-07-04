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

/**
 * @brief Calculates the complementary RGB color.
 *
 * @param rgb_led The original RGB color.
 * @param darken Whether to darken the complementary color.
 * @return The complementary RGB color.
 */
rgb_t get_complementary_rgb(rgb_t rgb_led, bool darken);
/**
 * @brief Shifts the hue of an HSV color.
 *
 * @param color The original HSV color.
 * @param offset The amount to shift the hue.
 * @param clockwise Whether to shift clockwise or counter-clockwise.
 * @return The shifted HSV color.
 */
hsv_t get_hsv_color_shifted(hsv_t color, uint8_t offset, bool clockwise);
/**
 * @brief Gets the inverse of the base HSV color.
 *
 * @return The inverse base HSV color.
 */
hsv_t get_base_hsv_color_inverse(void);
/**
 * @brief Gets a shifted version of the base HSV color.
 *
 * @param clockwise Whether to shift clockwise or counter-clockwise.
 * @return The shifted base HSV color.
 */
hsv_t get_base_hsv_color_shifted(bool clockwise);
/**
 * @brief Gets a quarter-shifted version of the base HSV color.
 *
 * @param clockwise Whether to shift clockwise or counter-clockwise.
 * @return The quarter-shifted base HSV color.
 */
hsv_t get_base_hsv_color_shifted_quarter(bool clockwise);

/**
 * @brief Blinks the arrow keys with white.
 */
void blink_arrows(void);
/**
 * @brief Blinks the space key, and optionally the left and right Alt keys.
 *
 * @param extended Whether to blink the left and right Alt keys as well.
 */
void blink_space(bool);
/**
 * @brief Blinks the NKRO indicator keys.
 *
 * @param isEnabling Whether NKRO is being enabled or disabled.
 */
void blink_NKRO(bool);
/**
 * @brief Blinks the number keys.
 *
 * @param isEnabling Whether the numbers are being enabled or disabled.
 */
void blink_numbers(bool);
/**
 * @brief Highlights the function keys with a specified color.
 *
 * @param color The RGB color to highlight with.
 * @param led_min The minimum LED index.
 * @param led_max The maximum LED index.
 */
void highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max);

extern bool recalculate_rgb;
