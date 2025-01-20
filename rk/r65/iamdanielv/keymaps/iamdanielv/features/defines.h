// Copyright 2025 DV (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

enum layer_names {
    _WIN_LYR,    // 0
    _WIN_FN_LYR, // 1
    _CTL_LYR,    // 2
    _NUM_LYR,    // 3
    _FN_LYR,     // 4
};

// ***************
// * Key Indexes *
// ***************
#define TAB_KI 29
#define CAPS_KI 28
#define Q_KI 30
#define Z_KI 4
#define LEFT_SFT_KI 3
#define LEFT_CTL_KI 2
#define LEFT_WIN_KI 1
#define LEFT_ALT_KI 0
#define SPACE_KI 65
#define RIGHT_ALT_KI 64
#define FN_KI 63
#define RIGHT_SFT_KI 14
#define ENTER_KI 16
#define HOME_KI 57
#define PGUP_KI 58
#define PGDN_KI 59

#define F_KI 24

#define I_KI 37
#define J_KI 21
#define K_KI 20
#define L_KI 19

#define W_KI 31
#define A_KI 27
#define S_KI 26
#define D_KI 25

#define UP_KI 15
#define LEFT_KI 62
#define DOWN_KI 61
#define RIGHT_KI 60

// ******************************
// * Aliases to simplify keymap *
// ******************************
#define W_FN_CAPS LT(_WIN_FN_LYR, KC_CAPS)
#define FN_RALT   LT(_FN_LYR, KC_RALT)

// shortcuts
#define MY_UNDO   C(KC_Z)
#define MY_CUT    C(KC_X)
#define MY_COPY   C(KC_INS)
#define MY_PASTE  S(KC_INS)
#define MY_TASK   LCTL(LSFT(KC_ESC))
#define MY_CONS   LCTL(LSFT(KC_GRV))
#define MY_BACK   A(KC_LEFT)
#define MY_FWD    A(KC_RIGHT)

#define TG_NUM    TG(_NUM_LYR)
#define TG_W_FN   TG(_WIN_FN_LYR)
#define TG_CTL    TG(_CTL_LYR)
#define MO_CTL    MO(_CTL_LYR)
#define TD_KB_RST TD(TD_RESET)
#define TD_KB_CLR TD(TD_CLEAR)

// placeholder, this is handled in process_record_user
#define MY_GRV    LT(0, KC_GRV)

