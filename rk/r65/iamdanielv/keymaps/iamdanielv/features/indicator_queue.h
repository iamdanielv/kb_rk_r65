#pragma once

#include QMK_KEYBOARD_H

// this is similar to RGB_MATRIX_INDICATOR_SET_COLOR
// but takes in an indicator_t instead
#define INDICATOR_Q_MATRIX_SET_COLOR(indicator)                                           \
    if (indicator.led_index >= led_min && indicator.led_index < led_max) {                \
        rgb_matrix_set_color(indicator.led_index, indicator.r, indicator.g, indicator.b); \
    }

#define INDICATOR_Q_MATRIX_SET_COLOR_CUSTOM(indicator, r, g, b)            \
    if (indicator.led_index >= led_min && indicator.led_index < led_max) { \
        rgb_matrix_set_color(indicator.led_index, r, g, b);                \
    }

#define INDICATOR_Q_GET_RGB_LED(indicator)                   \
    (rgb_t) {                                            \
        .r = indicator.r, .g = indicator.g, .b = indicator.b \
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

indicator_t indicator_queue[INDICATOR_QUEUE_MAX];

void indicator_enqueue(uint8_t led_index, uint32_t interval, uint8_t times_to_flash, uint8_t r, uint8_t g, uint8_t b);

void process_indicator_queue(uint8_t led_min, uint8_t led_max);
