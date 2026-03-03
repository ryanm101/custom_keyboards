# SPDX-License-Identifier: GPL-2.0-or-later
# Shared userspace — auto-included by QMK when keymap name matches this directory.
# Only compile tap_dances.c for keyboards that opt in with TAP_DANCE_ENABLE = yes.
ifeq ($(TAP_DANCE_ENABLE), yes)
    SRC += tap_dances.c
endif
