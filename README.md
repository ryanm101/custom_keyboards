# custom_boards

Personal QMK keymap repository — ZMK-style build system where only keymaps live here and QMK firmware is pulled at build time.

## Keyboards

| Board           | QMK Path                                                | Layout                          | Build                       |
| --------------- | ------------------------------------------------------- | ------------------------------- | --------------------------- |
| JJ40 (ortho)    | `layouts/ortho_4x12/ryanm101`                           | Community layout (`ortho_4x12`) | Via `make ortho KB=<board>` |
| Discipline 65%  | `keyboards/coseyfannitutti/discipline/keymaps/ryanm101` | Upstream QMK                    | CI + `make discipline`      |
| Winry315        | `keyboards/winry/winry315/keymaps/ryanm101`             | Upstream QMK                    | CI + `make winry315`        |
| Keychron K3 Pro | `keyboards/keychron/k3_pro/iso/rgb/keymaps/ryanm101`    | Keychron BT fork                | CI + `make k3pro`           |

## How It Works

```
┌─────────────────────────────────┐
│  this repo (keymaps only)       │
│  ├── keyboards/                 │
│  ├── layouts/                   │
│  └── qmk.json                   │
└────────────┬────────────────────┘
             │ overlay_dir / copy
    ┌────────▼────────┐   ┌──────────────────────┐
    │  upstream QMK   │   │  Keychron BT fork    │
    │  qmk/qmk_firmware│  │  Keychron/qmk_firmware│
    └─────────────────┘   └──────────────────────┘
```

GitHub Actions builds firmware on every push and attaches `.hex`/`.uf2` files to a GitHub Release.

## Local Setup (One-Time)

### 1. Install QMK CLI

```bash
pip3 install qmk
qmk setup
```

### 2. Clone this repo and register the overlay

```bash
git clone git@github.com:<your-username>/custom_boards.git ~/custom_boards
cd ~/custom_boards
make setup
```

By default `make setup` looks for QMK at `~/qmk_firmware`. Override with:

```bash
make setup QMK_FIRMWARE_DIR=/path/to/qmk_firmware
```

## Building Locally

```bash
make help           # Show all targets

make all            # Build Discipline + Winry315
make discipline     # Build coseyfannitutti/discipline
make winry315       # Build winry/winry315
make k3pro          # Build Keychron K3 Pro (auto-clones fork)

# JJ40 / ortho_4x12 — specify a board that supports the layout, e.g.:
make ortho KB=planck/rev6
```

Firmware files (`.hex`, `.uf2`) are placed in the repo root and are gitignored.

## GitHub Actions (CI)

Two workflows run on every push to `main`/`master`:

| Workflow      | File                                   | Boards               |
| ------------- | -------------------------------------- | -------------------- |
| Standard QMK  | `.github/workflows/build_qmk.yml`      | Discipline, Winry315 |
| Keychron fork | `.github/workflows/build_keychron.yml` | K3 Pro               |

Compiled firmware is uploaded as a GitHub Release automatically.

**Enable the workflows**: Go to your repo → Settings → Actions → Allow all actions.

## Adding a New Keyboard

### Standard QMK board

1. Create `keyboards/<kb>/keymaps/ryanm101/keymap.c` (+ `rules.mk`, `config.h`)
2. Add to `qmk.json`:
   ```json
   ["<kb>", "ryanm101"]
   ```
3. Push — CI builds it automatically.

### Board using a QMK fork

1. Add keymap files under `keyboards/<kb>/keymaps/ryanm101/`
2. Create a new workflow in `.github/workflows/` modelled on `build_keychron.yml`, pointing at the correct fork repo and branch.

## Flashing

Use [QMK Toolbox](https://github.com/qmk/qmk_toolbox) (GUI) or the CLI:

```bash
qmk flash -kb coseyfannitutti/discipline -km ryanm101
```

For the K3 Pro, use the firmware file produced by CI with the Keychron Web Interface or QMK Toolbox in DFU mode.

## JJ40 / ortho_4x12 note

The `jj40` keyboard was removed from upstream QMK. The keymap now lives under `layouts/ortho_4x12/ryanm101/` and can be compiled against any QMK board that declares `ortho_4x12` layout support (e.g. Planck, crkbd variants). Specify the target board at build time with `make ortho KB=<board>`.
