# compression

This repository builds an example of [self-modifying code](https://en.wikipedia.org/wiki/Self-modifying_code) for 32-bit RISC-V.
[payload.S](src/payload.S) lists a function that takes in an array of integers and returns the sum of those integers.
[compress_raw.c](src/compress_raw.c) [lz4](https://github.com/lz4/lz4)-compresses the compiled payload binary.
[loader.c](src/loader.c) decompresses the payload into memory and jumps to the entrypoint.

## Dependencies

- [RISCV GNU Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)
- [`liblz4-dev`](https://packages.debian.org/sid/liblz4-dev)
- `qemu-riscv32`:
  - [`qemu-system-misc`](https://packages.debian.org/sid/qemu-system-misc)
  - [`qemu-user`](https://packages.debian.org/sid/qemu-user)

## Targets

| Target | Description |
| --- | --- |
| `payload` | Compile and compress [payload.S](src/payload.S) |
| `loader` | Compile [loader.c](src/loader.c) |
| `run` | Run the loader with `qemu-riscv32` |
