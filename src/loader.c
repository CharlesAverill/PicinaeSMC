#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "lz4.h"

extern unsigned char bin_src_payload_lz4[];
extern unsigned int  bin_src_payload_lz4_len;

// mmap wrapper for RV32 (6-argument syscall)
void* mmap_exec(int size) {
    register long a0 __asm__("a0") = 0;      // addr
    register long a1 __asm__("a1") = size;   // length
    register long a2 __asm__("a2") = 7;      // prot: RWX
    register long a3 __asm__("a3") = 0x22;   // flags: MAP_PRIVATE|MAP_ANON
    register long a4 __asm__("a4") = -1;     // fd
    register long a5 __asm__("a5") = 0;      // offset
    register long a7 __asm__("a7") = 222;    // SYS_mmap

    __asm__ volatile("ecall"
                     : "+r"(a0)
                     : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a7)
                     : "memory");

    return (void*)a0;
}

uint32_t encode_jal(uint32_t pc, uint32_t target) {
    int32_t offset = target - pc;

    // Must be aligned
    if (offset & 1) {
        printf("Error: callback address not 2-byte aligned\n");
        return 0;
    }

    // Must fit in signed 21-bit immediate (shifted)
    if (offset < -(1 << 20) || offset >= (1 << 20)) {
        printf("Error: JAL offset (%p - %p) too large\n", target, pc);
        return 0;
    }

    int32_t imm = offset >> 1;  // drop low bit

    uint32_t opcode = 0x6F; // JAL

    uint32_t rd = 0; // x0 (no link)

    uint32_t inst = 0;
    inst |= (opcode & 0x7F);
    inst |= (rd & 0x1F) << 7;

    inst |= ((imm >> 12) & 0xFF) << 12;         // imm[19:12]
    inst |= ((imm >> 11) & 0x1) << 20;          // imm[11]
    inst |= ((imm >> 1) & 0x3FF) << 21;         // imm[10:1]
    inst |= ((imm >> 20) & 0x1) << 31;          // imm[20]

    return inst;
}

#define JALR0 0x000000e7
#define NOP 0x00000013

#define HEADER 0x31415926

void link(void* mem, uint32_t size) {
    uint32_t* p;
    uint32_t loc;
    // Search for header
    for(p = mem; p < (uint32_t*)mem + size; p++) {
        if (*p == HEADER) {
            printf("Header at %p\n", p);
            loc = (uint32_t)(p + 4);
        }
    }
    for(p = mem; p < (uint32_t*)mem + size; p++) {
        if (*p == JALR0) {
            *p = encode_jal((uint32_t)p, loc);
        }
    }
}

#define LEN 7
int main() {
    // Create test input array
    uint32_t arr[LEN] = {1, 2, 3, 4, 5, 100, 1000};

    // Allocate executable memory
    int max_output = 65536;
    void *mem = mmap_exec(max_output);

    // Decompress
    int out = LZ4_decompress_safe((char*)bin_src_payload_lz4, (char*)mem,
                                  bin_src_payload_lz4_len, max_output);
    if (out < 0) {
        fprintf(stderr, "Failed to decompress payload (%d)\n", out);
        return 1;
    }

    // Cast decompressed code to a function pointer
    uint32_t (*sum_fn)(uint32_t*, uint32_t) = mem;

    link(mem, out);

    uint32_t result = sum_fn(arr, LEN);
    printf("Result: %d\n", result);
    return 0;
}

