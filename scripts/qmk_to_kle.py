#!/usr/bin/env python3
"""
qmk_to_kle.py — Convert a QMK keyboard.json / info.json to a
keyboard-layout-editor.com JSON array.

Usage:
  qmk_to_kle.py <keyboard.json> <output.json> [LAYOUT_name]

  keyboard.json   QMK keyboard.json or info.json (must contain 'layouts')
  output.json     Path to write the KLE JSON array
  LAYOUT_name     Optional specific layout key, e.g. LAYOUT_iso_85
"""

import json, sys

# ---------------------------------------------------------------------------
# Row grouping
# ---------------------------------------------------------------------------
# Keys with the same rounded Y (within ROW_TOLERANCE) are placed on the same
# KLE row. After sorting left‑to‑right within each row, the converter emits
# property objects ({x, y, w, h}) only when values differ from the KLE cursor.

ROW_TOLERANCE = 0.4   # keys within this Y distance share a row


def _group_rows(layout_data):
    """Return a list of rows, each row = list of key dicts sorted by x."""
    # Sort all keys by Y first, then X
    keys = sorted(layout_data, key=lambda k: (k.get("y", 0), k.get("x", 0)))

    rows: list[list[dict]] = []
    current_row: list[dict] = []
    row_y: float | None = None

    for key in keys:
        y = key.get("y", 0)
        if row_y is None or abs(y - row_y) <= ROW_TOLERANCE:
            current_row.append(key)
            if row_y is None:
                row_y = y
        else:
            if current_row:
                rows.append(sorted(current_row, key=lambda k: k.get("x", 0)))
            current_row = [key]
            row_y = y

    if current_row:
        rows.append(sorted(current_row, key=lambda k: k.get("x", 0)))

    return rows


def _label(key):
    """Human‑readable label: explicit label > R,C matrix notation."""
    lbl = key.get("label", None)
    if lbl is not None:
        return str(lbl)
    m = key.get("matrix", None)
    if m:
        return f"R{m[0]},C{m[1]}"
    return "?"


def generate_kle(layout_data):
    """
    Convert a QMK layout array to a KLE JSON array.

    KLE row format:  [ {props}, "Label", {props}, "Label", ... ]
    KLE cursor state tracks current (x, y, w, h); props are only included
    when they differ from the default / current state.
    """
    rows = _group_rows(layout_data)
    if not rows:
        return []

    kle = []
    # KLE implicit cursor: after each row the cursor moves to x=0, y+=1
    kle_y = 0.0   # KLE's notion of current row y (0‑indexed, integer steps)

    for row_idx, row in enumerate(rows):
        # The physical Y of this row.
        phys_y = row[0].get("y", 0)

        kle_row = []
        kle_x = 0.0   # KLE cursor x resets to 0 at start of each row

        for key_idx, key in enumerate(row):
            x = key.get("x", 0)
            y = key.get("y", 0)
            w = round(key.get("w", 1), 4)
            h = round(key.get("h", 1), 4)

            props = {}

            # Y offset: only the first key in a row can carry y.
            # KLE advances y by 1 for each new array. We need to account
            # for the gap between expected KLE y and actual physical y.
            if key_idx == 0:
                expected_kle_y = kle_y + (1 if row_idx > 0 else 0)
                dy = round(phys_y - expected_kle_y, 4)
                if abs(dy) > 0.01:
                    props["y"] = dy
                kle_y = phys_y

            # X gap
            dx = round(x - kle_x, 4)
            if abs(dx) > 0.01:
                props["x"] = dx

            if w != 1.0:
                props["w"] = w
            if h != 1.0:
                props["h"] = h

            if props:
                kle_row.append(props)
            kle_row.append(_label(key))

            kle_x = x + w

        kle.append(kle_row)

    return kle


def process(file_path, out_path, layout_name=None):
    with open(file_path, "r") as f:
        data = json.load(f)

    if "layouts" not in data:
        print(f"No layouts found in {file_path}")
        return

    if layout_name and layout_name in data["layouts"]:
        layout_key = layout_name
    else:
        layout_key = next(iter(data["layouts"]))
        if "LAYOUT_all" in data["layouts"]:
            layout_key = "LAYOUT_all"

    layout_data = data["layouts"][layout_key]["layout"]
    kle = generate_kle(layout_data)

    with open(out_path, "w") as f:
        json.dump(kle, f, indent=4)
    print(f"Generated {out_path}  (layout: {layout_key}, {len(layout_data)} keys)")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    layout_name = sys.argv[3] if len(sys.argv) > 3 else None
    process(sys.argv[1], sys.argv[2], layout_name)
