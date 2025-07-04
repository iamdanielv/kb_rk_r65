#pragma once

#include QMK_KEYBOARD_H

// This macro sets the color of an indicator if its led_index is within the valid range
#define INDICATOR_Q_MATRIX_SET_COLOR(indicator)                                           \
    if ((indicator).led_index >= led_min && (indicator).led_index < led_max) {            \
        rgb_matrix_set_color((indicator).led_index, (indicator).r, (indicator).g, (indicator).b); \
    }

// This macro sets a custom color for an indicator if its led_index is within the valid range
#define INDICATOR_Q_MATRIX_SET_COLOR_CUSTOM(indicator, r, g, b)            \
    if ((indicator).led_index >= led_min && (indicator).led_index < led_max) { \
        rgb_matrix_set_color((indicator).led_index, (r), (g), (b));                \
    }

// This macro retrieves the RGB values of an indicator as an rgb_t struct
#define INDICATOR_Q_GET_RGB_LED(indicator)                   \
    (rgb_t) {                                            \
        .r = (indicator).r, .g = (indicator).g, .b = (indicator).b \
    }

#define INDICATOR_QUEUE_MAX 20

typedef struct {
    bool     active;
    uint8_t  led_index;
    uint32_t last_update;
    uint32_t interval;
    uint8_t  times_to_flash;
    uint8_t  r;
    uint8_t  g;
    uint8_t  b;
} indicator_t;

// Indicator queue to hold the indicators
indicator_t indicator_queue[INDICATOR_QUEUE_MAX];

/**
 * @brief Enqueue an indicator to the indicator queue.
 *
 * @param led_index Index of the LED to control.
 * @param interval Time interval between flashes.
 * @param times_to_flash Number of times to flash the LED (visible flashes). Internally, this is doubled to account for on/off cycles.
 * @param r Red color value.
 * @param g Green color value.
 * @param b Blue color value.
 */
void indicator_enqueue(uint8_t led_index, uint32_t interval, uint8_t times_to_flash, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Process the indicator queue and update the LEDs.
 *
 * @param led_min Minimum LED index to process.
 * @param led_max Maximum LED index to process.
 */
void process_indicator_queue(uint8_t led_min, uint8_t led_max);
