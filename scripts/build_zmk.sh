#!/usr/bin/env bash
# scripts/build_zmk.sh — Build a ZMK keyboard using Docker
# Usage: ./build_zmk.sh <zmk_config_dir> <repo_root>
#
# Reads build.yaml from <zmk_config_dir> to determine board/shield combos.
# Firmware (.uf2) files are placed in <repo_root>/firmware/.

set -euo pipefail

ZMK_CONFIG_DIR="$(cd "${1}" && pwd)"
REPO_ROOT="${2:-$(dirname "$ZMK_CONFIG_DIR")}"
BUILD_YAML="$ZMK_CONFIG_DIR/build.yaml"
CONFIG_DIR="$ZMK_CONFIG_DIR/config"
DOCKER_IMAGE="zmkfirmware/zmk-build-arm:stable"
CACHE_DIR="$REPO_ROOT/.zmk_cache"

if [ ! -f "$BUILD_YAML" ]; then
  echo "ERROR: build.yaml not found at $BUILD_YAML"
  exit 1
fi

echo "=== ZMK Build ==="
echo "    Config: $ZMK_CONFIG_DIR"
echo "    Cache:  $CACHE_DIR"
echo ""

# Parse build matrix from build.yaml — one JSON object per line
if command -v yq &>/dev/null; then
  BUILDS=$(yq -oj -I0 '.include[]' "$BUILD_YAML")
elif command -v python3 &>/dev/null; then
  BUILDS=$(python3 -c "
import yaml, json, sys
with open('$BUILD_YAML') as f:
    data = yaml.safe_load(f)
for item in data.get('include', []):
    print(json.dumps(item))
")
else
  echo "ERROR: yq or python3 (with PyYAML) required to parse build.yaml"
  exit 1
fi

mkdir -p "$CACHE_DIR" "$REPO_ROOT/firmware"

# Build each board+shield combo
echo "$BUILDS" | while IFS= read -r build_entry; do
  board=$(echo "$build_entry" | python3 -c "import json,sys; d=json.load(sys.stdin); print(d['board'])")
  shield=$(echo "$build_entry" | python3 -c "import json,sys; d=json.load(sys.stdin); print(d.get('shield',''))")
  snippet=$(echo "$build_entry" | python3 -c "import json,sys; d=json.load(sys.stdin); print(d.get('snippet',''))")
  cmake_args=$(echo "$build_entry" | python3 -c "import json,sys; d=json.load(sys.stdin); print(d.get('cmake-args',''))")

  display_name="${shield:+$shield - }$board"
  artifact_name="${shield:+${shield}-}${board}-zmk"
  artifact_name="${artifact_name//\//_}"

  echo "→ Building: $display_name"

  # Prepare west args
  extra_west_args=""
  if [ -n "$snippet" ]; then
    extra_west_args="-S \"$snippet\""
  fi

  extra_cmake_args=""
  if [ -n "$shield" ]; then
    extra_cmake_args="-DSHIELD=\"$shield\""
  fi
  if [ -n "$cmake_args" ]; then
    extra_cmake_args="$extra_cmake_args $cmake_args"
  fi

  # Run build in Docker container (--user avoids root-owned cache files)
  docker run --rm \
    --user "$(id -u):$(id -g)" \
    -e HOME=/workspace \
    -v "$ZMK_CONFIG_DIR:/zmk-config-src:ro" \
    -v "$CACHE_DIR:/workspace" \
    -w /workspace \
    "$DOCKER_IMAGE" \
    /bin/bash -c "
      set -e

      # Copy config into workspace (west needs it as a subdirectory)
      mkdir -p /workspace/config
      cp -r /zmk-config-src/config/* /workspace/config/

      # Init west workspace if needed
      if [ ! -f /workspace/.west/config ]; then
        west init -l /workspace/config
        west update --fetch-opt=--filter=tree:0
        west zephyr-export
      fi

      # Build
      west build -s zmk/app -d /workspace/build -b \"$board\" \
        -p \
        $extra_west_args \
        -- -DZMK_CONFIG=/workspace/config $extra_cmake_args

      # Copy output
      if [ -f /workspace/build/zephyr/zmk.uf2 ]; then
        cp /workspace/build/zephyr/zmk.uf2 /workspace/${artifact_name}.uf2
      elif [ -f /workspace/build/zephyr/zmk.bin ]; then
        cp /workspace/build/zephyr/zmk.bin /workspace/${artifact_name}.bin
      fi
    "

  # Copy firmware out of cache to firmware/
  for ext in uf2 bin hex; do
    if [ -f "$CACHE_DIR/${artifact_name}.${ext}" ]; then
      cp "$CACHE_DIR/${artifact_name}.${ext}" "$REPO_ROOT/firmware/"
      echo "  ✓ firmware/${artifact_name}.${ext}"
    fi
  done
done

echo ""
echo "✓ ZMK build complete"
