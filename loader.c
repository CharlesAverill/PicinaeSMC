#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "lz4.h"

extern unsigned char payload_lz4[];
extern unsigned int  payload_lz4_len;

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dst;
}

// int LZ4_decompress_safe(const char*, char*, int, int);

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

static long sys_exit(long x) {
    register long a0 __asm__("a0") = x;
    register long a7 __asm__("a7") = 93;
    __asm__ volatile("ecall" : : "r"(a0), "r"(a7));
    return 0;
}

#define LEN 7
int main() {
    // Create test input array
    uint32_t arr[LEN] = {1, 2, 3, 4, 5, 100, 1000};

    // Allocate executable memory
    int max_output = 65536;
    void *mem = mmap_exec(max_output);

    // Decompress
    int out = LZ4_decompress_safe((char*)payload_lz4, (char*)mem,
                                  payload_lz4_len, max_output);
    if (out < 0)
        sys_exit(111);

    // Cast decompressed code to a function pointer
    uint32_t (*sum_fn)(uint32_t*, uint32_t) = mem;

    uint32_t result = sum_fn(arr, LEN);
    printf("%d\n", result);
    return 0;
}

