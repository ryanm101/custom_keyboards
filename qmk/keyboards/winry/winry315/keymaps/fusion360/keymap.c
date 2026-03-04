#include QMK_KEYBOARD_H

// Layers
enum layers {
  _NAV = 0, // Navigation / general Fusion 360 operations
  _SK = 1,  // Sketch environment
  _MD = 2,  // Solid / Model environment
};

/*
  LAYOUT_top key positions (18 keys, 3 encoders)
  ┌──────────────┬──────────────────────┬──────────────┐
  │  Enc0 (zoom) │  Enc1 (orbit)        │  Enc2 (pan)  │
  ├──┬──┬──┬──┬──┴──┬──┬──┬──┬──┬──┬──┤
  │  row 2 (5 keys)                     │
  │  row 3 (5 keys)                     │
  │  row 4 (5 keys)                     │
  └────────────────────────────────────┘

  Fusion 360 navigation (default preset):
    Scroll         → Zoom
    Shift + Scroll → Orbit
    Mid-btn + drag → Pan   (Enc2 simulates via Shift+mid-click-drag keybind)

  Layer colours suggested for KLE:
    NAV (#e8d5b7 warm sand) → SK (#4a90d9 blue) → MD (#6abf6a green)
*/

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  /* ── Layer 0: NAV (general Fusion 360) ──────────────────────────────────
   *  Enc press │  Fit All (F6)    Zoom Fit (F6)   Home view
   *  Row 2     │  Front(1)  Top   Right   Home(F6)  Zoom-Fit
   *  Row 3     │  Undo     Redo   Save    Compute   Delete
   *  Row 4     │  Esc      Spc    Ctrl    Shift     ▷Sketch
   *
   *  Encoder rotation:
   *    Enc0 CW/CCW  → Zoom in / out   (mouse wheel)
   *    Enc1 CW/CCW  → Orbit CW / CCW  (Shift + mouse wheel)
   *    Enc2 CW/CCW  → Pan right / left (Shift + middle-click mapped;
   *                                     set Fusion nav preset to "PowerMill"
   *                                     and Shift+MMB pans, or keep default
   *                                     and this sends H-scroll as fallback)
   */
  [_NAV] = LAYOUT_top(
    /* enc buttons */  KC_F6,       KC_F6,      KC_HOME,
    /* row 2 views */  KC_KP_1,     KC_KP_7,    KC_KP_3,    KC_F6,       LCTL(KC_S),
    /* row 3 edit  */  LCTL(KC_Z),  LCTL(KC_Y), LCTL(KC_S), LCTL(KC_B),  KC_DEL,
    /* row 4 ctrl  */  KC_ESC,      KC_SPC,     KC_LCTL,    KC_LSFT,     MO(_SK)
  ),

  /* ── Layer 1: SKETCH ─────────────────────────────────────────────────────
   *  Enc press │  (transparent)
   *  Row 2     │  Line   Circle  Rect    Project  Dimension
   *  Row 3     │  Trim   Offset  Constr  Mirror   Fillet(sk)
   *  Row 4     │  Esc    Finish  Ctrl    Shift    ▷Model / ◀NAV
   *
   *  Fusion 360 single-key sketch shortcuts (default):
   *    L = Line            C = Circle         R = Rectangle
   *    P = Project         D = Dimension
   *    T = Trim            O = Offset         X = Construction
   *    F = Fillet          (Mirror has no default 1-key; use S shortcut bar)
   */
  [_SK] = LAYOUT_top(
    /* enc buttons */  KC_TRNS,  KC_TRNS,  KC_TRNS,
    /* row 2 draw  */  KC_L,     KC_C,     KC_R,     KC_P,      KC_D,
    /* row 3 constr */ KC_T,     KC_O,     KC_X,     KC_S,      KC_F,
    /* row 4 misc  */  KC_ESC,   KC_Q,     KC_LCTL,  KC_LSFT,   MO(_MD)
  ),

  /* ── Layer 2: MODEL / SOLID ──────────────────────────────────────────────
   *  Enc press │  (transparent)
   *  Row 2     │  Extrude  Hole    Fillet   Press-Pull  Move
   *  Row 3     │  Joint    Appear  Measure  Compute     Delete
   *  Row 4     │  Esc      Spc     Ctrl     Shift       ◀NAV (TG off)
   *
   *  Fusion 360 default modeling shortcuts:
   *    E = Extrude     H = Hole       F = Fillet
   *    Q = Press-Pull  M = Move       J = Joint
   *    A = Appearance  I = Measure    Ctrl+B = Compute
   */
  [_MD] = LAYOUT_top(
    /* enc buttons */  KC_TRNS,     KC_TRNS,    KC_TRNS,
    /* row 2 ops   */  KC_E,        KC_H,       KC_F,         KC_Q,        KC_M,
    /* row 3 util  */  KC_J,        KC_A,       KC_I,         LCTL(KC_B),  KC_DEL,
    /* row 4 ctrl  */  KC_ESC,      KC_SPC,     KC_LCTL,      KC_LSFT,     TG(_SK)
  ),
};
// clang-format on

// ── Encoders ─────────────────────────────────────────────────────────────────
//
//  Enc0 (left)   → Zoom:  scroll wheel up/down
//  Enc1 (centre) → Orbit: Shift + scroll (Fusion default orbit control)
//  Enc2 (right)  → Pan:   horizontal mouse scroll
//                         Works out-of-the-box with the "PowerMill" navigation
//                         preset (Shift+MMB pans).  With the default preset,
//                         switch Fusion's navigation to PowerMill or remap
//                         horizontal scroll to pan in your OS/driver.
//
bool encoder_update_user(uint8_t index, bool clockwise) {
  switch (index) {
  case 0:
    // Enc0: Zoom
    tap_code16(clockwise ? QK_MOUSE_WHEEL_UP : QK_MOUSE_WHEEL_DOWN);
    return false;

  case 1:
    // Enc1: Orbit — Shift + scroll (Fusion 360 default orbit)
    tap_code16(clockwise ? S(QK_MOUSE_WHEEL_UP) : S(QK_MOUSE_WHEEL_DOWN));
    return false;

  case 2:
    // Enc2: Pan — horizontal scroll
    // For the default Fusion nav preset: set "Pan, Zoom, Orbit Shortcuts" to
    // "PowerMill" in Fusion Preferences → General.  With PowerMill, Shift+MMB
    // pans. Alternatively, H-scroll (Button 6/7) pans in some OS scroll
    // drivers.  Both signals are sent here (H-scroll is the primary output).
    tap_code16(clockwise ? QK_MOUSE_WHEEL_RIGHT : QK_MOUSE_WHEEL_LEFT);
    return false;
  }

  return true;
}
