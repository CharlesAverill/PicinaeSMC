.PHONY: payload loader clean all

all: payload loader run

BUILD=bin
SRC=src

PAYLOAD=$(SRC)/payload.S
ELF=$(BUILD)/$(PAYLOAD:.S=.elf)
BIN=$(BUILD)/$(PAYLOAD:.S=.bin)
CMP=$(BUILD)/$(PAYLOAD:.S=.lz4)
INC=$(BUILD)/$(PAYLOAD:.S=.inc)

LOADER_SRC=$(SRC)/loader.c
LOADER=$(BUILD)/$(LOADER_SRC:.c=)
LZ4_SRC=$(SRC)/lz4.c

COMPRESS_RAW_SRC=$(SRC)/compress_raw.c
COMPRESS=$(BUILD)/$(COMPRESS_RAW_SRC:.c=)

RV_PREFIX=riscv32-unknown-elf-
GCC=$(RV_PREFIX)gcc
OBJCOPY=$(RV_PREFIX)objcopy
HOST_GCC=gcc

QEMU=qemu-riscv32

CFLAGS=-Iinclude

$(BUILD):
	mkdir -p $(BUILD) $(BUILD)/$(SRC)

payload: $(INC)
$(INC): $(PAYLOAD) $(LZ4_SRC) $(BUILD)
	$(GCC) $(CFLAGS) -nostdlib -static -Wl,-Ttext=0x0 $(PAYLOAD) -o $(ELF) -march=rv32im -mabi=ilp32
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	$(HOST_GCC) $(CFLAGS) -O2 $(COMPRESS_RAW_SRC) $(LZ4_SRC) -o $(COMPRESS)
	./$(COMPRESS) $(BIN) $(CMP)
	xxd -i $(CMP) > $(INC)

loader: $(LOADER)
$(LOADER): $(LOADER_SRC) $(INC)
	$(GCC) $(CFLAGS) -static -O2 $(LOADER_SRC) $(LZ4_SRC) -x c $(INC) -o $(LOADER)

run: $(LOADER_SRC) $(PAYLOAD) $(LZ4_SRC)
	$(QEMU) ./$(LOADER)

clean:
	@rm -rf $(ELF) $(BIN) $(CMP) $(INC) $(LOADER) $(COMPRESS) $(BUILD)

