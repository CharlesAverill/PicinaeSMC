# Self-Modifying Code in Picinae

This repository contains some examples of [self-modifying code](https://en.wikipedia.org/wiki/Self-modifying_code)
in 32-bit RISC-V:

- [compression](./compression): A program that decompresses a compressed `.text` section into memory and then executes it
- [jit](./jit): A barebones [JIT](https://en.wikipedia.org/wiki/Just-in-time_compilation) compiler for a rudimentary language
