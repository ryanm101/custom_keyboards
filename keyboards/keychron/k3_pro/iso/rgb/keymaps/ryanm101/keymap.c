// Copyright 2023 ryanm101
// SPDX-License-Identifier: GPL-2.0-or-later

// Keychron K3 Pro ISO RGB — ryanm101 keymap
// Function implementations live in users/ryanm101/tap_dances.c (compiled via
// rules.mk).

#include QMK_KEYBOARD_H

// ── Tap Dance ────────────────────────────────────────────────────────────────
// Inline enum + aliases so keymap_introspection.c can resolve them regardless
// of how the Keychron fork sets up TAP_DANCE_ENABLE at its include point.
enum {
  CLN = 0,
  QUOT,
  CAD_CAE,
  PAR,
  CUR,
  SQU,
  ANG,
  TD_TMUX,
};

// Fallback for QMK 0.14 introspection which runs without TAP_DANCE_ENABLE
#ifndef TD
#define TD(n) (QK_TAP_DANCE | ((n) & 0xFF))
#endif

#define COLON TD(CLN)
#define QUOTE TD(QUOT)
#define CADCAE TD(CAD_CAE)
#define PARAN TD(PAR)
#define CURLY TD(CUR)
#define SQUAR TD(SQU)
#define ANGUL TD(ANG)
#define TMUX TD(TD_TMUX)

// Forward declarations — implementations in users/ryanm101/tap_dances.c
#ifdef TAP_DANCE_ENABLE
void paranthesis_dance(tap_dance_state_t *state, void *user_data);
void curly_dance(tap_dance_state_t *state, void *user_data);
void square_dance(tap_dance_state_t *state, void *user_data);
void angular_dance(tap_dance_state_t *state, void *user_data);
void tmux_dance(tap_dance_state_t *state, void *user_data);
void cmd_dance(tap_dance_state_t *state, void *user_data);
#endif

// ── Layers ───────────────────────────────────────────────────────────────────
enum layers { MAC_BASE, MAC_FN, WIN_BASE, WIN_FN };

#include "shared_macros.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_iso_85(
        KC_ESC, KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, KC_NO, KC_NO, KC_MPRV,
        KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_SNAP, KC_DEL, RGB_MOD,
        KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
        KC_MINS, KC_EQL, KC_BSPC, KC_PGUP, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T,
        KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_ENT, KC_PGDN,
        KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, COLON,
        QUOTE, KC_BSLS, KC_HOME, KC_LSFT, KC_NUBS, KC_Z, KC_X, KC_C, KC_V, KC_B,
        KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, KC_END, KC_LCTL,
        KC_LOPTN, KC_LCMMD, KC_SPC, KC_RCMMD, MO(MAC_FN), KC_RCTL, KC_LEFT,
        KC_DOWN, KC_RGHT),

    [MAC_FN] = LAYOUT_iso_85(
        KC_TRNS, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9,
        KC_F10, KC_F11, KC_F12, KC_TRNS, KC_TRNS, RGB_TOG, KC_TRNS, BT_HST1,
        BT_HST2, BT_HST3, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_TOG, RGB_MOD, RGB_VAI,
        RGB_HUI, RGB_SAI, RGB_SPI, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_RMOD, RGB_VAD, RGB_HUD,
        RGB_SAD, RGB_SPD, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        BAT_LVL, NK_TOGG, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS),

    [WIN_BASE] = LAYOUT_iso_85(
        KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9,
        KC_F10, KC_F11, KC_F12, KC_PSCR, KC_DEL, RGB_MOD, KC_GRV, KC_1, KC_2,
        KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL,
        KC_BSPC, KC_PGUP, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U,
        KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_ENT, KC_PGDN, KC_CAPS, KC_A,
        KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, COLON, QUOTE, KC_BSLS,
        KC_HOME, KC_LSFT, KC_NUBS, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M,
        KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, KC_END, KC_LCTL, KC_LGUI,
        KC_LALT, KC_SPC, KC_RALT, MO(WIN_FN), KC_RCTL, KC_LEFT, KC_DOWN,
        KC_RGHT),

    [WIN_FN] = LAYOUT_iso_85(
        KC_TRNS, KC_BRID, KC_BRIU, KC_TASK, KC_FILE, RGB_VAD, RGB_VAI, KC_MPRV,
        KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_TRNS, KC_TRNS, RGB_TOG,
        KC_TRNS, BT_HST1, BT_HST2, BT_HST3, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_TOG,
        RGB_MOD, RGB_VAI, RGB_HUI, RGB_SAI, RGB_SPI, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_RMOD,
        RGB_VAD, RGB_HUD, RGB_SAD, RGB_SPD, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, BAT_LVL, NK_TOGG, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
};

#ifdef TAP_DANCE_ENABLE
tap_dance_action_t tap_dance_actions[] = {
    [CLN] = ACTION_TAP_DANCE_DOUBLE(KC_SCLN, S(KC_SCLN)),
    [QUOT] = ACTION_TAP_DANCE_DOUBLE(KC_QUOT, S(KC_2)),
    [CAD_CAE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, cmd_dance),
    [PAR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, paranthesis_dance),
    [CUR] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, curly_dance),
    [SQU] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, square_dance),
    [ANG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, angular_dance),
    [TD_TMUX] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, tmux_dance),
};
#endif
