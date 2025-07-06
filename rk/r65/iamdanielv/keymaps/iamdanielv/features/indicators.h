#pragma once

#include "color.h"
#include QMK_KEYBOARD_H

#define RGB_DRK_RED     0x80, 0x00, 0x00   // Dark Red
#define RGB_DRK_GREEN   0x00, 0x80, 0x00   // Dark Green
#define RGB_DRK_BLUE    0x00, 0x00, 0x80   // Dark Blue
#define RGB_PSTL_RED    0xCC, 0x66, 0x99   // Pastel Red
#define RGB_PSTL_GREEN  0x66, 0x99, 0x66   // Pastel Green
#define RGB_PSTL_BLUE   0x66, 0x99, 0xCC   // Pastel Blue
#define RGB_LAYER_LOCK  0xAA, 0x22, 0x00   // Layer Lock
#define RGB_KBCTL_CLEAR 0x55, 0x00, 0x55   // KBCTL Clear
#define RGB_MEDIA_KEYS  0xFF, 0xFF, 0xF0   // Media Keys

// Color calculation offsets
#define HUE_INVERSE 128
#define HUE_SHIFTED 32
#define HUE_QUARTER 64
#define HUE_ACCENT 31

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
void blink_space(bool extended);

/**
 * @brief Blinks the NKRO indicator keys.
 *
 * @param isEnabling Whether NKRO is being enabled or disabled.
 */
void blink_NKRO(bool isEnabling);

/**
 * @brief Blinks the number keys.
 *
 * @param isEnabling Whether the numbers are being enabled or disabled.
 */
void blink_numbers(bool isEnabling);

/**
 * @brief Highlights the function keys with a specified color.
 *
 * @param color The RGB color to highlight with.
 * @param led_min The minimum LED index.
 * @param led_max The maximum LED index.
 */
void highlight_fn_keys(rgb_t color, uint8_t led_min, uint8_t led_max);

/**
 * @brief Sets the color of a list of keys.
 *
 * @param color The RGB color to set.
 * @param led_min The minimum LED index to consider.
 * @param led_max The maximum LED index to consider.
 * @param key_indexes An array of key indexes to set the color for.
 * @param num_keys The number of keys in the `key_indexes` array.
 */
void set_keys_color(rgb_t color, uint8_t led_min, uint8_t led_max, const uint8_t key_indexes[], int num_keys);

/**
 * @brief Sets the indicator LED to the color of a layer
 *
 * @param layer The layer to check
 * @param led_index The index of the LED to set
 * @param color The color to set the LED to
 * @param led_min The minimum LED index, used by RGB_MATRIX_INDICATOR_SET_COLOR
 * @param led_max The maximum LED index, used by RGB_MATRIX_INDICATOR_SET_COLOR
 */
void set_layer_color_indicator(uint8_t layer, uint8_t led_index, rgb_t color, uint8_t led_min, uint8_t led_max);

/**
 * @brief Advanced user function for RGB matrix indicators.
 *
 * This function is called by the RGB matrix code to allow for custom LED indicator behavior.
 * It handles various layer indicators, caps lock, and NKRO status.
 *
 * @param led_min The minimum LED index to consider.
 * @param led_max The maximum LED index to consider.
 * @return True if the pipeline should continue processing, false if processing should stop.
 */
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max);

extern bool recalculate_rgb;
