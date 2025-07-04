// 2025 DV (@iamdanielv)
// based on https://getreuer.info/posts/keyboards/layer-lock
// While playing with the original Layer Lock library, I found that it was not
// working for layers that were toggled. This modified version of the library
// takes into account toggled layers and considers them locked.
// This allows us to toggle the layer on and off and use the layer lock key.

// I also noticed that the original library only considered the top most layer
// and toggled it's lock state. I've added some code to check from which layer
// the lock key was pressed and lock/unlock that layer.

// I prepended all function names with 'dv' to make sure I am calling the updated
// functions and not the original ones.

// Copyright 2022-2024 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file layer_lock.c
 * @brief Layer Lock implementation
 *
 * For original full documentation, see
 * <https://getreuer.info/posts/keyboards/layer-lock>
 */

#include "defines.h"
#include "dv_layer_lock.h"

// DV - removed this message, since I am testing some modifications to the original library
// #pragma message "Layer Lock is now a core QMK feature! To use it, update your QMK set up and see https://docs.qmk.fm/features/layer_lock"

// The current lock state. The kth bit is on if layer k is locked.
static layer_state_t dv_locked_layers = 0;

// Handles an event on an `MO` or `TT` layer switch key.
static bool dv_handle_mo_or_tt(uint8_t layer, keyrecord_t* record) {
    if (dv_is_layer_locked(layer)) {
        if (record->event.pressed) { // On press, unlock the layer.
            dv_layer_lock_invert(layer);
        }
        return false; // Skip default handling.
    }
    return true;
}

bool dv_process_layer_lock(uint16_t keycode, keyrecord_t* record, uint16_t lock_keycode) {
#if LAYER_LOCK_IDLE_TIMEOUT > 0
    layer_lock_timer = timer_read32();
#endif // LAYER_LOCK_IDLE_TIMEOUT > 0

    // The intention is that locked layers remain on. If something outside of
    // this feature turned any locked layers off, unlock them.
    if ((dv_locked_layers & ~layer_state) != 0) {
        dv_layer_lock_set_user(dv_locked_layers &= layer_state);
    }

    if (keycode == lock_keycode) {
        if (record->event.pressed) { // The layer lock key was pressed.

            // original
            // layer_lock_invert(get_highest_layer(layer_state));

            // DV edit
            //  we need to determine what layer we are on, and then change current state
            uint8_t current_layer = layer_switch_get_layer(record->event.key);
            dv_layer_lock_invert(current_layer);
        }
        return false;
    }

    switch (keycode) {
        case QK_MOMENTARY ... QK_MOMENTARY_MAX: // `MO(layer)` keys.
            return dv_handle_mo_or_tt(QK_MOMENTARY_GET_LAYER(keycode), record);

        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX: // `TT(layer)`.
            return dv_handle_mo_or_tt(QK_LAYER_TAP_TOGGLE_GET_LAYER(keycode), record);

        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX: // `TG(layer)` keys.
            // indicator_enqueue(LEFT_CTL_KI, 150, 2, RGB_ORANGE);
            // if (dv_is_layer_locked(QK_TOGGLE_LAYER_GET_LAYER(keycode))) {
            if (record->event.pressed) { // On press, unlock the layer.
                dv_layer_lock_invert(QK_TOGGLE_LAYER_GET_LAYER(keycode));
            }
            return false; // Skip default handling.
            //}
            break;

        case QK_LAYER_MOD ... QK_LAYER_MOD_MAX: { // `LM(layer, mod)`.
            uint8_t layer = QK_LAYER_MOD_GET_LAYER(keycode);
            if (dv_is_layer_locked(layer)) {
                if (record->event.pressed) { // On press, unlock the layer.
                    dv_layer_lock_invert(layer);
                } else { // On release, clear the mods.
                    clear_mods();
                    send_keyboard_report();
                }
                return false; // Skip default handling.
            }
        } break;

#ifndef NO_ACTION_TAPPING
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX: // `LT(layer, key)` keys.
            if (record->tap.count == 0 && !record->event.pressed && dv_is_layer_locked(QK_LAYER_TAP_GET_LAYER(keycode))) {
                // Release event on a held layer-tap key where the layer is locked.
                return false; // Skip default handling so that layer stays on.
            }
            break;
#endif // NO_ACTION_TAPPING
    }

    return true;
}

bool dv_is_layer_locked(uint8_t layer) {
    return dv_locked_layers & ((layer_state_t)1 << layer);
}

void dv_layer_lock_invert(uint8_t layer) {
    const layer_state_t mask = (layer_state_t)1 << layer;
    if ((dv_locked_layers & mask) == 0) { // Layer is being locked.
#ifndef NO_ACTION_ONESHOT
        if (layer == get_oneshot_layer()) {
            reset_oneshot_layer(); // Reset so that OSL doesn't turn layer off.
        }
#endif // NO_ACTION_ONESHOT
        layer_on(layer);
#if LAYER_LOCK_IDLE_TIMEOUT > 0
        layer_lock_timer = timer_read32();
#endif       // LAYER_LOCK_IDLE_TIMEOUT > 0
    } else { // Layer is being unlocked.
        layer_off(layer);
    }
    dv_layer_lock_set_user(dv_locked_layers ^= mask);
}

// Implement layer_lock_on/off by deferring to layer_lock_invert.
void dv_layer_lock_on(uint8_t layer) {
    if (!dv_is_layer_locked(layer)) {
        dv_layer_lock_invert(layer);
    }
}

void dv_layer_lock_off(uint8_t layer) {
    if (dv_is_layer_locked(layer)) {
        dv_layer_lock_invert(layer);
    }
}

void dv_layer_lock_all_off(void) {
    layer_and(~dv_locked_layers);
    dv_locked_layers = 0;
    dv_layer_lock_set_user(dv_locked_layers);
}

__attribute__((weak)) void dv_layer_lock_set_user(layer_state_t locked_layers) {}
