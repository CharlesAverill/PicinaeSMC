.PHONY: payload loader clean

PAYLOAD=payload.S
ELF=$(PAYLOAD:.S=.elf)
BIN=$(PAYLOAD:.S=.bin)
CMP=$(PAYLOAD:.S=.lz4)
INC=$(PAYLOAD:.S=.inc)

LOADER_SRC=loader.c
LOADER=$(LOADER_SRC:.c=)
LZ4_SRC=lz4.c

COMPRESS_RAW_SRC=compress_raw.c
COMPRESS=$(COMPRESS_RAW_SRC:.c=)

RV_PREFIX=riscv32-unknown-elf-
GCC=$(RV_PREFIX)gcc
OBJCOPY=$(RV_PREFIX)objcopy
HOST_GCC=gcc

LZ4=lz4

QEMU=qemu-riscv32

payload: $(INC)
$(INC): $(PAYLOAD) $(LZ4_SRC)
	$(GCC) -nostdlib -static -Wl,-Ttext=0x0 $(PAYLOAD) -o $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)
	$(HOST_GCC) -O2 $(COMPRESS_RAW_SRC) $(LZ4_SRC) -o $(COMPRESS)
	./$(COMPRESS) $(BIN) $(CMP)
# 	$(LZ4) -9 $(BIN) $(CMP)
	xxd -i $(CMP) > $(INC)

loader: $(INC)
$(LOADER): $(LOADER_SRC) $(INC)
	$(GCC) -static -O2 $(LOADER_SRC) $(LZ4_SRC) -x c $(INC) -o $(LOADER)

run: $(LOADER_SRC) $(PAYLOAD) $(LZ4_SRC)
	$(QEMU) ./$(LOADER)

clean:
	@rm -rf $(ELF) $(BIN) $(CMP) $(INC) $(LOADER) $(COMPRESS)

