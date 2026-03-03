# custom_boards

Personal keyboard firmware repository — QMK and ZMK keymaps, shared userspace code, and a unified build system. Firmware is cloned at build time; only keymaps live here.

## Keyboards

### QMK

| Board           | Path                                                        | Source              | Build             |
| --------------- | ----------------------------------------------------------- | ------------------- | ----------------- |
| JJ40 (ortho)    | `qmk/keyboards/kprepublic/jj40/keymaps/ryanm101`            | Upstream QMK master | `make jj40`       |
| Discipline 65%  | `qmk/keyboards/coseyfannitutti/discipline/keymaps/ryanm101` | Upstream QMK master | `make discipline` |
| Winry315        | `qmk/keyboards/winry/winry315/keymaps/ryanm101`             | Upstream QMK master | `make winry315`   |
| Keychron K3 Pro | `qmk/keyboards/keychron/k3_pro/iso/rgb/keymaps/ryanm101`    | Keychron BT fork    | `make k3pro`      |

### ZMK

| Board    | Path                  | Source     | Build           |
| -------- | --------------------- | ---------- | --------------- |
| CCK-BALL | `zmk/cck_ball/config` | ZMK v0.3.0 | `make cck-ball` |

## Repository Structure

```
custom_boards/
├── qmk/                               # QMK boards
│   ├── keyboards/                      #   per-board keymaps
│   │   ├── kprepublic/jj40/keymaps/ryanm101/
│   │   ├── coseyfannitutti/discipline/keymaps/ryanm101/
│   │   ├── winry/winry315/keymaps/ryanm101/
│   │   └── keychron/k3_pro/iso/rgb/keymaps/ryanm101/
│   ├── users/ryanm101/                 #   shared QMK userspace
│   │   ├── tap_dances.h                #     enum + aliases + prototypes
│   │   ├── tap_dances.c                #     function implementations
│   │   ├── shared_macros.h             #     shared #define macros
│   │   └── rules.mk                   #     conditional SRC += tap_dances.c
│   └── qmk.json                       #   per-board repo/ref config
├── zmk/                               # ZMK boards
│   └── cck_ball/
│       ├── config/                     #   west.yml, keymap, conf, shields
│       └── build.yaml                  #   build matrix
├── scripts/
│   ├── build_qmk.sh                  #   QMK build script
│   └── build_zmk.sh                  #   ZMK build script (Docker)
├── Makefile
└── .github/workflows/
    ├── build_qmk.yml                  #   QMK CI
    └── build_zmk.yml                  #   ZMK CI
```

## Building Locally

### QMK

```bash
make help           # Show all targets
make all-qmk        # Build all QMK boards
make jj40           # Build kprepublic/jj40
make discipline     # Build coseyfannitutti/discipline
make winry315       # Build winry/winry315
make k3pro          # Build Keychron K3 Pro (auto-clones BT fork)
```

### ZMK (requires Docker)

```bash
make all-zmk        # Build all ZMK boards
make cck-ball       # Build CCK-BALL (left + right + settings_reset)
```

### All boards

```bash
make all            # Build everything (QMK + ZMK)
```

### Flashing

**QMK** — put keyboard in bootloader mode, then:

```bash
make flash-jj40
make flash-discipline
make flash-winry315
make flash-k3pro
```

**ZMK** — copy the `.uf2` file from `firmware/` to the nice_nano mounted drive in bootloader mode.

### Cleaning

```bash
make clean          # Remove built firmware files
make clean-cache    # Remove cached QMK/ZMK clones
```

## Shared Code

### QMK Userspace (`qmk/users/ryanm101/`)

Reusable code shared across QMK keymaps:

| File              | Purpose                                                         |
| ----------------- | --------------------------------------------------------------- |
| `tap_dances.h`    | Tap dance enum, `TD()` aliases, function prototypes             |
| `tap_dances.c`    | Tap dance implementations (bracket pairs, tmux, git…)           |
| `shared_macros.h` | Shared `#define` macros (`LCKSCR`, etc.)                        |
| `rules.mk`        | Conditionally adds `tap_dances.c` when `TAP_DANCE_ENABLE = yes` |

### ZMK Shared Config

ZMK behaviors and macros are defined in `zmk/cck_ball/config/cck_ball.keymap`. For future ZMK boards, shared devicetree includes can be placed alongside the config.

## GitHub Actions (CI)

| Workflow | File                              | Boards                            | Trigger  |
| -------- | --------------------------------- | --------------------------------- | -------- |
| QMK      | `.github/workflows/build_qmk.yml` | JJ40, Discipline, Winry315, K3Pro | `qmk/**` |
| ZMK      | `.github/workflows/build_zmk.yml` | CCK-BALL                          | `zmk/**` |

## Adding a New Keyboard

### QMK board

1. Create `qmk/keyboards/<kb>/keymaps/ryanm101/keymap.c` (+ `rules.mk`)
2. Add to `qmk/qmk.json`
3. Add a Makefile target
4. Push — CI builds automatically

### ZMK board

1. Create `zmk/<board>/config/` with west.yml, keymap, conf, shield files
2. Create `zmk/<board>/build.yaml`
3. Add a Makefile target
4. Update `.github/workflows/build_zmk.yml` paths (or add a new workflow)
