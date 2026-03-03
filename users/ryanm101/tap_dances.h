// Copyright 2023 ryanm101
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// Tap dance enum — just integers, no QMK type dependencies.
// TD_COUNT is the sentinel; keep it last.
enum ryanm101_tap_dances {
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
  TD_COUNT,
};

// Aliases — TD(x) is a plain integer macro (QK_TAP_DANCE | (x & 0xFF)),
// available via QMK_KEYBOARD_H without needing TAP_DANCE_ENABLE defined.
// Include QMK_KEYBOARD_H in keymap.c before this header.
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

// Function prototypes — implemented in tap_dances.c (compiled via
// users/ryanm101/rules.mk). tap_dance_actions[] must be defined in each
// keymap.c that uses tap dances.
void paranthesis_dance(tap_dance_state_t *state, void *user_data);
void curly_dance(tap_dance_state_t *state, void *user_data);
void square_dance(tap_dance_state_t *state, void *user_data);
void angular_dance(tap_dance_state_t *state, void *user_data);
void tmux_dance(tap_dance_state_t *state, void *user_data);
void git_dance(tap_dance_state_t *state, void *user_data);
void cmd_dance(tap_dance_state_t *state, void *user_data);
void cmd_sft_slash_pipe_down(tap_dance_state_t *state, void *user_data);
void cmd_sft_slash_pipe_up(tap_dance_state_t *state, void *user_data);
