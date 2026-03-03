REPO_ROOT     := $(shell pwd)
QMK_JSON      := $(REPO_ROOT)/qmk/qmk.json
QMK_DIR       := $(REPO_ROOT)/qmk
KEYMAP        := ryanm101

# Read per-board config from qmk.json (requires jq)
define board_repo
$(shell jq -r '.keyboards[] | select(.keyboard == "$(1)") | .repo' $(QMK_JSON))
endef
define board_ref
$(shell jq -r '.keyboards[] | select(.keyboard == "$(1)") | .ref' $(QMK_JSON))
endef

.PHONY: all all-qmk all-zmk jj40 discipline winry315 k3pro cck-ball \
        flash-jj40 flash-discipline flash-winry315 flash-k3pro \
        clean clean-cache help

help: ## Show this help and per-board config
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@echo "  Per-board config (from qmk.json):"
	@jq -r '.keyboards[] | "    \(.keyboard): \(.repo) @ \(.ref)"' $(QMK_JSON)

all: all-qmk all-zmk ## Build all boards (QMK + ZMK)

all-qmk: jj40 discipline winry315 k3pro ## Build all QMK boards

all-zmk: cck-ball ## Build all ZMK boards

jj40: ## Build kprepublic/jj40 (shares QMK clone with winry315)
	bash scripts/build_qmk.sh \
		"kprepublic/jj40" \
		"$(KEYMAP)" \
		"$(call board_repo,kprepublic/jj40)" \
		"$(call board_ref,kprepublic/jj40)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)"

discipline: ## Build coseyfannitutti/discipline
	bash scripts/build_qmk.sh \
		"coseyfannitutti/discipline" \
		"$(KEYMAP)" \
		"$(call board_repo,coseyfannitutti/discipline)" \
		"$(call board_ref,coseyfannitutti/discipline)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)"

winry315: ## Build winry/winry315 (shares QMK clone with jj40)
	bash scripts/build_qmk.sh \
		"winry/winry315" \
		"$(KEYMAP)" \
		"$(call board_repo,winry/winry315)" \
		"$(call board_ref,winry/winry315)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)"

k3pro: ## Build Keychron K3 Pro (separate Keychron fork clone)
	bash scripts/build_qmk.sh \
		"keychron/k3_pro/iso/rgb" \
		"$(KEYMAP)" \
		"$(call board_repo,keychron/k3_pro/iso/rgb)" \
		"$(call board_ref,keychron/k3_pro/iso/rgb)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)"


# ── Flash targets ──────────────────────────────────────────────────────────────
# Put the keyboard into bootloader mode before running these.

flash-jj40: ## Flash kprepublic/jj40
	bash scripts/build_qmk.sh \
		"kprepublic/jj40" \
		"$(KEYMAP)" \
		"$(call board_repo,kprepublic/jj40)" \
		"$(call board_ref,kprepublic/jj40)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)" flash

flash-discipline: ## Flash coseyfannitutti/discipline
	bash scripts/build_qmk.sh \
		"coseyfannitutti/discipline" \
		"$(KEYMAP)" \
		"$(call board_repo,coseyfannitutti/discipline)" \
		"$(call board_ref,coseyfannitutti/discipline)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)" flash

flash-winry315: ## Flash winry/winry315
	bash scripts/build_qmk.sh \
		"winry/winry315" \
		"$(KEYMAP)" \
		"$(call board_repo,winry/winry315)" \
		"$(call board_ref,winry/winry315)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)" flash

flash-k3pro: ## Flash Keychron K3 Pro
	bash scripts/build_qmk.sh \
		"keychron/k3_pro/iso/rgb" \
		"$(KEYMAP)" \
		"$(call board_repo,keychron/k3_pro/iso/rgb)" \
		"$(call board_ref,keychron/k3_pro/iso/rgb)" \
		"$(QMK_DIR)" \
		"$(REPO_ROOT)" flash


# ── ZMK targets ────────────────────────────────────────────────────────────────

cck-ball: ## Build CCK-BALL (left + right + settings_reset) via Docker
	bash scripts/build_zmk.sh "$(REPO_ROOT)/zmk/cck_ball" "$(REPO_ROOT)"


clean: ## Remove built firmware files
	@rm -f firmware/*.hex firmware/*.bin firmware/*.uf2
	@echo "✓ Cleaned"

clean-cache: ## Remove cached QMK/ZMK clones
	@rm -rf .qmk_* .zmk_cache
	@echo "✓ Cache cleared"
