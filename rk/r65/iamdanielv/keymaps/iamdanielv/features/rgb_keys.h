#pragma once

#include QMK_KEYBOARD_H

bool process_rgb_keys(uint16_t keycode, keyrecord_t *record);

extern bool recalculate_rgb;
