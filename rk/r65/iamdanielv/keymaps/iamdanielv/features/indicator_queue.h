#pragma once

#include "quantum.h"

#define INDICATOR_QUEUE_MAX 20

typedef struct {
    bool active;
    uint8_t led_index;
    uint32_t last_update;
    uint32_t interval;
    uint8_t times_to_flash;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} indicator_t;

indicator_t indicator_queue[INDICATOR_QUEUE_MAX];

rgb_led_t get_complementary_color(rgb_led_t rgb_led, bool darken);

void indicator_enqueue(uint8_t led_index, uint32_t interval, uint8_t times_to_flash, uint8_t r, uint8_t g, uint8_t b);

void process_indicator_queue(uint8_t led_min, uint8_t led_max);
