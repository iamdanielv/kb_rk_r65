// Copyright 2025 DV (@iamdanielv)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

enum layer_names {
    BASE_LYR,       // 0 - regular qwerty
    HRM_BASE_LYR,   // 1 - home row mods qwerty
    EXT_LYR,        // 2 - similar to extend
    KBCTL_LYR,      // 3 - keyboard control layer
    NUM_LYR,        // 4 - numpad
    MEDIA_LYR       // 5 - media keys
};

// clang-format off
/*  LED Matrix
    ESC  1    2    3    4    5    6    7    8    9    0    -    =    BSPC MUTE
    56   55   54   53   52   51   50   49   48   47   46   45   44   43

    TAB  Q    W    E    R    T    Y    U    I    O    P    [    ]    BSLS HOME
    29   30   31   32   33   34   35   36   37   38   39   40   41   42   57

    CAPS A    S    D    F    G    H    J    K    L    ;    '         ENT  PGUP
    28   27   26   25   24   23   22   21   20   19   18   17        16   58

    LSFT Z    X    C    V    B    N    M    ,    .    /    RSFT      UP   PGDN
    3    4    5    6    7    8    9    10   11   12   13   14        15   59

    LCTL LG   LALT           SPC            RALT Fn       LEFT      DOWN RGHT
    2    1    0              65             64   63        62        61   60
*/
// clang-format on

// ***************
// * Key Indexes *
// ***************

#define ESC_KI 56
#define BSPC_KI 43

// Modifiers
#define GRV_KI 56
#define TAB_KI 29
#define CAPS_KI 28
#define LEFT_SFT_KI 3
#define LEFT_CTL_KI 2
#define LEFT_WIN_KI 1
#define LEFT_ALT_KI 0
#define SPACE_KI 65
#define FN_KI 63
#define RIGHT_ALT_KI 64
#define RIGHT_SFT_KI 14
#define ENTER_KI 16
#define HOME_KI 57
#define PGUP_KI 58
#define PGDN_KI 59

// Letter Keys
#define A_KI 27
#define B_KI 8
#define C_KI 6
#define D_KI 25
#define E_KI 32
#define F_KI 24
#define G_KI 23
#define H_KI 22
#define I_KI 37
#define J_KI 21
#define K_KI 20
#define L_KI 19
#define M_KI 10
#define N_KI 9
#define O_KI 38
#define P_KI 39
#define Q_KI 30
#define R_KI 33
#define S_KI 26
#define T_KI 34
#define U_KI 36
#define V_KI 7
#define W_KI 31
#define X_KI 5
#define Y_KI 35
#define Z_KI 4

// number keys
#define N1_KI 55
#define N2_KI 54
#define N3_KI 53
#define N4_KI 52
#define N5_KI 51
#define N6_KI 50
#define N7_KI 49
#define N8_KI 48
#define N9_KI 47
#define N0_KI 46

// F Keys
#define FN1_KI 55
#define FN2_KI 54
#define FN3_KI 53
#define FN4_KI 52
#define FN5_KI 51
#define FN6_KI 50
#define FN7_KI 49
#define FN8_KI 48
#define FN9_KI 47
#define FN10_KI 46
#define FN11_KI 45
#define FN12_KI 44

// symbols
#define COMM_KI 11 // ,
#define DOT_KI 12  // .
#define BSLS_KI 42 // \ backslash
#define SLSH_KI 13 // / forwardslash
#define SCLN_KI 18 // ;
#define QUOT_KI 17 // '
#define MINS_KI 45 // -
#define EQL_KI 44  // =

// arrows
#define UP_KI 15
#define LEFT_KI 62
#define DOWN_KI 61
#define RIGHT_KI 60

// ******************************
// * Aliases to simplify keymap *
// ******************************

// *****************
// * Home Row Mods *
// *****************
// based on: https://precondition.github.io/home-row-mods#getting-started-with-home-row-mods-on-qmk

// Left-hand home row mods
#define GUI_A LGUI_T(KC_A)
#define ALT_S LALT_T(KC_S)
#define SFT_D LSFT_T(KC_D)
#define CTL_F LCTL_T(KC_F)

// Right-hand home row mods
#define CTL_J RCTL_T(KC_J)
#define SFT_K RSFT_T(KC_K)
#define ALT_L RALT_T(KC_L)
#define GUI_SCLN RGUI_T(KC_SCLN)

// *******************************
// * More keycodes for HRM Layer *
// *******************************
#define CTLS_C C_S_T(KC_C) // tap: C ; hold: ctl and shift

#define MO_CAPS   TD(TD_MO_CAPS)
#define MO_RALT   LT(MEDIA_LYR, KC_RALT)
#define MY_GRV    TD(TD_GRV)

// *************
// * Shortcuts *
// *************
#define MY_UNDO   C(KC_Z)
#define MY_CUT    C(KC_X)
#define MY_COPY   C(KC_INS)
#define MY_PASTE  S(KC_INS)
#define MY_TASK   LCTL(LSFT(KC_ESC))
#define MY_CONS   LCTL(LSFT(KC_GRV))
#define MY_BACK   A(KC_LEFT)
#define MY_FWD    A(KC_RIGHT)
#define MSW_UP    KC_MS_WH_UP
#define MSW_DN    KC_MS_WH_DOWN

// ***********
// * Toggles *
// ***********
#define TG_NUM    TG(NUM_LYR)
#define TG_EXT    TG(EXT_LYR)
#define TG_KBCTL  TG(KBCTL_LYR)
#define TG_MEDIA  TG(MEDIA_LYR)
#define MO_KBCTL  MO(KBCTL_LYR)
#define TD_KB_RST TD(TD_RESET)
#define TD_KB_CLR TD(TD_CLEAR)
