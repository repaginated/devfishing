SHELL := /bin/sh
ARCH  := $(shell uname -m)
NPROC := $(shell nproc 2>/dev/null || echo 4)

CC ?= gcc

Q3_DIR      := Quake3e
Q3_OUT_DIR  := $(Q3_DIR)/build/release-linux-$(ARCH)
SRC_VM_DIR  := src_vm
WS_DIR      := workspace
GAME_DIR    := $(WS_DIR)/webfish
VM_OUT_DIR  := $(GAME_DIR)

CC_C99     := $(CC)
CFLAGS_C99 := -std=c99 -Wall -Wextra -O2 -fPIC -I$(Q3_DIR)/code/qcommon
LDFLAGS_SO := -shared -lm

GAME_SO    := $(VM_OUT_DIR)/qagame$(ARCH).so
CGAME_SO   := $(VM_OUT_DIR)/cgame$(ARCH).so
UI_SO      := $(VM_OUT_DIR)/ui$(ARCH).so

DEV_ARGS := \
	+set fs_basepath . \
	+set fs_homepath . \
	+set fs_basegame webfish \
	+set fs_game webfish \
	+set sv_pure 0 \
	+set vm_game 0 \
	+set vm_cgame 0 \
	+set vm_ui 0 \
	+set r_fullscreen 0 \
	+set r_mode -1 \
	+set r_customwidth 960 \
	+set r_customheight 540 \
	+set r_clear 1 \
	+toggleconsole

.PHONY: all engine vm sync run debug clean distclean

all: engine vm sync

engine:
	$(MAKE) -C $(Q3_DIR) USE_SDL=1 USE_RENDERER_DLOPEN=1 -j"$(NPROC)"

vm: $(VM_OUT_DIR) $(GAME_SO) $(CGAME_SO) $(UI_SO)

$(VM_OUT_DIR):
	mkdir -p $(VM_OUT_DIR)

$(GAME_SO): $(SRC_VM_DIR)/stub_game.c
	$(CC_C99) $(CFLAGS_C99) $< $(LDFLAGS_SO) -o $@

$(CGAME_SO): $(SRC_VM_DIR)/stub_cgame.c $(Q3_DIR)/code/qcommon/q_math.c
	$(CC_C99) $(CFLAGS_C99) -I$(Q3_DIR)/code/game \
		$(SRC_VM_DIR)/stub_cgame.c $(Q3_DIR)/code/qcommon/q_math.c \
		$(LDFLAGS_SO) -o $@

$(UI_SO): $(SRC_VM_DIR)/stub_ui.c
	$(CC_C99) $(CFLAGS_C99) $< $(LDFLAGS_SO) -o $@

sync: $(VM_OUT_DIR)
	mkdir -p $(WS_DIR)
	cp $(Q3_OUT_DIR)/quake3e.x64 $(WS_DIR)/
	cp $(Q3_OUT_DIR)/quake3e.ded.x64 $(WS_DIR)/ 2>/dev/null || true
	cp $(Q3_OUT_DIR)/quake3e_*_$(ARCH).so $(WS_DIR)/ 2>/dev/null || true

run: vm sync
	env -C $(WS_DIR) ./quake3e.x64 $(DEV_ARGS)

debug: vm sync
	env -C $(WS_DIR) gdb -ex "run $(DEV_ARGS)" -- ./quake3e.x64

clean:
	rm -f $(GAME_SO) $(CGAME_SO) $(UI_SO) $(GAME_DIR)/vm/*.so

distclean: clean
	$(MAKE) -C $(Q3_DIR) clean
	rm -f $(WS_DIR)/quake3e.x64 $(WS_DIR)/quake3e.ded.x64 $(WS_DIR)/quake3e_*_$(ARCH).so
