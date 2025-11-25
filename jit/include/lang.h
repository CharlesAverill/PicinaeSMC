#ifndef LANG
#define LANG

#include <stdint.h>

// Registers are 15-bit offsets into a global memory buffer (2^15 1-byte registers)
typedef uint16_t reg;
// Addresses are 8-bit offsets into the program (program has max 2^8 instructions)
typedef uint8_t addr;

// INC(r, z) - increment r and jump to z
typedef struct INC {
  // register to increment
  reg r;
  // address of next instruction to execute
  addr z;
} INC;

// JZDEC(r, zt, zf) - decrement r and jump to (r ? : zf : zt)
typedef struct JZDEC {
  // register to decrement
  reg r;
  // address to jump to if r-1 is zero
  addr zt;
  // address to jump to if r-1 is nonzero
  addr zf;
} JZDEC;

enum InstructionType { IINC, IJZDEC };

typedef struct Instruction {
  enum InstructionType type;
  union {
    INC inc;
    JZDEC jzdec;
  } instr;
} Instruction;

typedef struct program {
    Instruction* instrs;
    uint32_t num_instrs;
} program;

// Encode an instruction into a binary format
uint32_t encode_instruction(Instruction *i);

typedef struct RVInstructionSequence {
    uint32_t instrs[5];
    uint32_t num_instrs;
} RVInstructionSequence;

// Assemble a sequence of 32-bit RISC-V instruction given a binary-encoded `Instruction`
RVInstructionSequence* assemble(uint32_t encoding);

// Compile a program into a sequence of binary-encoded RISC-V instructions
RVInstructionSequence* compile(program *p);

#endif /* LANG */
