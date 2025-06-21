# -------- toolchain USED GPT TILL ANDREI COMES TO RESCUE ----------
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -O2 -flto -fopenmp -march=native
LDFLAGS := -lm -lopenblas -lzstd 

# -------- directories --------
SRC_DIR    := src
TOOLS_DIR  := tools
OBJ_DIR    := build/obj
BIN_DIR    := build

# -------- files --------------
SRCS       := $(wildcard $(SRC_DIR)/*.c)
OBJS       := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
ENGINE     := $(BIN_DIR)/engine

TRAIN_SRC  := $(SRC_DIR)/bytefish.c
TRAIN_OBJ  := $(OBJ_DIR)/bytefish.o
TRAIN_BIN  := $(BIN_DIR)/bytefish

# -------- rules --------------
.PHONY: all train test clean

all: $(ENGINE)

# --- engine target ---
$(ENGINE): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- trainer target ---
train: $(TRAIN_BIN)
	@echo "Built trainer at $(TRAIN_BIN)"

$(TRAIN_BIN): $(TRAIN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/train_int.o: $(TRAIN_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- unit tests (perft) ---
TEST    := $(BIN_DIR)/tests
test: $(TEST)
	@$(TEST)

$(TEST): $(ENGINE) test/perft_test.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# --- directories ---
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf build
