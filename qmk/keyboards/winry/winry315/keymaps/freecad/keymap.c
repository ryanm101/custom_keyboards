#include QMK_KEYBOARD_H

// Layers
enum layers {
  _NAV = 0, // Navigation / general FreeCAD operations
  _SK = 1,  // Sketcher tools and constraints
  _PD = 2,  // Part Design features
};

// Custom keycodes for FreeCAD "V → key" view sequences
enum custom_keycodes {
  VC_VF = SAFE_RANGE, // View → Fit All      (V, F)
  VC_VT,              // View → Top           (V, T)
  VC_VI,              // View → Isometric     (V, I)
  VC_VR,              // View → Right         (V, R)
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
  LAYOUT_top key positions (18 keys total)
  ┌──────────────┬──────────────────────┬──────────────┐
  │  Enc0 (zoom) │ Enc1 (undo/redo, ↕) │ Enc2 (pan ↔) │  ← press knob =
  key[0..2] ├──┬──┬──┬──┬──┴──┬──┬──┬──┬──┬──┬──┤ │  5 button keys (row 2) │ │
  5 button keys (row 3)             │ │  5 button keys (row 4)             │
  └────────────────────────────────────┘

  Layer colours used in KLE output:
    NAV (#e1dbd1 cream)  →  SK (#336699 blue)  →  PD (#57c942 green)
*/

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  /* ── Layer 0: NAV (general FreeCAD) ─────────────────────────────────────
   *  Enc press │  Fit All        Recompute       Toggle Vis (Space)
   *  Row 2     │  Top  Front(1)  Right   Iso     Fit All
   *  Row 3     │  Undo  Redo    Recompute  Save  Delete
   *  Row 4     │  Esc   Spc(Vis) Ctrl    Shift   ▷Sketcher
   *
   *  Encoder rotation:
   *    Enc0 CW/CCW  → Zoom in / out   (mouse scroll)
   *    Enc1 CW/CCW  → Redo / Undo     (Ctrl+Y / Ctrl+Z)
   *    Enc2 CW/CCW  → Pan right / left (Shift+scroll up/down)
   */
  [_NAV] = LAYOUT_top(
    /* enc buttons */  VC_VF,      LCTL(KC_R), KC_SPC,
    /* row 2 views */  VC_VT,      KC_KP_1,    VC_VR,      VC_VI,   VC_VF,
    /* row 3 edit  */  LCTL(KC_Z), LCTL(KC_Y), LCTL(KC_R), LCTL(KC_S), KC_DEL,
    /* row 4 ctrl  */  KC_ESC,     KC_SPC,     KC_LCTL,    KC_LSFT, MO(_SK)
  ),

  /* ── Layer 1: SKETCHER ───────────────────────────────────────────────────
   *  Enc press │  (transparent — falls back to NAV)
   *  Row 2     │  Line   Circle  Arc     Rect    Point
   *  Row 3     │  Horiz  Vert    Coinc   Fix     Tangent
   *  Row 4     │  Constr-Toggle  Trim    CloseSketch  ▷PD  ◀NAV
   *
   *  Enc1 CW/CCW on this layer → nudge numeric constraint field (↑/↓)
   *
   *  FreeCAD setup:  Tools → Customize → Keyboard
   *  Bind Ctrl+Alt+<key> to the Sketcher command shown:
   *    L = Sketcher_CreateLine         C = Sketcher_CreateCircle
   *    A = Sketcher_CreateArc          R = Sketcher_CreateRectangle
   *    P = Sketcher_CreatePoint        H = Sketcher_ConstrainHorizontal
   *    V = Sketcher_ConstrainVertical  O = Sketcher_ConstrainCoincident
   *    X = Sketcher_ConstrainLock      T = Sketcher_ConstrainTangent
   *    Q = Sketcher_ToggleConstruction E = Sketcher_Trimming
   *    W = Sketcher_LeaveSketch        (close / finish sketch)
   */
  [_SK] = LAYOUT_top(
    /* enc buttons */  KC_TRNS,           KC_TRNS,           KC_TRNS,
    /* row 2 draw  */  LCTL(LALT(KC_L)),  LCTL(LALT(KC_C)),  LCTL(LALT(KC_A)),  LCTL(LALT(KC_R)),  LCTL(LALT(KC_P)),
    /* row 3 constr */ LCTL(LALT(KC_H)),  LCTL(LALT(KC_V)),  LCTL(LALT(KC_O)),  LCTL(LALT(KC_X)),  LCTL(LALT(KC_T)),
    /* row 4 misc  */  LCTL(LALT(KC_Q)),  LCTL(LALT(KC_E)),  LCTL(LALT(KC_W)),  MO(_PD),            TG(_NAV)
  ),

  /* ── Layer 2: PART DESIGN ────────────────────────────────────────────────
   *  Enc press │  (transparent — falls back to NAV)
   *  Row 2     │  Pad    Pocket  Revolve Fillet  Chamfer
   *  Row 3     │  Datum  LinPat  PolarPat Mirror  Thickness
   *  Row 4     │  Recompute  Save  Esc  ▷SK  ◀NAV
   *
   *  FreeCAD setup:  Tools → Customize → Keyboard
   *  Bind Ctrl+Alt+<key> to the Part Design command shown:
   *    P = PartDesign_Pad              O = PartDesign_Pocket
   *    V = PartDesign_Revolution       F = PartDesign_Fillet
   *    C = PartDesign_Chamfer          D = PartDesign_Plane (datum)
   *    L = PartDesign_LinearPattern    Z = PartDesign_PolarPattern
   *    M = PartDesign_Mirrored         T = PartDesign_Thickness
   */
  [_PD] = LAYOUT_top(
    /* enc buttons */  KC_TRNS,           KC_TRNS,           KC_TRNS,
    /* row 2 feats */  LCTL(LALT(KC_P)),  LCTL(LALT(KC_O)),  LCTL(LALT(KC_V)),  LCTL(LALT(KC_F)),  LCTL(LALT(KC_C)),
    /* row 3 ops   */  LCTL(LALT(KC_D)),  LCTL(LALT(KC_L)),  LCTL(LALT(KC_Z)),  LCTL(LALT(KC_M)),  LCTL(LALT(KC_T)),
    /* row 4 ctrl  */  LCTL(KC_R),        LCTL(KC_S),        KC_ESC,            MO(_SK),            TG(_NAV)
  ),
};
// clang-format on

// Encoders — 3 encoders, rotation behaviour is context-sensitive per layer
bool encoder_update_user(uint8_t index, bool clockwise) {
  uint8_t layer = get_highest_layer(layer_state);

  switch (index) {
  case 0:
    // Enc0 (left knob): Zoom — mouse wheel up/down
    tap_code16(clockwise ? QK_MOUSE_WHEEL_UP : QK_MOUSE_WHEEL_DOWN);
    return false;

  case 1:
    // Enc1 (centre knob):
    //   Sketcher → nudge active numeric constraint field (↑ / ↓)
    //   Elsewhere → Redo / Undo  (Ctrl+Y / Ctrl+Z; FreeCAD uses Ctrl+Y for
    //   redo)
    if (layer == _SK) {
      tap_code16(clockwise ? KC_UP : KC_DOWN);
    } else {
      tap_code16(clockwise ? LCTL(KC_Y) : LCTL(KC_Z));
    }
    return false;

  case 2:
    // Enc2 (right knob): Pan — Shift+scroll (FreeCAD default pan binding)
    tap_code16(clockwise ? S(QK_MOUSE_WHEEL_UP) : S(QK_MOUSE_WHEEL_DOWN));
    return false;
  }

  return true;
}
