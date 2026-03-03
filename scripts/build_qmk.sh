#!/usr/bin/env bash
# scripts/build_qmk.sh — Build (or flash) a single QMK keyboard
# Usage: ./build_qmk.sh <keyboard> <keymap> <repo> <ref> <qmk_dir> <repo_root> [action]
#   action: build (default) | flash
#
# When action=flash and a firmware file already exists in firmware/, the build
# step is skipped and the existing binary is flashed directly via `qmk flash`.
#
# The QMK clone lives in <userspace_dir>/.qmk_<repo-slug>/ so boards sharing
# the same repo (e.g. jj40 + winry315) reuse a single checkout.

set -euo pipefail

KEYBOARD="${1}"
KEYMAP="${2}"
REPO="${3}"
REF="${4}"
USERSPACE_DIR="${5:-$(pwd)}"
REPO_ROOT="${6:-$(dirname "$USERSPACE_DIR")}"
ACTION="${7:-build}"

# Derive a slug from the repo URL so boards on the same repo share one clone.
REPO_SLUG="$(echo "$REPO" | sed 's|https\?://||; s|[^a-zA-Z0-9]|_|g' | cut -c1-50)"
FIRMWARE_DIR="${REPO_ROOT}/.qmk_${REPO_SLUG}"

# QMK names output files as <keyboard_underscored>_<keymap>.<ext>
KEYBOARD_SLUG="$(echo "$KEYBOARD" | tr '/' '_')"
mkdir -p "$REPO_ROOT/firmware"
EXISTING_FW="$(find "$REPO_ROOT/firmware" -maxdepth 1 \
  \( -name "${KEYBOARD_SLUG}_${KEYMAP}.hex" \
  -o -name "${KEYBOARD_SLUG}_${KEYMAP}.bin" \
  -o -name "${KEYBOARD_SLUG}_${KEYMAP}.uf2" \) 2>/dev/null | head -1 || true)"

# If flashing and a binary already exists, skip straight to flash.
if [ "$ACTION" = "flash" ] && [ -n "$EXISTING_FW" ]; then
  echo "=== Flashing: $KEYBOARD:$KEYMAP ==="
  echo "    Using existing firmware: $(basename "$EXISTING_FW")"
  echo "    (Put keyboard in bootloader mode now)"
  echo ""
  
  # QMK internal flashers lock onto their home workspace and strip paths.
  # So we copy the firmware temporarily into the qmk repo root.
  FW_NAME="$(basename "$EXISTING_FW")"
  cp "$EXISTING_FW" "$FIRMWARE_DIR/$FW_NAME"
  cd "$FIRMWARE_DIR"
  qmk flash "$FW_NAME"
  cd - > /dev/null
  rm -f "$FIRMWARE_DIR/$FW_NAME"
  
  echo "✓ Done: $KEYBOARD:$KEYMAP"
  exit 0
fi

echo "=== Building: $KEYBOARD:$KEYMAP ==="
echo "    Repo:  $REPO @ $REF"
echo "    Cache: $FIRMWARE_DIR"
echo ""

# Clone repo if not already present
if [ ! -d "$FIRMWARE_DIR" ]; then
  echo "→ Cloning $REPO ..."
  git clone --recurse-submodules "$REPO" "$FIRMWARE_DIR"
fi

cd "$FIRMWARE_DIR"
echo "→ Checking out $REF ..."
git fetch origin
git checkout "$REF"

# Only update submodules if the HEAD commit actually changed (saves ~30s per board).
CURRENT_HEAD="$(git rev-parse HEAD)"
if [ ! -f "$FIRMWARE_DIR/.last_submodule_sync" ] || \
   [ "$(cat "$FIRMWARE_DIR/.last_submodule_sync")" != "$CURRENT_HEAD" ]; then
  echo "→ Updating submodules ..."
  git submodule update --init --recursive
  echo "$CURRENT_HEAD" > "$FIRMWARE_DIR/.last_submodule_sync"
fi

# Install repo-specific Python dependencies (needed for Keychron fork etc.)
if [ -f "$FIRMWARE_DIR/requirements.txt" ]; then
  echo "→ Creating uv virtual environment for Python dependencies ..."
  # --clear avoids the interactive 'replace venv?' prompt
  uv venv "$FIRMWARE_DIR/.venv" --clear --quiet
  echo "→ Installing Python dependencies via uv ..."
  # Activate first, then install (no deprecated -p flag)
  source "$FIRMWARE_DIR/.venv/bin/activate"
  uv pip install -r "$FIRMWARE_DIR/requirements.txt" --quiet
fi

# Copy our keymap into firmware tree
KEYMAP_SRC="$USERSPACE_DIR/keyboards/$KEYBOARD/keymaps/$KEYMAP"
KEYMAP_DST="$FIRMWARE_DIR/keyboards/$KEYBOARD/keymaps/$KEYMAP"
echo "→ Copying keymap ..."
mkdir -p "$KEYMAP_DST"
cp -r "$KEYMAP_SRC/." "$KEYMAP_DST/"

# Copy shared userspace (tap dances, shared rules, etc.)
USERS_SRC="$USERSPACE_DIR/users/$KEYMAP"
USERS_DST="$FIRMWARE_DIR/users/$KEYMAP"
if [ -d "$USERS_SRC" ]; then
  echo "→ Copying userspace ($KEYMAP) ..."
  mkdir -p "$USERS_DST"
  cp -r "$USERS_SRC/." "$USERS_DST/"
fi

# Build
echo "→ Compiling ..."
make "${KEYBOARD}:${KEYMAP}"

# Collect firmware output — match only this keyboard's files to avoid
# copying bootloader hexes, ELFs, and other boards' artefacts.
mkdir -p "$REPO_ROOT/firmware"
for ext in hex bin uf2; do
  find "$FIRMWARE_DIR" -maxdepth 2 -name "${KEYBOARD_SLUG}_${KEYMAP}.${ext}" \
    -exec cp -f {} "$REPO_ROOT/firmware/" \; 2>/dev/null || true
done

# Find the newly built firmware
NEW_FW="$(find "$REPO_ROOT/firmware" -maxdepth 1 \
  \( -name "${KEYBOARD_SLUG}_${KEYMAP}.hex" \
  -o -name "${KEYBOARD_SLUG}_${KEYMAP}.bin" \
  -o -name "${KEYBOARD_SLUG}_${KEYMAP}.uf2" \) 2>/dev/null | head -1 || true)"

if [ "$ACTION" = "flash" ]; then
  if [ -n "$NEW_FW" ]; then
    echo "→ Flashing (put keyboard in bootloader mode now) ..."
    
    FW_NAME="$(basename "$NEW_FW")"
    cp "$NEW_FW" "$FIRMWARE_DIR/$FW_NAME"
    cd "$FIRMWARE_DIR"
    qmk flash "$FW_NAME"
    cd - > /dev/null
    rm -f "$FIRMWARE_DIR/$FW_NAME"
  else
    echo "❌ Error: Could not find generated firmware for $KEYBOARD_SLUG_$KEYMAP to flash."
    exit 1
  fi
fi

echo "✓ Done: $KEYBOARD:$KEYMAP"
