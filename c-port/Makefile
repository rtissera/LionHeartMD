# Lionheart Remake C/SDL2 port - phase 1 bootstrap
# Faithful port of the GPL-3.0 Lionheart Remake (b3dgs) Java source.

CC      ?= cc
CFLAGS  ?= -O2 -g -Wall -Wextra -Wno-unused-parameter -std=c11
PKG_DEPS = sdl2 SDL2_image SDL2_ttf
CPPFLAGS = -Iinclude $(shell pkg-config --cflags $(PKG_DEPS) 2>/dev/null)
LDFLAGS  =
# -ldl: lh_music dlopen()s libsc68 at runtime (see lh_music.c).
# libsc68 itself is NOT linked: we resolve symbols dynamically so the
# binary still works on systems without it (silent music fallback).
LDLIBS   = $(shell pkg-config --libs $(PKG_DEPS) 2>/dev/null) -lm -ldl

SRC_DIR := src
OBJ_DIR := build
BIN     := $(OBJ_DIR)/lionheart

# Standalone CLI tests live in src/test_*.c and are NOT linked into the
# main lionheart binary; build them with `make test`.
TEST_SRCS := $(wildcard $(SRC_DIR)/test_*.c)
SRCS := $(filter-out $(TEST_SRCS),$(wildcard $(SRC_DIR)/*.c))
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Object files linked into every test binary. Tests are tiny CLIs with
# no main game loop, so we link the modules they may consult plus the
# SDL libs they need at link time. Unused .o entries are dead-stripped.
TEST_LIB_OBJS := \
    $(OBJ_DIR)/lh_le_file_reading.o \
    $(OBJ_DIR)/lh_le_file_writing.o \
    $(OBJ_DIR)/lh_le_map_tile.o \
    $(OBJ_DIR)/lh_map_tile_persister.o \
    $(OBJ_DIR)/lh_audio.o \
    $(OBJ_DIR)/lh_music.o \
    $(OBJ_DIR)/lh_sfx.o \
    $(OBJ_DIR)/lh_medias.o

TEST_BINS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%,$(TEST_SRCS))

.PHONY: all clean run test

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test: $(TEST_BINS)
	@for b in $(TEST_BINS); do echo "=== $$b ==="; $$b || exit 1; done

$(OBJ_DIR)/test_%: $(SRC_DIR)/test_%.c $(TEST_LIB_OBJS) | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(TEST_LIB_OBJS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

$(OBJ_DIR):
	@mkdir -p $@

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(OBJ_DIR)

-include $(DEPS)
