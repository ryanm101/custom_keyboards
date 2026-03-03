// Copyright 2023 ryanm101
// SPDX-License-Identifier: GPL-2.0-or-later
// Shared tap dance function implementations.
// tap_dance_actions[] is defined in each keymap.c that uses these functions.
#include QMK_KEYBOARD_H
#include "tap_dances.h"

void paranthesis_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("()");
    tap_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("(");
  } else if (state->count == 3) {
    SEND_STRING(")");
  }
}

void curly_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("{}");
    tap_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("{");
  } else if (state->count == 3) {
    SEND_STRING("}");
  }
}

void square_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("[]");
    tap_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("[");
  } else if (state->count == 3) {
    SEND_STRING("]");
  }
}

void angular_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("<>");
    tap_code(KC_LEFT);
  } else if (state->count == 2) {
    SEND_STRING("<");
  } else if (state->count == 3) {
    SEND_STRING(">");
  }
}

void tmux_dance(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    SEND_STRING("tmux");
    tap_code(KC_ENT);
  } else if (state->count == 2) {
    register_mods(MOD_BIT(KC_LCTL));
    tap_code(KC_B);
    unregister_mods(MOD_BIT(KC_LCTL));
    register_mods(MOD_BIT(KC_LSFT));
    tap_code(KC_5);
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
    register_mods(MOD_BIT(KC_LCTL) | MOD_BIT(KC_LALT));
    tap_code(KC_DELETE);
    unregister_mods(MOD_BIT(KC_LCTL) | MOD_BIT(KC_LALT));
  } else if (state->count == 2) {
    register_mods(MOD_BIT(KC_LGUI) | MOD_BIT(KC_LALT));
    tap_code(KC_ESC);
    unregister_mods(MOD_BIT(KC_LGUI) | MOD_BIT(KC_LALT));
  }
}

void cmd_sft_slash_pipe_down(tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    if (state->interrupted || !state->pressed) {
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
