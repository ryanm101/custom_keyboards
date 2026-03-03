# custom_boards

Personal QMK keymap repository — ZMK-style build system where only keymaps and shared userspace code live here; QMK firmware is cloned at build time.

## Keyboards

| Board           | QMK Path                                                | Source              | Build             |
| --------------- | ------------------------------------------------------- | ------------------- | ----------------- |
| JJ40 (ortho)    | `keyboards/kprepublic/jj40/keymaps/ryanm101`            | Upstream QMK master | `make jj40`       |
| Discipline 65%  | `keyboards/coseyfannitutti/discipline/keymaps/ryanm101` | Upstream QMK master | `make discipline` |
| Winry315        | `keyboards/winry/winry315/keymaps/ryanm101`             | Upstream QMK master | `make winry315`   |
| Keychron K3 Pro | `keyboards/keychron/k3_pro/iso/rgb/keymaps/ryanm101`    | Keychron BT fork    | `make k3pro`      |

## How It Works

```
┌──────────────────────────────────┐
│  this repo (keymaps + userspace) │
│  ├── keyboards/                  │
│  ├── users/ryanm101/             │  ← shared tap dances, macros
│  ├── scripts/build_keyboard.sh   │
│  └── qmk.json                   │
└────────────┬─────────────────────┘
             │  clone → copy keymap + userspace → make
    ┌────────▼─────────┐   ┌──────────────────────┐
    │  upstream QMK     │   │  Keychron BT fork    │
    │  qmk/qmk_firmware │   │  Keychron/qmk_firmware│
    └──────────────────┘   └──────────────────────┘
```

The build script (`scripts/build_keyboard.sh`) clones/caches each QMK repo, copies the keymap **and** `users/ryanm101/` into the firmware tree, then runs `make`.

## Shared Userspace

Reusable code lives in `users/ryanm101/` and is automatically copied into the QMK firmware tree at build time:

| File              | Purpose                                                         |
| ----------------- | --------------------------------------------------------------- |
| `tap_dances.h`    | Tap dance enum, `TD()` aliases, function prototypes             |
| `tap_dances.c`    | Tap dance function implementations (bracket pairs, tmux, git…)  |
| `shared_macros.h` | Shared `#define` macros (`LCKSCR`, etc.)                        |
| `rules.mk`        | Conditionally adds `tap_dances.c` when `TAP_DANCE_ENABLE = yes` |

### Using tap dances in a keymap

**Modern QMK boards** (jj40, discipline, winry315): include the shared header:

```c
#include QMK_KEYBOARD_H
#include "tap_dances.h"
#include "shared_macros.h"
```

Then add `TAP_DANCE_ENABLE = yes` to the keymap's `rules.mk` and define `tap_dance_actions[]` in `keymap.c`.

**Keychron fork** (QMK 0.14, k3pro): the fork's introspection doesn't pass `TAP_DANCE_ENABLE` as a C macro, so the k3pro keymap inlines the enum/aliases and wraps tap dance code in `#ifdef TAP_DANCE_ENABLE`. Function implementations are still shared via `tap_dances.c`.

### Adding a new shared macro

Add it to `users/ryanm101/shared_macros.h` — all keymaps that `#include "shared_macros.h"` will pick it up.

### Adding a new tap dance

1. Add the enum value (before `TD_COUNT`) in `tap_dances.h`
2. Add a `#define` alias in `tap_dances.h`
3. Add the function prototype in `tap_dances.h`
4. Implement the function in `tap_dances.c`
5. Add to `tap_dance_actions[]` in each keymap that uses it

## Building Locally

```bash
make help           # Show all targets + per-board config
make all            # Build all four boards
make jj40           # Build kprepublic/jj40
make discipline     # Build coseyfannitutti/discipline
make winry315       # Build winry/winry315
make k3pro          # Build Keychron K3 Pro (auto-clones BT fork)
```

### Flashing

Put the keyboard into bootloader mode, then:

```bash
make flash-jj40
make flash-discipline
make flash-winry315
make flash-k3pro
```

Or use [QMK Toolbox](https://github.com/qmk/qmk_toolbox) with the `.hex`/`.bin` files from `firmware/`.

### Cleaning

```bash
make clean          # Remove built firmware files
make clean-cache    # Remove cached QMK clones
```

## GitHub Actions (CI)

The workflow in `.github/workflows/build.yml` builds all boards on every push to `main`/`master` and uploads firmware as release artifacts.

## Adding a New Keyboard

### Standard QMK board

1. Create `keyboards/<kb>/keymaps/ryanm101/keymap.c` (+ `rules.mk`)
2. Add to `qmk.json`:
   ```json
   {
     "keyboard": "<kb>",
     "keymap": "ryanm101",
     "repo": "https://github.com/qmk/qmk_firmware.git",
     "ref": "master"
   }
   ```
3. Add a Makefile target (follow existing pattern)
4. Push — CI builds it automatically

### Board using a QMK fork

Same as above, but point `repo` and `ref` at the fork (see the k3pro entry in `qmk.json`).

## Repository Structure

```
custom_boards/
├── keyboards/                          # Per-keyboard keymap overrides
│   ├── kprepublic/jj40/keymaps/ryanm101/
│   ├── coseyfannitutti/discipline/keymaps/ryanm101/
│   ├── winry/winry315/keymaps/ryanm101/
│   └── keychron/k3_pro/iso/rgb/keymaps/ryanm101/
├── users/ryanm101/                     # Shared userspace
│   ├── tap_dances.h                    #   tap dance enum + aliases + prototypes
│   ├── tap_dances.c                    #   tap dance implementations
│   ├── shared_macros.h                 #   shared #define macros
│   └── rules.mk                       #   conditional SRC += tap_dances.c
├── scripts/build_keyboard.sh           # Clone/cache QMK, copy files, build
├── qmk.json                           # Per-board repo/ref config
├── Makefile                            # Build targets
└── .github/workflows/build.yml        # CI
```
