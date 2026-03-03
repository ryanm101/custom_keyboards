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
  qmk flash "$EXISTING_FW"
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
git submodule update --init --recursive

# Install repo-specific Python dependencies (needed for Keychron fork etc.)
if [ -f "$FIRMWARE_DIR/requirements.txt" ]; then
  echo "→ Creating uv virtual environment for Python dependencies ..."
  uv venv "$FIRMWARE_DIR/.venv" --quiet
  echo "→ Installing Python dependencies via uv ..."
  uv pip install -p "$FIRMWARE_DIR/.venv" -r "$FIRMWARE_DIR/requirements.txt" --quiet
  # Activate venv for subsequent qmk commands
  source "$FIRMWARE_DIR/.venv/bin/activate"
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

# Build or build+flash
if [ "$ACTION" = "flash" ]; then
  echo "→ Compiling + flashing (put keyboard in bootloader mode now) ..."
  make "${KEYBOARD}:${KEYMAP}:flash"
else
  echo "→ Compiling ..."
  make "${KEYBOARD}:${KEYMAP}"
fi

# Collect firmware output
mkdir -p "$REPO_ROOT/firmware"
find "$FIRMWARE_DIR" -maxdepth 1 \( -name "*.hex" -o -name "*.bin" -o -name "*.uf2" \) \
  -exec cp {} "$REPO_ROOT/firmware/" \; 2>/dev/null || true

echo "✓ Done: $KEYBOARD:$KEYMAP"
