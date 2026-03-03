#include QMK_KEYBOARD_H

// Layers
enum layers {
  _NAV = 0, // FreeCAD navigation / general
  _SK = 1,  // Sketcher
  _PD = 2,  // Part Design
};

// Custom keycodes for multi-key FreeCAD sequences (e.g. "V" then "F")
enum custom_keycodes {
  VC_VF = SAFE_RANGE, // View -> Fit all (FreeCAD default: V then F)
  VC_VT,              // V then T (Top)
  VC_VI,              // V then I (Isometric)
  VC_VFRT,            // V then F (Front) - if you choose to bind it so
  VC_VR,              // V then R (Right)
};

static void tap_v_then(uint16_t key) {
  tap_code(KC_V);
  tap_code(key);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (!record->event.pressed)
    return true;

  switch (keycode) {
  case VC_VF:
    tap_v_then(KC_F);
    return false;
  case VC_VT:
    tap_v_then(KC_T);
    return false;
  case VC_VI:
    tap_v_then(KC_I);
    return false;
  case VC_VR:
    tap_v_then(KC_R);
    return false;
  default:
    return true;
  }
}

/*
  Keymap goals:
  - Layer 0 (_NAV): views, fit, recompute, save, toggle visibility, delete,
  undo/redo
  - Layer 1 (_SK): sketch tools + constraints (these are placeholders; bind in
  FreeCAD)
  - Layer 2 (_PD): pad/pocket/fillet/chamfer/thickness etc (placeholders; bind
  in FreeCAD)

  IMPORTANT:
  For Sketcher/PartDesign tools, FreeCAD often has no default single-key
  shortcut. The clean approach is: 1) In FreeCAD: Tools -> Customize -> Keyboard
   2) Bind your chosen hotkeys (e.g. Ctrl+Alt+1.. etc)
   3) Map those exact chords here.
*/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* Layer 0: NAV / General
     * [ 1] Top view        [ 2] Front view      [ 3] Right view      [ 4]
     * Isometric     [ 5] Fit all [ 6] Undo            [ 7] Redo            [ 8]
     * Recompute       [ 9] Save          [10] Toggle visibility [11] Delete
     * [12] Esc             [13] Ctrl            [14] Shift         [15]
     * Momentary Sketch layer
     */
    [_NAV] =
        LAYOUT_top(KC_MUTE, KC_MUTE, KC_MUTE, VC_VT, LCTL(KC_1), VC_VR, VC_VI,
                   VC_VF, LCTL(KC_Z), LCTL(KC_Y), LCTL(KC_R), LCTL(KC_S), KC_V,
                   KC_DEL, KC_ESC, KC_LCTL, KC_LSFT, MO(_SK)),

    /* Layer 1: SKETCHER
     * Suggestion: bind these in FreeCAD to your preferred sketch
     * tools/constraints. Here I'm using Ctrl+Alt+number as an example
     * convention.
     *
     * Row 1: line/rect/circle/arc/trim
     * Row 2: horiz/vert/parallel/perp/dimension
     * Row 3: toggle construction / toggle constraints / esc / momentary PD /
     * back to NAV (TG)
     */
    [_SK] = LAYOUT_top(
        KC_TRNS, KC_TRNS, KC_TRNS, LCTL(LALT(KC_1)), LCTL(LALT(KC_2)),
        LCTL(LALT(KC_3)), LCTL(LALT(KC_4)), LCTL(LALT(KC_5)), LCTL(LALT(KC_6)),
        LCTL(LALT(KC_7)), LCTL(LALT(KC_8)), LCTL(LALT(KC_9)), LCTL(LALT(KC_0)),
        LCTL(LALT(KC_C)), LCTL(LALT(KC_T)), KC_ESC, MO(_PD), TG(_NAV)),

    /* Layer 2: PART DESIGN
     * Row 1: pad/pocket/fillet/chamfer/thickness
     * Row 2: datum plane / polar pattern / linear pattern / mirror / draft
     * Row 3: recompute / save / esc / momentary SK / back to NAV
     */
    [_PD] = LAYOUT_top(KC_TRNS, KC_TRNS, KC_TRNS, LCTL(LALT(KC_Q)),
                       LCTL(LALT(KC_W)), LCTL(LALT(KC_E)), LCTL(LALT(KC_R)),
                       LCTL(LALT(KC_T)), LCTL(LALT(KC_A)), LCTL(LALT(KC_S)),
                       LCTL(LALT(KC_D)), LCTL(LALT(KC_F)), LCTL(LALT(KC_G)),
                       LCTL(KC_R), LCTL(KC_S), KC_ESC, MO(_SK), TG(_NAV)),
};

// Encoders: 3 encoders
bool encoder_update_user(uint8_t index, bool clockwise) {
  uint8_t layer = get_highest_layer(layer_state);

  switch (index) {
  case 0:
    // Encoder 0: Zoom (wheel)
    if (clockwise) {
      tap_code16(QK_MOUSE_WHEEL_UP);
    } else {
      tap_code16(QK_MOUSE_WHEEL_DOWN);
    }
    return false;

  case 1:
    // Encoder 1: Context sensitive
    if (layer == _SK) {
      // Sketcher: nudge active numeric field (constraint value etc.)
      tap_code16(clockwise ? KC_UP : KC_DOWN);
    } else {
      // General: Undo / Redo
      tap_code16(clockwise ? LCTL(KC_Y) : LCTL(KC_Z));
    }
    return false;

  case 2:
    // Encoder 2: Pan left/right (horizontal scroll)
    if (clockwise) {
      tap_code16(QK_MOUSE_WHEEL_RIGHT);
    } else {
      tap_code16(QK_MOUSE_WHEEL_LEFT);
    }
    return false;
  }

  return true;
}
