.PHONY: all clean

all: jit

BUILD := bin
SRC := src
INCLUDE := include
JIT := jit

C_FILES := $(shell find $(SRC) -type f -name "*.c")
O_FILES := $(patsubst $(SRC)/%.c,$(BUILD)/$(SRC)/%.o,$(C_FILES))

RV_PREFIX := riscv32-unknown-elf-
CC := $(RV_PREFIX)gcc
OBJCOPY := $(RV_PREFIX)objcopy
QEMU=qemu-riscv32

CFLAGS := -I$(INCLUDE) -Wall -Wextra -O2

jit: $(BUILD)/$(JIT)
$(BUILD)/$(JIT): $(O_FILES)
	$(CC) $(O_FILES) -o $@

$(BUILD)/$(SRC)/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)
