REPO_ROOT     := $(shell pwd)
QMK_JSON      := $(REPO_ROOT)/qmk/qmk.json
QMK_DIR       := $(REPO_ROOT)/qmk
KEYMAP        := ryanm101

# Read per-board config from qmk.json (requires jq)
define board_repo
$(shell jq -r '.keyboards[] | select(.keyboard == "$(1)") | .repo' $(QMK_JSON) | head -1)
endef
define board_ref
$(shell jq -r '.keyboards[] | select(.keyboard == "$(1)") | .ref' $(QMK_JSON) | head -1)
endef

# Generic QMK build + flash macro.
# Usage: $(eval $(call qmk_board,<target-name>,<keyboard/path>))
define qmk_board
.PHONY: $(1) flash-$(1)

$(1): ## Build $(2)
	bash scripts/build_qmk.sh \
		"$(2)" \
		"$(KEYMAP)" \
		"$(call board_repo,$(2))" \
		"$(call board_ref,$(2))" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)"

flash-$(1): ## Flash $(2)  [put board in bootloader first]
	bash scripts/build_qmk.sh \
		"$(2)" \
		"$(KEYMAP)" \
		"$(call board_repo,$(2))" \
		"$(call board_ref,$(2))" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)" flash
endef

.PHONY: all all-qmk all-zmk cck-ball clean clean-cache help

help: ## Show this help and per-board config
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@echo "  Per-board config (from qmk.json):"
	@jq -r '.keyboards[] | "    \(.keyboard) [\(.keymap)]: \(.repo) @ \(.ref)"' $(QMK_JSON)

all: all-qmk all-zmk ## Build all boards (QMK + ZMK)

all-qmk: jj40 discipline winry315 winry315-freecad k3pro ## Build all QMK boards

all-zmk: cck-ball ## Build all ZMK boards

# ── QMK board targets (generated via macro) ────────────────────────────────────
$(eval $(call qmk_board,jj40,kprepublic/jj40))
$(eval $(call qmk_board,discipline,coseyfannitutti/discipline))
$(eval $(call qmk_board,winry315,winry/winry315))
$(eval $(call qmk_board,k3pro,keychron/k3_pro/iso/rgb))

# winry315 freecad keymap — overrides the default KEYMAP
.PHONY: winry315-freecad flash-winry315-freecad

winry315-freecad: ## Build winry/winry315 with freecad keymap
	bash scripts/build_qmk.sh \
		"winry/winry315" \
		"freecad" \
		"$(call board_repo,winry/winry315)" \
		"$(call board_ref,winry/winry315)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)"

flash-winry315-freecad: ## Flash winry/winry315 freecad keymap  [put board in bootloader first]
	bash scripts/build_qmk.sh \
		"winry/winry315" \
		"freecad" \
		"$(call board_repo,winry/winry315)" \
		"$(call board_ref,winry/winry315)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)" flash


# ── ZMK targets ────────────────────────────────────────────────────────────────

cck-ball: ## Build CCK-BALL (left + right + settings_reset) via Docker
	bash scripts/build_zmk.sh "$(REPO_ROOT)/zmk/cck_ball" "$(REPO_ROOT)"


# ── Utility targets ────────────────────────────────────────────────────────────

clean: ## Remove built firmware files
	@rm -f firmware/*.hex firmware/*.bin firmware/*.uf2 firmware/*.elf
	@echo "✓ Cleaned"

clean-cache: ## Remove cached QMK/ZMK clones
	@rm -rf .qmk_* .zmk_cache
	@echo "✓ Cache cleared"
