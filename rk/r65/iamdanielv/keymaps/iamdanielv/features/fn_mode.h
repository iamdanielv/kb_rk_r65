#pragma once

#include QMK_KEYBOARD_H

extern bool fn_mode_enabled;

bool process_fn_mode(uint16_t keycode, keyrecord_t *record);
