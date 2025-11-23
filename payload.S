    .section .text
    .global _start

# uint32_t sum(uint32_t *arr, uint32_t len)
# RV32 calling convention: a0=arr, a1=len

_start:
    mv t0, a0        # t0 = arr pointer
    mv t1, a1        # t1 = length
    li t2, 0         # sum = 0

loop:
    beq t1, zero, done   # if len == 0 → done
    lw t3, 0(t0)         # load *arr
    add t2, t2, t3       # sum += val
    addi t0, t0, 4       # arr++
    addi t1, t1, -1      # len--
    j loop

done:
    mv a0, t2            # return sum
    ret

