#include "indicator_queue.h"
#include "indicators.h"

#define TIMER_DEFAULT_VALUE 0x00

// Enqueue an indicator to the queue
void indicator_enqueue(uint8_t led_index, uint32_t interval, uint8_t times_to_flash, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < INDICATOR_QUEUE_MAX; i++) {
        if (!indicator_queue[i].active) {
            // This queue position is not active, so we can use it
            indicator_queue[i].active         = true;
            indicator_queue[i].led_index      = led_index;
            indicator_queue[i].last_update    = timer_read32();
            indicator_queue[i].interval       = interval;
            indicator_queue[i].times_to_flash = times_to_flash * 2;
            indicator_queue[i].r              = r;
            indicator_queue[i].g              = g;
            indicator_queue[i].b              = b;
            return;
        }
    }
}

// Dequeue an indicator from the queue
void indicator_dequeue(uint8_t led_index) {
    for (int i = 0; i < INDICATOR_QUEUE_MAX; i++) {
        if (indicator_queue[i].active && indicator_queue[i].led_index == led_index) {
            // We found a match, deactivate it
            indicator_queue[i].active      = false;
            indicator_queue[i].last_update = TIMER_DEFAULT_VALUE;
            // Continue checking in case there are duplicates
        }
    }
}

// Process the indicator queue
void process_indicator_queue(uint8_t led_min, uint8_t led_max) {
    for (int i = 0; i < INDICATOR_QUEUE_MAX; i++) {
        if (indicator_queue[i].active) {
            // This queue position is active, process it
            if (timer_elapsed32(indicator_queue[i].last_update) >= indicator_queue[i].interval) {
                // The timer has elapsed, perform the action
                indicator_queue[i].last_update = timer_read32(); // Reset the timer to now

                if (indicator_queue[i].times_to_flash) {
                    indicator_queue[i].times_to_flash--;
                }

                if (indicator_queue[i].times_to_flash <= 0) {
                    // We have flashed as many times as requested, clear this queue spot
                    indicator_queue[i].active      = false;
                    indicator_queue[i].last_update = TIMER_DEFAULT_VALUE;
                }
            }

            if (indicator_queue[i].times_to_flash % 2) {
                INDICATOR_Q_MATRIX_SET_COLOR(indicator_queue[i]);
            } else {
                rgb_t this_rgb_led = INDICATOR_Q_GET_RGB_LED(indicator_queue[i]);
                rgb_t alt          = get_complementary_rgb(this_rgb_led, false);
                INDICATOR_Q_MATRIX_SET_COLOR_CUSTOM(indicator_queue[i], alt.r, alt.g, alt.b);
            }
        }
    }
}
