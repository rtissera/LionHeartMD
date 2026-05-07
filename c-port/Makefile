# Lionheart Remake C/SDL2 port - phase 1 bootstrap
# Faithful port of the GPL-3.0 Lionheart Remake (b3dgs) Java source.

CC      ?= cc
CFLAGS  ?= -O2 -g -Wall -Wextra -Wno-unused-parameter -std=c11
PKG_DEPS = sdl2 SDL2_image SDL2_ttf
CPPFLAGS = -Iinclude $(shell pkg-config --cflags $(PKG_DEPS) 2>/dev/null)
LDFLAGS  =
LDLIBS   = $(shell pkg-config --libs $(PKG_DEPS) 2>/dev/null) -lm

SRC_DIR := src
OBJ_DIR := build
BIN     := $(OBJ_DIR)/lionheart

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

$(OBJ_DIR):
	@mkdir -p $@

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(OBJ_DIR)

-include $(DEPS)
