#!/usr/bin/env python3
"""
qmk_to_kle.py — Convert a QMK keymap.c + keyboard.json to a single
keyboard-layout-editor.com JSON array with all layers shown as multi-legend
labels (layer 0 label \\n layer 1 label \\n ...).

Usage:
  qmk_to_kle.py <keyboard.json> <keymap.c> <output.json> [LAYOUT_name]

  keyboard.json   QMK keyboard.json / info.json (must contain 'layouts')
  keymap.c        QMK keymap C source file
  output.json     Path to write the KLE JSON array
  LAYOUT_name     Optional specific layout key (e.g. LAYOUT_left).
                  Defaults to the first layout referenced in keymap.c.
"""

import json
import os
import re
import sys

# ---------------------------------------------------------------------------
# Row-grouping config
# ---------------------------------------------------------------------------
ROW_TOLERANCE = 0.4   # keys within this Y distance share a KLE row


# ---------------------------------------------------------------------------
# Label simplification
# ---------------------------------------------------------------------------

_PRETTY: dict[str, str] = {
    "TRNS": "▽",
    "_______": "▽",
    "NO": "✗",
    "XXXXXXX": "✗",
    "BSPC": "⌫",
    "DEL": "⌦",
    "ENT": "↵",
    "ENTER": "↵",
    "ESC": "Esc",
    "SPC": "Spc",
    "TAB": "Tab",
    "CAPS": "Caps",
    "LSFT": "Shift",
    "RSFT": "Shift",
    "LCTL": "Ctrl",
    "RCTL": "Ctrl",
    "LALT": "Alt",
    "RALT": "AltGr",
    "LGUI": "Win",
    "RGUI": "Win",
    "APP": "Menu",
    "LEFT": "←",
    "RIGHT": "→",
    "UP": "↑",
    "DOWN": "↓",
    "PGUP": "PgUp",
    "PGDN": "PgDn",
    "HOME": "Home",
    "END": "End",
    "INS": "Ins",
    "PSCR": "PrtSc",
    "SYRQ": "SysRq",
    "NUM": "NumLk",
    "MUTE": "Mute",
    "VOLU": "Vol+",
    "VOLD": "Vol-",
    "MPLY": "Play",
    "MNXT": "Next",
    "MPRV": "Prev",
    "COMM": ",",
    "DOT": ".",
    "SLSH": "/",
    "BSLS": "\\",
    "MINS": "-",
    "EQL": "=",
    "LBRC": "[",
    "RBRC": "]",
    "SCLN": ";",
    "QUOT": "'",
    "GRV": "`",
    "NUHS": "#",
    "NONUS_BACKSLASH": "|\\",
    "KP_0": "KP0", "KP_1": "KP1", "KP_2": "KP2", "KP_3": "KP3",
    "KP_4": "KP4", "KP_5": "KP5", "KP_6": "KP6", "KP_7": "KP7",
    "KP_8": "KP8", "KP_9": "KP9",
    "KP_DOT": "KP.", "KP_ENTER": "KP↵", "KP_PLUS": "KP+",
    "KP_MINUS": "KP-", "KP_ASTERISK": "KP*", "KP_SLASH": "KP/",
}


def _pretty_label(raw: str) -> str:
    """Produce a readable KLE label from a raw QMK keycode string."""
    raw = raw.strip()

    # Transparent / no-op
    if raw in ("_______", "KC_TRNS"):
        return "▽"
    if raw in ("XXXXXXX", "KC_NO"):
        return "✗"

    # MO / TG / LT
    m = re.fullmatch(r"MO\((_?\w+)\)", raw)
    if m:
        return f"MO({m.group(1)})"
    m = re.fullmatch(r"TG\((_?\w+)\)", raw)
    if m:
        return f"TG({m.group(1)})"
    m = re.fullmatch(r"LT\((_?\w+),\s*(KC_\w+|\w+)\)", raw)
    if m:
        inner = m.group(2).removeprefix("KC_")
        return f"LT({m.group(1)}/{inner})"

    # LCTL(KC_x) / LSFT(...) / S(...)
    m = re.fullmatch(r"(LCTL|LSFT|RCTL|RSFT|S|ALGR_T)\(KC_(\w+)\)", raw)
    if m:
        mod = {"LCTL": "^", "LSFT": "⇧", "RCTL": "^", "RSFT": "⇧",
               "S": "⇧", "ALGR_T": "AGr"}.get(m.group(1), m.group(1))
        return f"{mod}{m.group(2)}"

    # LCTL(LALT(KC_x))
    m = re.fullmatch(r"LCTL\(LALT\(KC_(\w+)\)\)", raw)
    if m:
        return f"^Alt{m.group(1)}"

    # KC_ prefix
    if raw.startswith("KC_"):
        short = raw[3:]
        return _PRETTY.get(short, short)

    # UG_ / RM_ / BL_ / RGB_M_ — already short
    for prefix in ("UG_", "RM_", "BL_", "RGB_M_"):
        if raw.startswith(prefix):
            return raw

    return _PRETTY.get(raw, raw)


# ---------------------------------------------------------------------------
# C keymap parser
# ---------------------------------------------------------------------------

def _remove_c_comments(src: str) -> str:
    src = re.sub(r"/\*.*?\*/", " ", src, flags=re.DOTALL)
    src = re.sub(r"//[^\n]*", "", src)
    return src


def _split_args(s: str) -> list[str]:
    """Split a comma-separated argument string respecting nested parens."""
    args: list[str] = []
    current: list[str] = []
    depth = 0
    for ch in s:
        if ch == "(" :
            depth += 1
            current.append(ch)
        elif ch == ")":
            depth -= 1
            current.append(ch)
        elif ch == "," and depth == 0:
            token = "".join(current).strip()
            if token:
                args.append(token)
            current = []
        else:
            current.append(ch)
    token = "".join(current).strip()
    if token:
        args.append(token)
    return args


def _parse_layers(keymap_src: str) -> list[tuple[str, str, list[str]]]:
    """
    Return [(layer_name, layout_macro, [keycodes…]), …] from a keymap.c.
    """
    src = _remove_c_comments(keymap_src)

    m = re.search(r"keymaps\s*\[.*?\]\s*=\s*\{(.*)\}", src, re.DOTALL)
    if not m:
        raise ValueError("Could not find keymaps[] initialiser in keymap.c")
    body = m.group(1)

    layers = []
    pattern = re.compile(r"\[\s*(_?\w+)\s*\]\s*=\s*(LAYOUT_\w+)\s*\(", re.ASCII)

    for match in pattern.finditer(body):
        layer_name = match.group(1)
        layout_name = match.group(2)
        start = match.end()

        depth = 1
        i = start
        while i < len(body) and depth > 0:
            if body[i] == "(":
                depth += 1
            elif body[i] == ")":
                depth -= 1
            i += 1
        args_str = body[start: i - 1]
        layers.append((layer_name, layout_name, _split_args(args_str)))

    return layers


# ---------------------------------------------------------------------------
# Geometry helpers
# ---------------------------------------------------------------------------

def _group_rows(layout_data: list[dict]) -> list[list[dict]]:
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


# Colours applied per layer in the stacked representation.
# Layer 0 uses the first colour; each subsequent layer gets the next one.
LAYER_COLORS = [
    "#e1dbd1",   # 0: cream  (standard key)
    "#336699",   # 1: blue   (fn)
    "#57c942",   # 2: green  (fn2)
    "#fd5151",   # 3: red    (fn3 / special)
    "#84cbcb",   # 4: teal
    "#7979ba",   # 5: purple
    "#848bcb",   # 6: lavender
]


def generate_kle(layout_data: list[dict], layers: list[list[str]]) -> list:
    """
    Build a KLE JSON array using the *stacked* multi-layer representation.

    Each QMK layer is rendered as a full copy of the keyboard geometry placed
    below the previous copy, separated by an extra {y:1} gap.  Keys on layer
    N>0 that are transparent (▽) are rendered as "" (blank in KLE).  Each
    layer group is given a distinct background colour from LAYER_COLORS.

    This matches the format shown in the user's reference KLE layouts.
    """
    rows = _group_rows(layout_data)
    if not rows:
        return []

    n_layers = len(layers)
    orig_idx = {id(k): i for i, k in enumerate(layout_data)}

    # Precompute per-row y-deltas from the layer-0 geometry.
    # row_dys[0] = initial y offset of the first row (relative to kle y=0).
    # row_dys[i] = extra gap beyond 1 unit between row i-1 and row i.
    row_phys_ys = [min(k.get("y", 0) for k in row) for row in rows]
    row_dys: list[float] = []
    for i, py in enumerate(row_phys_ys):
        if i == 0:
            row_dys.append(round(py, 4))
        else:
            row_dys.append(round(py - row_phys_ys[i - 1] - 1.0, 4))

    kle: list = []
    kle_y = 0.0   # tracks the physical y the KLE cursor last landed on

    for layer_idx in range(n_layers):
        color = LAYER_COLORS[layer_idx % len(LAYER_COLORS)]
        label_list = layers[layer_idx]

        for row_idx, row in enumerate(rows):
            kle_row: list = []
            kle_x = 0.0

            # --- y delta for this row ---
            if layer_idx == 0 and row_idx == 0:
                # Very first row: no auto-advance yet.
                dy = row_dys[0]
                kle_y = dy
            else:
                # KLE auto-advances y by 1 between rows; we add the delta on top.
                extra = 1.0 if (layer_idx > 0 and row_idx == 0) else 0.0
                dy = row_dys[row_idx] + extra
                kle_y = kle_y + 1.0 + dy

            # --- first-key props (y, color, x) ---
            row_start_props: dict = {}
            if abs(dy) > 0.001:
                row_start_props["y"] = round(dy, 4)
            # Set colour on the first key of every row (KLE persists until changed,
            # but being explicit avoids bleed-through from previous rows).
            row_start_props["c"] = color

            # --- emit keys ---
            for key_idx, key in enumerate(row):
                x = key.get("x", 0)
                w = round(key.get("w", 1), 4)
                h = round(key.get("h", 1), 4)

                props: dict = {}
                if key_idx == 0:
                    props.update(row_start_props)

                dx = round(x - kle_x, 4)
                if abs(dx) > 0.001:
                    props["x"] = dx
                if w != 1.0:
                    props["w"] = w
                if h != 1.0:
                    props["h"] = h

                if props:
                    kle_row.append(props)

                ki = orig_idx[id(key)]
                label = label_list[ki]
                # Transparent / blocked keys shown blank on non-base layers
                if layer_idx > 0 and label in ("▽", "✗"):
                    label = ""
                kle_row.append(label)

                kle_x = x + w

            kle.append(kle_row)

    return kle



def _load_kb_data(kb_path: str) -> dict:
    """
    Load keyboard layout data from a QMK keyboard.json or info.json.

    If the given file has no 'layouts' key (common for sub-variant info.json
    files that inherit from a parent), walks up the directory tree merging
    parent info.json files until layouts are found.

    Also accepts a keyboard directory path — auto-detects keyboard.json then
    info.json inside it.
    """
    # Accept a directory: find the JSON file inside it
    if os.path.isdir(kb_path):
        for name in ("keyboard.json", "info.json"):
            candidate = os.path.join(kb_path, name)
            if os.path.isfile(candidate):
                kb_path = candidate
                break
        else:
            raise FileNotFoundError(f"No keyboard.json or info.json in {kb_path}")

    with open(kb_path) as f:
        data = json.load(f)

    # If this file has no layouts, walk up the tree merging parent info.json
    search_dir = os.path.dirname(os.path.abspath(kb_path))
    while not data.get("layouts"):
        parent = os.path.dirname(search_dir)
        if parent == search_dir:
            break   # reached filesystem root; give up
        for name in ("keyboard.json", "info.json"):
            parent_file = os.path.join(parent, name)
            if os.path.isfile(parent_file):
                with open(parent_file) as f:
                    parent_data = json.load(f)
                # Merge: child values take precedence, parent fills in missing keys
                merged = {**parent_data, **data}
                # Always inherit layouts from parent when child has none
                if not data.get("layouts") and parent_data.get("layouts"):
                    merged["layouts"] = parent_data["layouts"]
                if not data.get("layout_aliases") and parent_data.get("layout_aliases"):
                    merged["layout_aliases"] = parent_data["layout_aliases"]
                data = merged
                break
        search_dir = parent

    return data


def process(kb_path: str, keymap_path: str, out_path: str, layout_name: str | None = None):
    kb_data = _load_kb_data(kb_path)

    if not kb_data.get("layouts"):
        print(f"ERROR: No 'layouts' found in {kb_path} or any parent info.json", file=sys.stderr)
        sys.exit(1)

    available_layouts = list(kb_data["layouts"].keys())
    aliases = kb_data.get("layout_aliases", {})

    def resolve(name: str) -> str:
        return aliases.get(name, name)

    with open(keymap_path) as f:
        keymap_src = f.read()

    raw_layers = _parse_layers(keymap_src)
    if not raw_layers:
        print(f"ERROR: No layers found in {keymap_path}", file=sys.stderr)
        sys.exit(1)

    layer_names = [n for n, _, _ in raw_layers]
    print(f"Found {len(raw_layers)} layer(s): " + ", ".join(layer_names))

    # Pick layout
    def pick_layout(hint: str | None) -> str:
        if hint:
            r = resolve(hint)
            if r in kb_data["layouts"]:
                return r
            print(f"WARNING: Layout '{hint}' not found. Available: {', '.join(available_layouts)}", file=sys.stderr)
        for _, lname, _ in raw_layers:
            r = resolve(lname)
            if r in kb_data["layouts"]:
                return r
        for candidate in ("LAYOUT_all", available_layouts[0]):
            r = resolve(candidate)
            if r in kb_data["layouts"]:
                return r
        return available_layouts[0]

    layout_key = pick_layout(layout_name)
    layout_data = kb_data["layouts"][layout_key]["layout"]
    n_keys = len(layout_data)
    print(f"Using layout '{layout_key}' ({n_keys} keys)")

    # Convert each layer's keycodes to pretty labels, warn on size mismatch
    label_layers: list[list[str]] = []
    for lname, lmacro, keycodes in raw_layers:
        if len(keycodes) != n_keys:
            print(
                f"  WARNING: layer '{lname}' has {len(keycodes)} keycodes "
                f"but layout has {n_keys} — padding/truncating",
                file=sys.stderr,
            )
            keycodes = (keycodes + [""] * n_keys)[:n_keys]
        label_layers.append([_pretty_label(kc) for kc in keycodes])

    kle = generate_kle(layout_data, label_layers)

    os.makedirs(os.path.dirname(os.path.abspath(out_path)), exist_ok=True)
    with open(out_path, "w") as f:
        # KLE raw-data format: rows separated by commas, NO outer [] wrapper.
        rows_json = [json.dumps(row, ensure_ascii=False) for row in kle]
        f.write(",\n".join(rows_json) + "\n")
    print(f"Written {out_path}")


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print(__doc__)
        sys.exit(1)
    _layout = sys.argv[4] if len(sys.argv) > 4 else None
    process(sys.argv[1], sys.argv[2], sys.argv[3], _layout)
