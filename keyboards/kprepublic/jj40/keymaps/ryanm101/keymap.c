// Copyright 2023 ryanm101
// SPDX-License-Identifier: GPL-2.0-or-later

// JJ40 keymap — ryanm101
// This is identical to layouts/ortho_4x12/ryanm101 but built as a
// keyboard-specific keymap so it works with the build system directly.

#include QMK_KEYBOARD_H

#define LCKSCR LCTL(LGUI(KC_Q))

#define _QWERTY 0
#define _L1 1
#define _L2 2
#define _L3 3

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
};

// Tap Dance
#define COLON TD(CLN)
#define QUOTE TD(QUOT)
#define CADCAE TD(CAD_CAE)
#define PARAN TD(PAR)
#define CURLY TD(CUR)
#define SQUAR TD(SQU)
#define ANGUL TD(ANG)
#define TMUX TD(TD_TMUX)
#define GIT TD(TD_GIT)
#define SHIFTSLASHPIPE TD(TD_SHIFTSLASHPIPE)

enum {
  CLN = 0,
  QUOT,
  CAD_CAE,
  PAR,
  CUR,
  SQU,
  ANG,
  TD_TMUX,
  TD_GIT,
  TD_SHIFTSLASHPIPE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Qwerty
     * ,-------------------------------------------------------------------------------------.
     * | Tab  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |
     * P  | Bksp |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * | Esc  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |
     * ;:  |  '@  |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |  ,<  |  .>  |
     * /?  |Enter |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl | GUI  | Alt  |  \|  |  L1  |    Space    |  L2  | Left | Down |
     * Up  |Right |
     * `-------------------------------------------------------------------------------------'
     */
    [_QWERTY] = LAYOUT_ortho_4x12(
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P,
        KC_BSPC, KC_ESC, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L,
        COLON, QUOTE, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M,
        KC_COMM, KC_DOT, KC_SLSH, KC_ENT, KC_LCTL, KC_LGUI, KC_LALT,
        KC_NONUS_BACKSLASH, MO(_L1), KC_SPC, KC_SPC, MO(_L2), KC_LEFT, KC_DOWN,
        KC_UP, KC_RGHT),

    /* L1
     * ,-------------------------------------------------------------------------------------.
     * |  `¬  |INSERT| PGUP | HOME |  <>  |      |  ()  |  7&  |  8*  |  9(  |
     * | Bksp |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * |      |DELETE| PGDN | END  |  []  |  {}  |  =+  |  4$  |  5%  |  6^  |
     * *   |      |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * |LSHIFT|PSCRN |      |CADCAE|  L3  |LCKSCR|  -_  |  1!  |  2"  |  3£  |
     * /?  |ENTER |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl | GUI  | Alt  |  #~  |      |    Space    |      |   0) |  .>  |
     * | MENU |
     * `-------------------------------------------------------------------------------------'
     */
    [_L1] = LAYOUT_ortho_4x12(
        KC_GRV, KC_INS, KC_PGUP, KC_HOME, ANGUL, KC_NO, PARAN, KC_7, KC_8, KC_9,
        KC_NO, KC_BSPC, KC_NO, KC_DEL, KC_PGDN, KC_END, SQUAR, CURLY, KC_EQL,
        KC_4, KC_5, KC_6, S(KC_8), KC_NO, _______, KC_PSCR, KC_NO, CADCAE,
        MO(_L3), LCKSCR, KC_MINS, KC_1, KC_2, KC_3, KC_SLSH, KC_ENT, _______,
        _______, _______, KC_NUHS, KC_NO, KC_SPC, KC_SPC, KC_NO, ALGR_T(KC_DOT),
        KC_NO, KC_NO, KC_APP),

    /* L2
     * ,-------------------------------------------------------------------------------------.
     * |Sysreq|  F9  |  F10 |  F11 |  F12 |      |NUMLK |   7  |   8  |   9  |
     * *   |Bksp  |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * |      |  F5  |  F6  |  F7  |  F8  |      |      |   4  |   5  |   6  |
     * /   |      |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * |      |  F1  |  F2  |  F3  |  F4  |      |      |   1  |   2  |   3  |
     * +   |KP_ENT|
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Ctrl | GUI  | Alt  |      |      |    Space    |      |   0  |   .  |
     * -   |      |
     * `-------------------------------------------------------------------------------------'
     */
    [_L2] = LAYOUT_ortho_4x12(
        KC_SYRQ, KC_F9, KC_F10, KC_F11, KC_F12, KC_NO, KC_NUM, KC_KP_7, KC_KP_8,
        KC_KP_9, KC_KP_ASTERISK, KC_BSPC, KC_NO, KC_F5, KC_F6, KC_F7, KC_F8,
        KC_NO, KC_NO, KC_KP_4, KC_KP_5, KC_KP_6, KC_KP_SLASH, _______, KC_NO,
        KC_F1, KC_F2, KC_F3, KC_F4, KC_NO, KC_NO, KC_KP_1, KC_KP_2, KC_KP_3,
        KC_KP_PLUS, KC_KP_ENTER, _______, _______, _______, KC_NO, KC_NO,
        KC_SPC, KC_SPC, KC_NO, KC_KP_0, KC_KP_DOT, KC_KP_MINUS, _______),

    /* L3 - RGB / Navigation
     * jj40/rev1: 5x WS2812 underglow (rgblight) + backlight (pin D4)
     */
    [_L3] = LAYOUT_ortho_4x12(
        UG_TOGG, UG_HUEU, UG_HUED, UG_SATU, UG_SATD, KC_NO, KC_NO, KC_PGDN,
        KC_PGUP, KC_NO, KC_NO, KC_NO, BL_TOGG, BL_STEP, UG_VALU, UG_VALD,
        UG_NEXT, UG_PREV, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_NO, KC_NO, TMUX,
        GIT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_ENT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_SPC, KC_SPC, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO),
};

// Parentheses
void paranthesis_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("()");
    register_code(KC_LEFT);
    unregister_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("(");
  } else if (state->count == 3) {
    SEND_STRING(")");
  }
}
void curly_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("{}");
    register_code(KC_LEFT);
    unregister_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("{");
  } else if (state->count == 3) {
    SEND_STRING("}");
  }
}
void square_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("[]");
    register_code(KC_LEFT);
    unregister_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("[");
  } else if (state->count == 3) {
    SEND_STRING("]");
  }
}
void angular_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("<>");
    register_code(KC_LEFT);
    unregister_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("<");
  } else if (state->count == 3) {
    SEND_STRING(">");
  }
}
void tmux_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("tmux");
    register_code(KC_ENT);
    unregister_code(KC_ENT);
  } else if (state->count == 2) {
    register_mods(MOD_BIT(KC_LCTL));
    register_code(KC_B);
    unregister_code(KC_B);
    unregister_mods(MOD_BIT(KC_LCTL));
    register_mods(MOD_BIT(KC_LSFT));
    register_code(KC_5);
    unregister_code(KC_5);
    unregister_mods(MOD_BIT(KC_LSFT));
  }
}
void git_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("git checkout $1");
  }
}
void cmd_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_mods(MOD_BIT(KC_LCTL));
    register_mods(MOD_BIT(KC_LALT));
    register_code(KC_DELETE);
    unregister_mods(MOD_BIT(KC_LCTL));
    unregister_mods(MOD_BIT(KC_LALT));
    unregister_code(KC_DELETE);
  } else if (state->count == 2) {
    register_mods(MOD_BIT(KC_LGUI));
    register_mods(MOD_BIT(KC_LALT));
    register_code(KC_ESC);
    unregister_mods(MOD_BIT(KC_LGUI));
    unregister_mods(MOD_BIT(KC_LALT));
    unregister_code(KC_ESC);
  }
}
void cmd_sft_slash_pipe_down(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    if (state->interrupted || state->pressed == 0) {
      register_code(KC_NONUS_BACKSLASH);
    } else {
      register_code(KC_LSFT);
    }
  } else if (state->count == 2) {
    register_mods(MOD_BIT(KC_LSFT));
    register_code(KC_NONUS_BACKSLASH);
  }
}
void cmd_sft_slash_pipe_up(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    if (keyboard_report->mods & MOD_BIT(KC_LSFT)) {
      unregister_code(KC_LSFT);
    } else {
      unregister_code(KC_NONUS_BACKSLASH);
    }
  } else if (state->count == 2) {
    unregister_mods(MOD_BIT(KC_LSFT));
    unregister_code(KC_NONUS_BACKSLASH);
  }
}

tap_dance_action_t tap_dance_actions[] = {
    [CLN] = ACTION_TAP_DANCE_DOUBLE(KC_SCLN, S(KC_SCLN)),
    [QUOT] = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, S(KC_2)),
    [CAD_CAE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, cmd_dance),
    [PAR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, paranthesis_dance),
    [CUR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, curly_dance),
    [SQU] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, square_dance),
    [ANG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, angular_dance),
    [TD_TMUX] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, tmux_dance),
    [TD_GIT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, git_dance),
    [TD_SHIFTSLASHPIPE] = ACTION_TAP_DANCE_FN_ADVANCED(
        cmd_sft_slash_pipe_down, cmd_sft_slash_pipe_up, NULL),
};
