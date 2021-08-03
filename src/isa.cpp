#include "globals.hpp"

#include "cpu.hpp"

#define MAGIC_CONST 0xFF

// Addressing modes

// Accumulator
#define acc(op) { dummyNextByte(); reg_a = op(reg_a); instr.cycle = 0; break; }
// Immediate
#define imm(op) { op(nextByte()); instr.cycle = 0; break; }
// Implied
#define imp(op) { dummyNextByte(); op(); instr.cycle = 0; break; }
// Zero Page
#define zpg_r(op) {\
switch (instr.cycle){\
    case 2: instr.address = nextByte(); break;\
    case 3: op(read(instr.address)); instr.cycle = 0; break;\
} break; }
#define zpg_w(op) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: write(instr.address, op()); instr.cycle = 0; break;\
} break; }
#define zpg_rmw(op) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: instr.value = read(instr.address); break;\
    case 4: write(instr.address, instr.value); instr.value = op(instr.value); break;\
    case 5: write(instr.address, instr.value); instr.cycle = 0; break;\
} break; }
// Zero Page, X- or Y-indexed
#define zpg_i_r(op, i) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: read(instr.address); instr.address += i; instr.address &= 0x00FF; break;\
    case 4: op(read(instr.address)); instr.cycle = 0; break;\
} break; }
#define zpg_i_w(op, i) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: read(instr.address); instr.address += i; instr.address &= 0x00FF; break;\
    case 4: write(instr.address, op()); instr.cycle = 0; break;\
} break; }
#define zpg_i_rmw(op, i) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: read(instr.address); instr.address += i; instr.address &= 0x00FF; break;\
    case 4: instr.value = read(instr.address); break;\
    case 5: write(instr.address, instr.value); instr.value = op(instr.value); break;\
    case 6: write(instr.address, instr.value); instr.cycle = 0; break;\
} break; }
// Absolute
#define abs_r(op) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: instr.address |= nextByte() << 8; break;\
    case 4: op(read(instr.address)); instr.cycle = 0; break;\
} break; }
#define abs_w(op) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: instr.address |= nextByte() << 8; break;\
    case 4: write(instr.address, op()); instr.cycle = 0; break;\
} break; }
#define abs_rmw(op) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3: instr.address |= nextByte() << 8; break;\
    case 4: instr.value = read(instr.address); break;\
    case 5: write(instr.address, instr.value); instr.value = op(instr.value); break;\
    case 6: write(instr.address, instr.value); instr.cycle = 0; break;\
} break; }
// Absolute, X- or Y-indexed
#define abs_i_r(op, i) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3:\
        {instr.address |= nextByte() << 8;\
        uword addr_low = i + (instr.address & 0x00FF);\
        instr.page_cross = addr_low > 0x00FF;\
        instr.address &= 0xFF00;\
        instr.address |= addr_low & 0x00FF;}\
        break;\
    case 4:\
        if (instr.page_cross) { read(instr.address); instr.address += 0x0100; }\
        else { op(read(instr.address)); instr.cycle = 0; }\
        break;\
    case 5: op(read(instr.address)); instr.cycle = 0; break;\
} break; }
#define abs_i_w(op, i) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3:\
        {instr.address |= nextByte() << 8;\
        uword addr_low = i + (instr.address & 0x00FF);\
        instr.page_cross = addr_low > 0x00FF;\
        instr.address &= 0xFF00;\
        instr.address |= addr_low & 0x00FF;}\
        break;\
    case 4: read(instr.address); if (instr.page_cross) instr.address += 0x0100; break;\
    case 5: write(instr.address, op()); instr.cycle = 0; break;\
} break; }
#define abs_i_rmw(op, i) {\
switch (instr.cycle) {\
    case 2: instr.address = nextByte(); break;\
    case 3:\
        {instr.address |= nextByte() << 8;\
        uword addr_low = i + (instr.address & 0x00FF);\
        instr.page_cross = addr_low > 0x00FF;\
        instr.address &= 0xFF00;\
        instr.address |= addr_low & 0x00FF;}\
        break;\
    case 4: read(instr.address); if (instr.page_cross) instr.address += 0x0100; break;\
    case 5: instr.value = read(instr.address); break;\
    case 6: write(instr.address, instr.value); instr.value = op(instr.value); break;\
    case 7: write(instr.address, instr.value); instr.cycle = 0; break;\
} break; }
// Relative
#define rel(op) {\
switch (instr.cycle) {\
    case 2:\
        instr.value = nextByte();\
        instr.branch_taken = op();\
        if (!instr.branch_taken) instr.cycle = 0;\
        break;\
    case 3:\
        dummyNextByte();\
        instr.address = reg_pc + static_cast<word>(static_cast<byte>(instr.value));\
        {\
            ubyte pcl = reg_pc & 0x00FF;\
            pcl += instr.value;\
            reg_pc &= 0xFF00;\
            reg_pc |= pcl;\
            instr.page_cross = instr.address != reg_pc;\
        }\
        if (!instr.page_cross) instr.cycle = 0;\
        break;\
    case 4:\
        dummyNextByte();\
        reg_pc = instr.address;\
        instr.cycle = 0;\
        break;\
} break; }
// Indirect, X-indexed (AKA indexed indirect)
#define ind_x_r(op) {\
switch (instr.cycle) {\
    case 2: instr.value = nextByte(); break;\
    case 3: read(instr.value); instr.value += reg_x; break;\
    case 4: instr.address = read(instr.value); break;\
    case 5: instr.address |= read(static_cast<ubyte>(instr.value+1)) << 8; break;\
    case 6: op(read(instr.address)); instr.cycle = 0; break;\
} break; }
#define ind_x_w(op) {\
switch (instr.cycle) {\
    case 2: instr.value = nextByte(); break;\
    case 3: read(instr.value); instr.value += reg_x; break;\
    case 4: instr.address = read(instr.value); break;\
    case 5: instr.address |= read(static_cast<ubyte>(instr.value+1)) << 8; break;\
    case 6: write(instr.address, op()); instr.cycle = 0; break;\
} break; }
#define ind_x_rmw(op) {\
switch (instr.cycle) {\
    case 2: instr.value = nextByte(); break;\
    case 3: read(instr.value); instr.value += reg_x; break;\
    case 4: instr.address = read(instr.value); break;\
    case 5: instr.address |= read(static_cast<ubyte>(instr.value+1)) << 8; break;\
    case 6: instr.value = read(instr.address); break;\
    case 7: write(instr.address, instr.value); instr.value = op(instr.value); break;\
    case 8: write(instr.address, instr.value); instr.cycle = 0; break;\
} break; }
// Indirect, Y-indexed (AKA indirect indexed)
#define ind_y_r(op) {\
switch (instr.cycle) {\
    case 2: instr.value = nextByte(); break;\
    case 3: instr.address = read(instr.value); break;\
    case 4:\
        {instr.address |= read(ubyte(instr.value+1)) << 8;\
        uword addr_low = reg_y + (instr.address & 0x00FF);\
        instr.page_cross = addr_low > 0x00FF;\
        instr.address &= 0xFF00;\
        instr.address |= addr_low & 0x00FF;}\
        break;\
    case 5:\
        if (instr.page_cross) { read(instr.address); instr.address += 0x0100; }\
        else { op(read(instr.address)); instr.cycle = 0; }\
        break;\
    case 6: op(read(instr.address)); instr.cycle = 0; break;\
} break; }
#define ind_y_w(op) {\
switch (instr.cycle) {\
    case 2: instr.value = nextByte(); break;\
    case 3: instr.address = read(instr.value); break;\
    case 4:\
        {instr.address |= read(ubyte(instr.value+1)) << 8;\
        uword addr_low = reg_y + (instr.address & 0x00FF);\
        instr.page_cross = addr_low > 0x00FF;\
        instr.address &= 0xFF00;\
        instr.address |= addr_low & 0x00FF;}\
        break;\
    case 5:\
        read(instr.address);\
        if (instr.page_cross) instr.address += 0x0100;\
        break;\
    case 6: write(instr.address, op()); instr.cycle = 0; break;\
} break; }
#define ind_y_rmw(op) {\
switch (instr.cycle) {\
    case 2: instr.value = nextByte(); break;\
    case 3: instr.address = read(instr.value); break;\
    case 4:\
        {instr.address |= read(ubyte(instr.value+1)) << 8;\
        uword addr_low = reg_y + (instr.address & 0x00FF);\
        instr.page_cross = addr_low > 0x00FF;\
        instr.address &= 0xFF00;\
        instr.address |= addr_low & 0x00FF;}\
        break;\
    case 5:\
        instr.value = read(instr.address);\
        if (instr.page_cross) instr.address += 0x0100;\
        break;\
    case 6: instr.value = read(instr.address); break;\
    case 7: write(instr.address, instr.value); instr.value = op(instr.value); break;\
    case 8: write(instr.address, instr.value); instr.cycle = 0; break;\
} break; }

void CPU::executeInstruction()
{
    switch (instr.code)
    {
        case 0x00: BRK(); break;
        case 0x01: ind_x_r(ORA);
        case 0x02: STP(); break;
        case 0x03: ind_x_rmw(SLO);
        case 0x04: zpg_r(NOP);
        case 0x05: zpg_r(ORA);
        case 0x06: zpg_rmw(ASL);
        case 0x07: zpg_rmw(SLO);
        case 0x08: PHP(); break;
        case 0x09: imm(ORA);
        case 0x0A: acc(ASL);
        case 0x0B: imm(ANC);
        case 0x0C: abs_r(NOP);
        case 0x0D: abs_r(ORA);
        case 0x0E: abs_rmw(ASL);
        case 0x0F: abs_rmw(SLO);
        case 0x10: rel(BPL);
        case 0x11: ind_y_r(ORA);
        case 0x12: STP(); break;
        case 0x13: ind_y_rmw(SLO);
        case 0x14: zpg_i_r(NOP, reg_x);
        case 0x15: zpg_i_r(ORA, reg_x);
        case 0x16: zpg_i_rmw(ASL, reg_x);
        case 0x17: zpg_i_rmw(SLO, reg_x);
        case 0x18: imp(CLC);
        case 0x19: abs_i_r(ORA, reg_y);
        case 0x1A: imp(NOP);
        case 0x1B: abs_i_rmw(SLO, reg_y);
        case 0x1C: abs_i_r(NOP, reg_x);
        case 0x1D: abs_i_r(ORA, reg_x);
        case 0x1E: abs_i_rmw(ASL, reg_x);
        case 0x1F: abs_i_rmw(SLO, reg_x);
        case 0x20: JSR(); break;
        case 0x21: ind_x_r(AND);
        case 0x22: STP(); break;
        case 0x23: ind_x_rmw(RLA);
        case 0x24: zpg_r(BIT);
        case 0x25: zpg_r(AND);
        case 0x26: zpg_rmw(ROL);
        case 0x27: zpg_rmw(RLA);
        case 0x28: PLP(); break;
        case 0x29: imm(AND);
        case 0x2A: acc(ROL);
        case 0x2B: imm(ANC);
        case 0x2C: abs_r(BIT);
        case 0x2D: abs_r(AND);
        case 0x2E: abs_rmw(ROL);
        case 0x2F: abs_rmw(RLA);
        case 0x30: rel(BMI);
        case 0x31: ind_y_r(AND);
        case 0x32: STP(); break;
        case 0x33: ind_y_rmw(RLA);
        case 0x34: zpg_i_r(NOP, reg_x);
        case 0x35: zpg_i_r(AND, reg_x);
        case 0x36: zpg_i_rmw(ROL, reg_x);
        case 0x37: zpg_i_rmw(RLA, reg_x);
        case 0x38: imp(SEC);
        case 0x39: abs_i_r(AND, reg_y);
        case 0x3A: imp(NOP);
        case 0x3B: abs_i_rmw(RLA, reg_y);
        case 0x3C: abs_i_r(NOP, reg_x);
        case 0x3D: abs_i_r(AND, reg_x);
        case 0x3E: abs_i_rmw(ROL, reg_x);
        case 0x3F: abs_i_rmw(RLA, reg_x);
        case 0x40: RTI(); break;
        case 0x41: ind_x_r(EOR);
        case 0x42: STP(); break;
        case 0x43: ind_x_rmw(SRE);
        case 0x44: zpg_r(NOP);
        case 0x45: zpg_r(EOR);
        case 0x46: zpg_rmw(LSR);
        case 0x47: zpg_rmw(SRE);
        case 0x48: PHA(); break;
        case 0x49: imm(EOR);
        case 0x4A: acc(LSR);
        case 0x4B: imm(ALR);
        case 0x4C: JMP(); break;
        case 0x4D: abs_r(EOR);
        case 0x4E: abs_rmw(LSR);
        case 0x4F: abs_rmw(SRE);
        case 0x50: rel(BVC);
        case 0x51: ind_y_r(EOR);
        case 0x52: STP(); break;
        case 0x53: ind_y_rmw(SRE);
        case 0x54: zpg_i_r(NOP, reg_x);
        case 0x55: zpg_i_r(EOR, reg_x);
        case 0x56: zpg_i_rmw(LSR, reg_x);
        case 0x57: zpg_i_rmw(SRE, reg_x);
        case 0x58: imp(CLI);
        case 0x59: abs_i_r(EOR, reg_y);
        case 0x5A: imp(NOP);
        case 0x5B: abs_i_rmw(SRE, reg_y);
        case 0x5C: abs_i_r(NOP, reg_x);
        case 0x5D: abs_i_r(EOR, reg_x);
        case 0x5E: abs_i_rmw(LSR, reg_x);
        case 0x5F: abs_i_rmw(SRE, reg_x);
        case 0x60: RTS(); break;
        case 0x61: ind_x_r(ADC);
        case 0x62: STP(); break;
        case 0x63: ind_x_rmw(RRA);
        case 0x64: zpg_r(NOP);
        case 0x65: zpg_r(ADC);
        case 0x66: zpg_rmw(ROR);
        case 0x67: zpg_rmw(RRA);
        case 0x68: PLA(); break;
        case 0x69: imm(ADC);
        case 0x6A: acc(ROR);
        case 0x6B: imm(ARR);
        case 0x6C: JMP_IND(); break;
        case 0x6D: abs_r(ADC);
        case 0x6E: abs_rmw(ROR);
        case 0x6F: abs_rmw(RRA);
        case 0x70: rel(BVS);
        case 0x71: ind_y_r(ADC);
        case 0x72: STP(); break;
        case 0x73: ind_y_rmw(RRA);
        case 0x74: zpg_i_r(NOP, reg_x);
        case 0x75: zpg_i_r(ADC, reg_x);
        case 0x76: zpg_i_rmw(ROR, reg_x);
        case 0x77: zpg_i_rmw(RRA, reg_x);
        case 0x78: imp(SEI);
        case 0x79: abs_i_r(ADC, reg_y);
        case 0x7A: imp(NOP);
        case 0x7B: abs_i_rmw(RRA, reg_y);
        case 0x7C: abs_i_r(NOP, reg_x);
        case 0x7D: abs_i_r(ADC, reg_x);
        case 0x7E: abs_i_rmw(ROR, reg_x);
        case 0x7F: abs_i_rmw(RRA, reg_x);
        case 0x80: imm(NOP);
        case 0x81: ind_x_w(STA);
        case 0x82: imm(NOP);
        case 0x83: ind_x_w(SAX);
        case 0x84: zpg_w(STY);
        case 0x85: zpg_w(STA);
        case 0x86: zpg_w(STX);
        case 0x87: zpg_w(SAX)
        case 0x88: imp(DEY);
        case 0x89: imm(NOP);
        case 0x8A: imp(TXA);
        case 0x8B: imm(ANE);
        case 0x8C: abs_w(STY);
        case 0x8D: abs_w(STA);
        case 0x8E: abs_w(STX);
        case 0x8F: abs_w(SAX);
        case 0x90: rel(BCC);
        case 0x91: ind_y_w(STA);
        case 0x92: STP(); break;
        case 0x93: ind_y_w(SHA);
        case 0x94: zpg_i_w(STY, reg_x);
        case 0x95: zpg_i_w(STA, reg_x);
        case 0x96: zpg_i_w(STX, reg_y);
        case 0x97: zpg_i_w(SAX, reg_y);
        case 0x98: imp(TYA);
        case 0x99: abs_i_w(STA, reg_y);
        case 0x9A: imp(TXS);
        case 0x9B: abs_i_w(TAS, reg_y);
        case 0x9C: abs_i_w(SHY, reg_x);
        case 0x9D: abs_i_w(STA, reg_x);
        case 0x9E: abs_i_w(SHX, reg_y);
        case 0x9F: abs_i_w(SHA, reg_y);
        case 0xA0: imm(LDY);
        case 0xA1: ind_x_r(LDA);
        case 0xA2: imm(LDX);
        case 0xA3: ind_x_r(LAX);
        case 0xA4: zpg_r(LDY);
        case 0xA5: zpg_r(LDA);
        case 0xA6: zpg_r(LDX);
        case 0xA7: zpg_r(LAX);
        case 0xA8: imp(TAY);
        case 0xA9: imm(LDA);
        case 0xAA: imp(TAX);
        case 0xAB: imm(LXA);
        case 0xAC: abs_r(LDY);
        case 0xAD: abs_r(LDA);
        case 0xAE: abs_r(LDX);
        case 0xAF: abs_r(LAX);
        case 0xB0: rel(BCS);
        case 0xB1: ind_y_r(LDA);
        case 0xB2: STP(); break;
        case 0xB3: ind_y_r(LAX);
        case 0xB4: zpg_i_r(LDY, reg_x);
        case 0xB5: zpg_i_r(LDA, reg_x);
        case 0xB6: zpg_i_r(LDX, reg_y);
        case 0xB7: zpg_i_r(LAX, reg_y);
        case 0xB8: imp(CLV);
        case 0xB9: abs_i_r(LDA, reg_y);
        case 0xBA: imp(TSX);
        case 0xBB: abs_i_r(LAS, reg_y);
        case 0xBC: abs_i_r(LDY, reg_x);
        case 0xBD: abs_i_r(LDA, reg_x);
        case 0xBE: abs_i_r(LDX, reg_y);
        case 0xBF: abs_i_r(LAX, reg_y);
        case 0xC0: imm(CPY);
        case 0xC1: ind_x_r(CMP);
        case 0xC2: imm(NOP);
        case 0xC3: ind_x_rmw(DCP);
        case 0xC4: zpg_r(CPY);
        case 0xC5: zpg_r(CMP);
        case 0xC6: zpg_rmw(DEC);
        case 0xC7: zpg_rmw(DCP);
        case 0xC8: imp(INY);
        case 0xC9: imm(CMP);
        case 0xCA: imp(DEX);
        case 0xCB: imm(SBX);
        case 0xCC: abs_r(CPY);
        case 0xCD: abs_r(CMP);
        case 0xCE: abs_rmw(DEC);
        case 0xCF: abs_rmw(DCP);
        case 0xD0: rel(BNE);
        case 0xD1: ind_y_r(CMP);
        case 0xD2: STP(); break;
        case 0xD3: ind_y_rmw(DCP);
        case 0xD4: zpg_i_r(NOP, reg_x);
        case 0xD5: zpg_i_r(CMP, reg_x);
        case 0xD6: zpg_i_rmw(DEC, reg_x);
        case 0xD7: zpg_i_rmw(DCP, reg_x);
        case 0xD8: imp(CLD);
        case 0xD9: abs_i_r(CMP, reg_y);
        case 0xDA: imp(NOP);
        case 0xDB: abs_i_rmw(DCP, reg_y);
        case 0xDC: abs_i_r(NOP, reg_x);
        case 0xDD: abs_i_r(CMP, reg_x);
        case 0xDE: abs_i_rmw(DEC, reg_x);
        case 0xDF: abs_i_rmw(DCP, reg_x);
        case 0xE0: imm(CPX);
        case 0xE1: ind_x_r(SBC);
        case 0xE2: imm(NOP);
        case 0xE3: ind_x_rmw(ISC);
        case 0xE4: zpg_r(CPX);
        case 0xE5: zpg_r(SBC);
        case 0xE6: zpg_rmw(INC);
        case 0xE7: zpg_rmw(ISC);
        case 0xE8: imp(INX);
        case 0xE9: imm(SBC);
        case 0xEA: imp(NOP);
        case 0xEB: imm(SBC); // Not a mistake, there are 2 of these
        case 0xEC: abs_r(CPX);
        case 0xED: abs_r(SBC);
        case 0xEE: abs_rmw(INC);
        case 0xEF: abs_rmw(ISC);
        case 0xF0: rel(BEQ);
        case 0xF1: ind_y_r(SBC);
        case 0xF2: STP(); break;
        case 0xF3: ind_y_rmw(ISC);
        case 0xF4: zpg_i_r(NOP, reg_x);
        case 0xF5: zpg_i_r(SBC, reg_x);
        case 0xF6: zpg_i_rmw(INC, reg_x);
        case 0xF7: zpg_i_rmw(ISC, reg_x);
        case 0xF8: imp(SED);
        case 0xF9: abs_i_r(SBC, reg_y);
        case 0xFA: imp(NOP);
        case 0xFB: abs_i_rmw(ISC, reg_y);
        case 0xFC: abs_i_r(NOP, reg_x);
        case 0xFD: abs_i_r(SBC, reg_x);
        case 0xFE: abs_i_rmw(INC, reg_x);
        case 0xFF: abs_i_rmw(ISC, reg_x);
    }
}

// Interrupts

void CPU::BRK() noexcept
{
    switch (instr.cycle) 
    {
        case 2: nextByte(); break;
        case 3: push(reg_pc >> 8); break;
        case 4: push(reg_pc & 0x00FF); break;
        // Interrupt hijacking comes into play here
        case 5: push(reg_sr.reg | 0x30); reg_sr.i = true; break;
        case 6: reg_pc = read(0xFFFE); break;
        case 7: reg_pc |= read(0xFFFF) << 8; instr.cycle = 0; break;
    }
}

void CPU::IRQ() noexcept
{
    switch (instr.cycle)
    {
        case 1: dummyNextByte(); instr.code = 0x00; break;
        case 2: dummyNextByte(); break;
        case 3: push(reg_pc >> 8); break;       // Push PCH
        case 4: push(reg_pc & 0x00FF); break;   // Push PCL
        // Interrupt hijacking comes into play here
        case 5: push((reg_sr.reg & 0xCF) | 0x20); break;
        case 6: reg_pc = read(0xFFFE); reg_sr.i = true; break;
        case 7: reg_pc |= read(0xFFFF) << 8; instr.cycle = 0; break;
    }
}

void CPU::NMI() noexcept
{
    switch (instr.cycle)
    {
        case 1: dummyNextByte(); instr.code = 0x00; break;
        case 2: dummyNextByte(); break;
        case 3: push(reg_pc >> 8); break;       // Push PCH
        case 4: push(reg_pc & 0x00FF); break;   // Push PCL
        // Interrupt hijacking comes into play here
        case 5: push((reg_sr.reg & 0xCF) | 0x20); break;
        case 6: reg_pc = read(0xFFFA); reg_sr.i = true; break;
        case 7: reg_pc |= read(0xFFFB) << 8; instr.cycle = 0; break;
    }
}

// Instructions accessing the stack

void CPU::RTI() noexcept
{
    switch (instr.cycle) 
    {
        case 2: dummyNextByte(); break;
        case 3: break; // SP increment would happen here, but NNES does it within pop()
        case 4: reg_sr.reg = pop() & 0xCF; break; // Ignore bits 4 and 5
        case 5: reg_pc &= 0xFF00; reg_pc |= pop(); break;
        case 6: reg_pc &= 0x00FF; reg_pc |= (pop() << 8); instr.cycle = 0; break;
    }
}

void CPU::RTS() noexcept
{
    switch (instr.cycle) 
    {
        case 2: dummyNextByte(); break;
        case 3: break;
        case 4: reg_pc &= 0xFF00; reg_pc |= pop(); break;
        case 5: reg_pc &= 0x00FF; reg_pc |= (pop() << 8); break;
        case 6: reg_pc++; instr.cycle = 0; break;
    } 
}

void CPU::PHA() noexcept
{
    switch (instr.cycle) 
    {
        case 2: dummyNextByte(); break;
        case 3: push(reg_a); instr.cycle = 0; break;
    }
}

void CPU::PHP() noexcept
{
    switch (instr.cycle) 
    {
        case 2: dummyNextByte(); break;
        case 3: push(reg_sr.reg | 0x30); instr.cycle = 0; break;
    }
}

void CPU::PLA() noexcept
{
    switch (instr.cycle) 
    {
        case 2: dummyNextByte(); break;
        case 3: break; // increment SP
        case 4:
            reg_a = pop();
            reg_sr.z = reg_a == 0;
            reg_sr.n = static_cast<bool>(reg_a & 0x80);
            instr.cycle = 0; 
            break;
    }
}

void CPU::PLP() noexcept
{
    switch (instr.cycle) 
    {
        case 2: dummyNextByte(); break;
        case 3: break; // increment SP
        case 4: reg_sr.reg = pop() & 0xCF; instr.cycle = 0; break;
    }
}

void CPU::JSR() noexcept
{
    switch (instr.cycle) 
    {
        case 2: instr.address = nextByte(); break;
        case 3: break; // TODO internal operation? buffer addr low
        case 4: push(reg_pc >> 8); break;
        case 5: push(reg_pc & 0x00FF); break;
        case 6: reg_pc = (nextByte() << 8) | instr.address; instr.cycle = 0; break;
    }
}

void CPU::JMP() noexcept
{
    switch (instr.cycle) 
    {
        case 2: instr.address = nextByte(); break;
        case 3: reg_pc = (nextByte() << 8) | instr.address; instr.cycle = 0; break;
    }
}

void CPU::JMP_IND() noexcept
{
    switch (instr.cycle) 
    {
        case 2: instr.address = nextByte(); break;
        case 3: instr.address |= (nextByte() << 8); break;
        case 4: instr.value = read(instr.address); break; // Using instr.value as a latch
        case 5: // Page boundary crossing not handled
            reg_pc = (read((instr.address & 0xFF00) | ((instr.address+1) & 0x00FF)) << 8) | instr.value; 
            instr.cycle = 0; 
            break;
    }
}

// Branch instructions

bool CPU::BCC() noexcept { return !reg_sr.c; }
bool CPU::BCS() noexcept { return reg_sr.c; }
bool CPU::BNE() noexcept { return !reg_sr.z; }
bool CPU::BEQ() noexcept { return reg_sr.z; }
bool CPU::BPL() noexcept { return !reg_sr.n; }
bool CPU::BMI() noexcept { return reg_sr.n; }
bool CPU::BVC() noexcept { return !reg_sr.v; }
bool CPU::BVS() noexcept { return reg_sr.v; }

// Read instructions

// Credit for branchless ADC/SBC algorithm goes to: https://near.sh/articles/cpu/alu
void CPU::ADC(ubyte value) noexcept
{
    ubyte result = value + reg_a + reg_sr.c;
    ubyte carries = value ^ reg_a ^ result;
    ubyte overflows = (value ^ result) & (reg_a ^ result);
    reg_sr.c = static_cast<bool>((carries ^ overflows) & 0x80);
    reg_sr.z = result == 0;
    reg_sr.v = static_cast<bool>(overflows & 0x80);
    reg_sr.n = static_cast<bool>(result & 0x80);
    reg_a = result;
}

void CPU::ALR(ubyte value) noexcept
{
    ubyte result = reg_a & value;
    reg_sr.c = static_cast<bool>(result & 1);
    reg_a = LSR(result);
}

void CPU::ANC(ubyte value) noexcept
{
    AND(value);
    reg_sr.c = reg_sr.n;
}

void CPU::AND(ubyte value) noexcept
{
    ubyte result = value & reg_a;
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    reg_a = result;
}

void CPU::ANE(ubyte value) noexcept
{
    reg_x = (reg_a | MAGIC_CONST) & reg_x & value;
    reg_a = reg_x;
}

void CPU::ARR(ubyte value) noexcept
{
    AND(value);
    reg_a = ROR(reg_a);
    reg_sr.c = static_cast<bool>(reg_a & 0x40);
    reg_sr.v = reg_sr.c ^ static_cast<bool>(reg_a & 0x20);
}

void CPU::BIT(ubyte value) noexcept
{
    ubyte result = reg_a & value;
    reg_sr.z = result == 0;
    reg_sr.v = static_cast<bool>(value & 0x40);
    reg_sr.n = static_cast<bool>(value & 0x80);
}

void CPU::CMP(ubyte value) noexcept
{
    reg_sr.c = reg_a >= value;
    reg_sr.z = reg_a == value;
    reg_sr.n = static_cast<bool>((reg_a - value) & 0x80);
}

void CPU::CPX(ubyte value) noexcept
{
    reg_sr.c = reg_x >= value;
    reg_sr.z = reg_x == value;
    reg_sr.n = static_cast<bool>((reg_x - value) & 0x80);
}

void CPU::CPY(ubyte value) noexcept
{
    reg_sr.c = reg_y >= value;
    reg_sr.z = reg_y == value;
    reg_sr.n = static_cast<bool>((reg_y - value) & 0x80);
}

void CPU::EOR(ubyte value) noexcept
{
    ubyte result = value ^ reg_a;
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    reg_a = result;
}

void CPU::LAS(ubyte value) noexcept
{
    ubyte result = value & reg_sp;
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    reg_a = result;
    reg_x = result;
    reg_sp = result;
}

void CPU::LAX(ubyte value) noexcept
{
    LDA(value);
    LDX(value);
}

void CPU::LDA(ubyte value) noexcept
{
    reg_sr.z = value == 0;
    reg_sr.n = static_cast<bool>(value & 0x80);
    reg_a = value;
}

void CPU::LDX(ubyte value) noexcept
{
    reg_sr.z = value == 0;
    reg_sr.n = static_cast<bool>(value & 0x80);
    reg_x = value;
}

void CPU::LDY(ubyte value) noexcept
{
    reg_sr.z = value == 0;
    reg_sr.n = static_cast<bool>(value & 0x80);
    reg_y = value;
}

void CPU::LXA(ubyte value) noexcept
{
    reg_a = (reg_a | MAGIC_CONST) & value;
    reg_x = reg_a;
    reg_sr.z = reg_a == 0;
    reg_sr.n = static_cast<bool>(reg_a & 0x80);
}

void CPU::NOP([[maybe_unused]] ubyte value) noexcept { return; }

void CPU::ORA(ubyte value) noexcept
{
    ubyte result = value | reg_a;
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    reg_a = result;
}

void CPU::SBC(ubyte value) noexcept { ADC(~value); }

void CPU::SBX(ubyte value) noexcept
{
    ubyte result = reg_a & reg_x;
    reg_sr.c = result >= value;
    result -= value;
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    reg_x = result;
}

// Write instructions

ubyte CPU::SAX() noexcept { return reg_a & reg_x; }

ubyte CPU::SHA() noexcept // TODO RDY line instability?
{
    if (instr.page_cross)
    {
        ubyte addr_high = instr.address >> 8;
        instr.address &= 0x00FF;
        instr.address |= (reg_a & reg_x & addr_high) << 8;
    }
    return reg_a & reg_x & ((instr.address >> 8) + 1);
}

ubyte CPU::SHX() noexcept // TODO RDY line instability?
{
    if (instr.page_cross)
    {
        ubyte addr_high = instr.address >> 8;
        instr.address &= 0x00FF;
        instr.address |= (reg_x & addr_high) << 8;
    }
    return reg_x & ((instr.address >> 8) + 1);
}

ubyte CPU::SHY() noexcept // TODO RDY line instability?
{
    if (instr.page_cross)
    {
        ubyte addr_high = instr.address >> 8;
        instr.address &= 0x00FF;
        instr.address |= (reg_y & addr_high) << 8;
    }
    return reg_y & ((instr.address >> 8) + 1);
}

ubyte CPU::STA() noexcept { return reg_a; }
ubyte CPU::STX() noexcept { return reg_x; }
ubyte CPU::STY() noexcept { return reg_y; }

ubyte CPU::TAS() noexcept // TODO RDY line instability?
{
    reg_sp = reg_a & reg_x;
    if (instr.page_cross)
    {
        ubyte addr_high = instr.address >> 8;
        instr.address &= 0x00FF;
        instr.address |= (reg_a & reg_x & addr_high) << 8;
    }
    return reg_a & reg_x & ((instr.address >> 8) + 1);
}

// Read-Modify-Write instructions

ubyte CPU::ASL(ubyte value) noexcept
{
    ubyte result = value << 1;
    reg_sr.c = static_cast<bool>(value & 0x80);
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    return result;
}

ubyte CPU::DCP(ubyte value) noexcept
{
    ubyte result = DEC(value);
    CMP(result);
    return result;
}

ubyte CPU::DEC(ubyte value) noexcept
{
    value--;
    reg_sr.z = value == 0;
    reg_sr.n = static_cast<bool>(value & 0x80);
    return value;
}

ubyte CPU::INC(ubyte value) noexcept
{
    value++;
    reg_sr.z = value == 0;
    reg_sr.n = static_cast<bool>(value & 0x80);
    return value;
}

ubyte CPU::ISC(ubyte value) noexcept
{
    ubyte result = INC(value);
    SBC(result);
    return result;
}

ubyte CPU::LSR(ubyte value) noexcept
{
    ubyte result = value >> 1;
    assert(!(result & 0x80));
    reg_sr.c = static_cast<bool>(value & 1);
    reg_sr.z = result == 0;
    reg_sr.n = false; // Result should never be negative
    return result;
}

ubyte CPU::RLA(ubyte value) noexcept
{
    ubyte result = ROL(value);
    AND(result);
    return result;
}

ubyte CPU::ROL(ubyte value) noexcept
{
    ubyte result = value << 1;
    result |= reg_sr.c;
    reg_sr.c = static_cast<bool>(value & 0x80);
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    return result;
}

ubyte CPU::ROR(ubyte value) noexcept
{
    ubyte result = value >> 1;
    result |= reg_sr.c << 7;
    reg_sr.c = static_cast<bool>(value & 1);
    reg_sr.z = result == 0;
    reg_sr.n = static_cast<bool>(result & 0x80);
    return result;
}

ubyte CPU::RRA(ubyte value) noexcept
{
    ubyte result = ROR(value);
    ADC(result);
    return result;
}

ubyte CPU::SLO(ubyte value) noexcept
{
    ubyte result = ASL(value);
    ORA(result);
    return result;
}

ubyte CPU::SRE(ubyte value) noexcept
{
    ubyte result = LSR(value);
    EOR(result);
    return result;
}

// Other

void CPU::CLC() noexcept { reg_sr.c = false; }
void CPU::CLD() noexcept { reg_sr.d = false; }
void CPU::CLI() noexcept { reg_sr.i = false; }
void CPU::CLV() noexcept { reg_sr.v = false; }

void CPU::DEX() noexcept
{
    reg_x--;
    reg_sr.z = reg_x == 0;
    reg_sr.n = static_cast<bool>(reg_x & 0x80);
}

void CPU::DEY() noexcept
{
    reg_y--;
    reg_sr.z = reg_y == 0;
    reg_sr.n = static_cast<bool>(reg_y & 0x80);
}

void CPU::INX() noexcept
{
    reg_x++;
    reg_sr.z = reg_x == 0;
    reg_sr.n = static_cast<bool>(reg_x & 0x80);
}

void CPU::INY() noexcept
{
    reg_y++;
    reg_sr.z = reg_y == 0;
    reg_sr.n = static_cast<bool>(reg_y & 0x80);
}

void CPU::NOP() noexcept { return; }

void CPU::SEC() noexcept { reg_sr.c = true; }
void CPU::SED() noexcept { reg_sr.d = true; }
void CPU::SEI() noexcept { reg_sr.i = true; }
void CPU::SEV() noexcept { reg_sr.v = true; }

void CPU::TAX() noexcept
{
    reg_x = reg_a;
    reg_sr.z = reg_x == 0;
    reg_sr.n = static_cast<bool>(reg_x & 0x80);
}

void CPU::TAY() noexcept
{
    reg_y = reg_a;
    reg_sr.z = reg_y == 0;
    reg_sr.n = static_cast<bool>(reg_y & 0x80);
}

void CPU::TSX() noexcept
{
    reg_x = reg_sp;
    reg_sr.z = reg_x == 0;
    reg_sr.n = static_cast<bool>(reg_x & 0x80);
}

void CPU::TXA() noexcept
{
    reg_a = reg_x;
    reg_sr.z = reg_a == 0;
    reg_sr.n = static_cast<bool>(reg_a & 0x80);
}

void CPU::TXS() noexcept
{
    reg_sp = reg_x;
}

void CPU::TYA() noexcept
{
    reg_a = reg_y;
    reg_sr.z = reg_a == 0;
    reg_sr.n = static_cast<bool>(reg_a & 0x80);
}

void CPU::STP()
{
    std::cerr << "Error: invalid opcode " << hex(instr.code) << std::endl;
    throw std::exception();
}