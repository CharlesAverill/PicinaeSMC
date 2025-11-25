#include "lang.h"
#include "rv.h"
#include <stdlib.h>

// Assembler mapping
// Bit       0: opcode
// Bits   1-15: register r
// Bits  16-23: jump target 1
// Bits  24-31: jump target 2

// Opcodes
//   INC: 0
// JZDEC: 1

uint32_t encode_instruction(Instruction *i) {
    uint8_t opcode;
    reg r;
    addr jt1, jt2;

    switch (i->type) {
        case IINC: {
            opcode = 0;
            r = i->instr.inc.r;
            jt1 = i->instr.inc.z;
            jt2 = 0;
            break;
        }

        case IJZDEC: {
            opcode = 1;
            r = i->instr.jzdec.r;
            jt1 = i->instr.jzdec.zt;
            jt2 = i->instr.jzdec.zf;
            break;
        }

        default: {
            opcode = 1;
            r = 0x7fff;
            jt1 = __UINT8_MAX__;
            jt2 = __UINT8_MAX__;
            break;
        }
    };

    return (opcode & 0x1) | ((r & 0x7fff) << 1) | (jt1 << 16) | (jt2 << 24);
}

RVInstructionSequence* assemble(uint32_t encoding) {
    RVInstructionSequence* out = malloc(sizeof(RVInstructionSequence));

    switch (encoding & 0x1) {
        // INC r, z -> lbu t0, r(s0); addi t0, t0, 1; sb t0, r(s0); beq x0, x0, offset(z)
        case 0: {
            reg r = (encoding >> 1) & 0x7fff;
            addr z = (encoding >> 16) & 0xff;

            out->num_instrs = 4;
            out->instrs[0] = LBU(t0, s0, r);
            out->instrs[1] = ADDI(t0, t0, 1);
            out->instrs[2] = SB(t0, s0, r);
            out->instrs[3] = BEQ(0, 0, z);
            break;
        }

        // JZDEC r, zt, zf -> lbu t0, r(s0); addi t0, t0, 1; sb t0, r(s0); beq t0, x0, offset(zt); beq x0, x0, offset(zf)
        case 1: {            
            reg r = (encoding >> 1) & 0x7fff;
            addr zt = (encoding >> 16) & 0xff;
            addr zf = (encoding >> 24) & 0xff;

            out->num_instrs = 5;

            out->instrs[0] = LBU(t0, s0, r);
            out->instrs[1] = ADDI(t0, t0, -1);
            out->instrs[2] = SB(t0, s0, r);
            out->instrs[3] = BEQ(t0, 0, zt);
            out->instrs[4] = BEQ(0, 0, zf);
            break;
        }
    }

    return out;
}

void run(program *p) {
    
}
