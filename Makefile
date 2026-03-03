QMK_JSON      := $(shell pwd)/qmk.json
USERSPACE_DIR := $(shell pwd)
KEYMAP        := ryanm101

# Read per-board config from qmk.json (requires jq)
define board_repo
$(shell jq -r '.keyboards[] | select(.keyboard == "$(1)") | .repo' $(QMK_JSON))
endef
define board_ref
$(shell jq -r '.keyboards[] | select(.keyboard == "$(1)") | .ref' $(QMK_JSON))
endef
define layout_repo
$(shell jq -r '.layouts[] | select(.layout == "$(1)") | .repo' $(QMK_JSON))
endef
define layout_ref
$(shell jq -r '.layouts[] | select(.layout == "$(1)") | .ref' $(QMK_JSON))
endef

.PHONY: all jj40 discipline winry315 k3pro ortho \
        flash-jj40 flash-discipline flash-winry315 flash-k3pro flash-ortho \
        clean clean-cache help

help: ## Show this help and per-board config
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@echo "  Per-board config (from qmk.json):"
	@jq -r '.keyboards[] | "    \(.keyboard): \(.repo) @ \(.ref)"' $(QMK_JSON)

all: jj40 discipline winry315 k3pro ## Build all boards

jj40: ## Build kprepublic/jj40 (shares QMK clone with winry315)
	bash scripts/build_keyboard.sh \
		"kprepublic/jj40" \
		"$(KEYMAP)" \
		"$(call board_repo,kprepublic/jj40)" \
		"$(call board_ref,kprepublic/jj40)" \
		"$(USERSPACE_DIR)"

discipline: ## Build coseyfannitutti/discipline
	bash scripts/build_keyboard.sh \
		"coseyfannitutti/discipline" \
		"$(KEYMAP)" \
		"$(call board_repo,coseyfannitutti/discipline)" \
		"$(call board_ref,coseyfannitutti/discipline)" \
		"$(USERSPACE_DIR)"

winry315: ## Build winry/winry315 (shares QMK clone with jj40)
	bash scripts/build_keyboard.sh \
		"winry/winry315" \
		"$(KEYMAP)" \
		"$(call board_repo,winry/winry315)" \
		"$(call board_ref,winry/winry315)" \
		"$(USERSPACE_DIR)"

k3pro: ## Build Keychron K3 Pro (separate Keychron fork clone)
	bash scripts/build_keyboard.sh \
		"keychron/k3_pro/iso/rgb" \
		"$(KEYMAP)" \
		"$(call board_repo,keychron/k3_pro/iso/rgb)" \
		"$(call board_ref,keychron/k3_pro/iso/rgb)" \
		"$(USERSPACE_DIR)"

ortho: ## Build ortho_4x12 layout (requires KB=<board>, e.g. make ortho KB=planck/rev6)
ifndef KB
	$(error KB is not set. Usage: make ortho KB=<keyboard_that_supports_ortho_4x12>)
endif
	bash scripts/build_keyboard.sh \
		"$(KB)" \
		"$(KEYMAP)" \
		"$(call layout_repo,ortho_4x12)" \
		"$(call layout_ref,ortho_4x12)" \
		"$(USERSPACE_DIR)"

# ── Flash targets ──────────────────────────────────────────────────────────────
# Put the keyboard into bootloader mode before running these.

flash-jj40: ## Flash kprepublic/jj40
	bash scripts/build_keyboard.sh \
		"kprepublic/jj40" \
		"$(KEYMAP)" \
		"$(call board_repo,kprepublic/jj40)" \
		"$(call board_ref,kprepublic/jj40)" \
		"$(USERSPACE_DIR)" flash

flash-discipline: ## Flash coseyfannitutti/discipline
	bash scripts/build_keyboard.sh \
		"coseyfannitutti/discipline" \
		"$(KEYMAP)" \
		"$(call board_repo,coseyfannitutti/discipline)" \
		"$(call board_ref,coseyfannitutti/discipline)" \
		"$(USERSPACE_DIR)" flash

flash-winry315: ## Flash winry/winry315
	bash scripts/build_keyboard.sh \
		"winry/winry315" \
		"$(KEYMAP)" \
		"$(call board_repo,winry/winry315)" \
		"$(call board_ref,winry/winry315)" \
		"$(USERSPACE_DIR)" flash

flash-k3pro: ## Flash Keychron K3 Pro
	bash scripts/build_keyboard.sh \
		"keychron/k3_pro/iso/rgb" \
		"$(KEYMAP)" \
		"$(call board_repo,keychron/k3_pro/iso/rgb)" \
		"$(call board_ref,keychron/k3_pro/iso/rgb)" \
		"$(USERSPACE_DIR)" flash

flash-ortho: ## Flash ortho_4x12 layout (requires KB=<board>)
ifndef KB
	$(error KB is not set. Usage: make flash-ortho KB=<board>)
endif
	bash scripts/build_keyboard.sh \
		"$(KB)" \
		"$(KEYMAP)" \
		"$(call layout_repo,ortho_4x12)" \
		"$(call layout_ref,ortho_4x12)" \
		"$(USERSPACE_DIR)" flash

clean: ## Remove built firmware files
	@rm -f firmware/*.hex firmware/*.bin firmware/*.uf2
	@echo "✓ Cleaned"

clean-cache: ## Remove cached QMK clones (.qmk_* dirs in repo root)
	@rm -rf .qmk_*
	@echo "✓ QMK clone cache cleared"
