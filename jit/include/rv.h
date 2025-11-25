#ifndef RV
#define RV

#define ADDI(rd, rs1, imm) \
    ((imm & 0xfff) << 20) | (rs1 << 15) | (rd << 7) | 0x13

#define LBU(rd, rs1, imm) \
    ((imm & 0xfff) << 20) | (rs1 << 15) | (4 << 12) | (rd << 7) | 0x03

#define SB(rs2, rs1, imm) \
    ((((imm & 0xfff) >> 5) & 0x7f) << 25) | (rs2 << 20) | (rs1 << 15) | ((imm & 0x1f) << 7) | 0x23

#define BEQ(rs1, rs2, imm) \
    ((imm & 0x1fff) << 31) | ((imm >> 5) & 0x3f << 25) | (rs2 << 20) | (rs1 << 15) | \
    (((imm >> 1) & 0xf) << 8) | (((imm >> 11) & 0x1) << 7) | 0x63

#define t0 0x5
#define s0 0x8

#endif /* RV */
