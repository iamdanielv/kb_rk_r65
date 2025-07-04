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

// Copyright 2022-2025 Google LLC
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
 * @file layer_lock.h
 * @brief Layer Lock, a key to stay in the current layer.
 *
 * Overview
 * --------
 *
 * @note Layer Lock is now a core QMK feature! See
 * <https://docs.qmk.fm/features/layer_lock>
 *
 * Layers are often accessed by holding a button, e.g. with a momentary layer
 * switch `MO(layer)` or layer tap `LT(layer, key)` key. But you may sometimes
 * want to "lock" or "toggle" the layer so that it stays on without having to
 * hold down a button. One way to do that is with a tap-toggle `TT` layer key,
 * but here is an alternative.
 *
 * This library implements a "Layer Lock key". When tapped, it "locks" the
 * highest layer to stay active, assuming the layer was activated by one of the
 * following keys:
 *
 *  * `MO(layer)` momentary layer switch
 *  * `LT(layer, key)` layer tap
 *  * `OSL(layer)` one-shot layer
 *  * `TT(layer)` layer tap toggle
 *  * `LM(layer, mod)` layer-mod key (the layer is locked, but not the mods)
 *
 * Tapping the Layer Lock key again unlocks and turns off the layer.
 *
 * @note When a layer is "locked", other layer keys such as `TO(layer)` or
 * manually calling `layer_off(layer)` will override and unlock the layer.
 *
 * Configuration
 * -------------
 *
 * Optionally, a timeout may be defined so that Layer Lock disables
 * automatically if not keys are pressed for `LAYER_LOCK_IDLE_TIMEOUT`
 * milliseconds. Define `LAYER_LOCK_IDLE_TIMEOUT` in your config.h, for instance
 *
 *     #define LAYER_LOCK_IDLE_TIMEOUT 60000  // Turn off after 60 seconds.
 *
 * and call `layer_lock_task()` from your `housekeeping_task_user()` in
 * keymap.c:
 *
 *     void housekeeping_task_user(void) {
 *       layer_lock_task();
 *       // Other tasks...
 *     }
 *
 * For full documentation, see
 * <https://getreuer.info/posts/keyboards/layer-lock>
 */

#pragma once

#include "quantum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Handler function for Layer Lock.
 *
 * In your keymap, define a custom keycode to use for Layer Lock. Then handle
 * Layer Lock from your `process_record_user` function by calling
 * `process_layer_lock`, passing your custom keycode for the `lock_keycode` arg:
 *
 *     #include "features/layer_lock.h"
 *
 *     bool process_record_user(uint16_t keycode, keyrecord_t* record) {
 *       if (!process_layer_lock(keycode, record, LLOCK)) { return false; }
 *       // Your macros ...
 *
 *       return true;
 *     }
 */
bool dv_process_layer_lock(uint16_t keycode, keyrecord_t* record, uint16_t lock_keycode);

/** Returns true if `layer` is currently locked. */
bool dv_is_layer_locked(uint8_t layer);

/** Locks and turns on `layer`. */
void dv_layer_lock_on(uint8_t layer);

/** Unlocks and turns off `layer`. */
void dv_layer_lock_off(uint8_t layer);

/** Unlocks and turns off all locked layers. */
void dv_layer_lock_all_off(void);

/** Toggles whether `layer` is locked. */
void dv_layer_lock_invert(uint8_t layer);

/**
 * Optional callback that gets called when a layer is locked or unlocked.
 *
 * This is useful to represent the current lock state, e.g. by setting an LED or
 * playing a sound. In your keymap, define
 *
 *     void layer_lock_set_user(layer_state_t locked_layers) {
 *       // Do something like `set_led(is_layer_locked(NAV));`
 *     }
 *
 * @param locked_layers Bitfield in which the kth bit represents whether the
 *                      kth layer is on.
 */
void dv_layer_lock_set_user(layer_state_t locked_layers);

#ifdef __cplusplus
}
#endif
