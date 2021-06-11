#include "isa.h"

// TODO clean up code so it's simpler and formatted better

int ISA::executeOpcode(ubyte instr)
{
    // declare return values
    uword address;
    byte val;
    int cycles;

    switch (instr)
    {
        case 0x00: // BRK impl
            return BRK();

        case 0x01: // ORA X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, true);
            val = cpu.read(address);
            cycles += 1;
            return ORA(cycles, val);

        // Unofficial
        case 0x03: // SLO X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 3;
            return SLO(cycles, address);

        // Unofficial
        case 0x04: // NOP zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            return cycles;

        case 0x05: // ORA zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return ORA(cycles, val);

        case 0x06: // ASL zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return ASL(cycles, address, val, false);

        // Unofficial
        case 0x07: // SLO zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return SLO(cycles, address);

        case 0x08: // PHP impl
            return PHP();

        case 0x09: // ORA #
            std::tie(val, cycles) = Mode::immediate();
            return ORA(cycles, val);

        case 0x0A: // ASL A
            return ASL(2, 0, cpu.reg_a, true);

        // Unofficial
        case 0x0B: // ANC #
            std::tie(val, cycles) = Mode::immediate();
            return ANC(cycles, val);

        // Unofficial
        case 0x0C: // NOP abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            return cycles;

        case 0x0D: // ORA abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return ORA(cycles, val);

        case 0x0E: // ASL abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            val = cpu.read(address);
            return ASL(cycles, address, val, false);

        // Unofficial
        case 0x0F: // SLO abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return SLO(cycles, address);

        case 0x10: // BPL rel
            std::tie(address, cycles) = Mode::relative();
            return BPL(cycles, address);

        case 0x11: // ORA ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return ORA(cycles, val);

        // Unofficial
        case 0x13: // SLO ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            cycles += 3;
            return SLO(cycles, address);

        // Unofficial
        case 0x14: // NOP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return cycles;

        case 0x15: // ORA zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            val = cpu.read(address);
            return ORA(cycles, val);

        case 0x16: // ASL zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 2;
            val = cpu.read(address);
            return ASL(cycles, address, val, false);

        // Unofficial
        case 0x17: // SLO zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            return SLO(cycles, address);

        case 0x18: // CLC impl
            return CLC();

        case 0x19: // ORA abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return ORA(cycles, val);

        // Unofficial
        case 0x1A: // NOP impl
            return 2;

        // Unofficial
        case 0x1B: // SLO abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 4;
            return SLO(cycles, address);

        // Unofficial
        case 0x1C: // NOP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            return cycles;

        case 0x1D: // ORA abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return ORA(cycles, val);
        
        case 0x1E: // ASL abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            val = cpu.read(address);
            return ASL(cycles, address, val, false);

        // Unofficial
        case 0x1F: // SLO abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            return SLO(cycles, address);

        case 0x20: // JSR abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return JSR(address);

        case 0x21: // AND X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, true);
            cycles += 1;
            val = cpu.read(address);
            return AND(cycles, val);

        // Unofficial
        case 0x23: // RLA X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 3;
            return RLA(cycles, address);

        case 0x24: // BIT zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return BIT(cycles, val);

        case 0x25: // AND zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return AND(cycles, val);
        
        case 0x26: // ROL zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            val = cpu.read(address);
            return ROL(cycles, address, val, false);

        // Unofficial
        case 0x27: // RLA zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return RLA(cycles, address);

        case 0x28: // PLP impl
            return PLP();

        case 0x29: // AND #
            std::tie(val, cycles) = Mode::immediate();
            return AND(cycles, val);

        case 0x2A: // ROL A
            val = cpu.reg_a;
            return ROL(2, 0, val, true);

        // Unofficial
        case 0x2B: // ANC #
            std::tie(val, cycles) = Mode::immediate();
            return ANC(cycles, val);

        case 0x2C: // BIT abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return BIT(cycles, val);

        case 0x2D: // AND abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return AND(cycles, val);

        case 0x2E: // ROL abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            val = cpu.read(address);
            return ROL(cycles, address, val, false);

        // Unofficial
        case 0x2F: // RLA abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return RLA(cycles, address);

        case 0x30: // BMI rel
            std::tie(address, cycles) = Mode::relative();
            return BMI(cycles, address);

        case 0x31: // AND ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return AND(cycles, val);

        // Unofficial
        case 0x33: // RLA ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            cycles += 3;
            return RLA(cycles, address);

        // Unofficial
        case 0x34: // NOP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return cycles;

        case 0x35: // AND zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            val = cpu.read(address);
            return AND(cycles, val);

        case 0x36: // ROL zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            val = cpu.read(address);
            return ROL(cycles, address, val, false);

        // Unofficial
        case 0x37: // RLA zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            return RLA(cycles, address);

        case 0x38: // SEC impl
            return SEC();

        case 0x39: // AND abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return AND(cycles, val);

        // Unofficial
        case 0x3A: // NOP impl
            return 2;

        // Unofficial
        case 0x3B: // RLA abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 4;
            return RLA(cycles, address);

        // Unofficial
        case 0x3C: // NOP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            return cycles;

        case 0x3D: // AND abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return AND(cycles, val);

        case 0x3E: // ROL abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            val = cpu.read(address);
            return ROL(cycles, address, val, false);

        // Unofficial
        case 0x3F: // RLA abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            return RLA(cycles, address);

        case 0x40: // RTI impl
            return RTI();

        case 0x41: // EOR X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, true);
            cycles += 1;
            val = cpu.read(address);
            return EOR(cycles, val);

        // Unofficial
        case 0x43: // SRE X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 3;
            return SRE(cycles, address);

        // Unofficial
        case 0x44: // NOP zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            return cycles;

        case 0x45: // EOR zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return EOR(cycles, val);

        case 0x46: // LSR zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            val = cpu.read(address);
            return LSR(cycles, address, val, false);

        // Unofficial
        case 0x47: // SRE zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return SRE(cycles, address);

        case 0x48: // PHA impl
            return PHA();

        case 0x49: // EOR #
            std::tie(val, cycles) = Mode::immediate();
            return EOR(cycles, val);

        case 0x4A: // LSR A
            cycles = 2;
            val = cpu.reg_a;
            return LSR(cycles, 0, val, true);

        // Unofficial
        case 0x4B: // ALR #
            std::tie(val, cycles) = Mode::immediate();
            return ALR(cycles, val);

        case 0x4C: // JMP abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            return JMP(cycles, address);

        case 0x4D: // EOR abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return EOR(cycles, val);

        case 0x4E: // LSR abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            val = cpu.read(address);
            return LSR(cycles, address, val, false);

        // Unofficial
        case 0x4F: // SRE abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return SRE(cycles, address);

        case 0x50: // BVC rel
            std::tie(address, cycles) = Mode::relative();
            return BVC(cycles, address);

        case 0x51: // EOR ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return EOR(cycles, val);

        // Unofficial
        case 0x53: // SRE ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            cycles += 3;
            return SRE(cycles, address);

        // Unofficial
        case 0x54: // NOP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return cycles;

        case 0x55: // EOR zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            val = cpu.read(address);
            return EOR(cycles, val);

        case 0x56: // LSR zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            val = cpu.read(address);
            return LSR(cycles, address, val, false);

        // Unofficial
        case 0x57: // SRE zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            return SRE(cycles, address);

        case 0x58: // CLI impl
            return CLI();

        case 0x59: // EOR abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return EOR(cycles, val);

        // Unofficial
        case 0x5A: // NOP impl
            return 2;

        // Unofficial
        case 0x5B: // SRE abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 4;
            return SRE(cycles, address);

        // Unofficial
        case 0x5C: // NOP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            return cycles;

        case 0x5D: // EOR abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return EOR(cycles, val);

        case 0x5E: // LSR abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            val = cpu.read(address);
            return LSR(cycles, address, val, false);

        // Unofficial
        case 0x5F: // SRE abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            return SRE(cycles, address);

        case 0x60: // RTS impl
            return RTS();

        case 0x61: // ADC X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, true);
            cycles += 1;
            val = cpu.read(address);
            return ADC(cycles, val);

        // Unofficial
        case 0x63: // RRA X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 3;
            return RRA(cycles, address);

        // Unofficial
        case 0x64: // NOP zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            return cycles;

        case 0x65: // ADC zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return ADC(cycles, val);

        case 0x66: // ROR zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            val = cpu.read(address);
            return ROR(cycles, address, val, false);

        // Unofficial
        case 0x67: // RRA zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return RRA(cycles, address);

        case 0x68: // PLA impl
            return PLA();

        case 0x69: // ADC #
            std::tie(val, cycles) = Mode::immediate();
            return ADC(cycles, val);

        case 0x6A: // ROR A
            cycles = 2;
            val = cpu.reg_a;
            return ROR(cycles, 0, val, true);

        // Unofficial
        case 0x6B: // ARR #
            std::tie(val, cycles) = Mode::immediate();
            return ARR(cycles, val);

        case 0x6C: // JMP ind
            std::tie(address, cycles) = Mode::indirect(0, 0, false);
            return JMP(cycles, address);

        case 0x6D: // ADC abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return ADC(cycles, val);

        case 0x6E: // ROR abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            val = cpu.read(address);
            return ROR(cycles, address, val, false);

        // Unofficial
        case 0x6F: // RRA abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return RRA(cycles, address);

        case 0x70: // BVS rel
            std::tie(address, cycles) = Mode::relative();
            return BVS(cycles, address);

        case 0x71: // ADC ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return ADC(cycles, val);

        // Unofficial
        case 0x73: // RRA ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            cycles += 3;
            return RRA(cycles, address);

        // Unofficial
        case 0x74: // NOP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return cycles;

        case 0x75: // ADC zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            val = cpu.read(address);
            return ADC(cycles, val);

        case 0x76: // ROR zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            val = cpu.read(address);
            return ROR(cycles, address, val, false);

        // Unofficial
        case 0x77: // RRA zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            return RRA(cycles, address);

        case 0x78: // SEI impl
            return SEI();

        case 0x79: // ADC abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return ADC(cycles, val);

        // Unofficial
        case 0x7A: // NOP impl
            return 2;

        // Unofficial
        case 0x7B: // RRA abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 4;
            return RRA(cycles, address);

        // Unofficial
        case 0x7C: // NOP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            return cycles;

        case 0x7D: // ADC abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return ADC(cycles, val);

        case 0x7E: // ROR abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            val = cpu.read(address);
            return ROR(cycles, address, val, false);

        // Unofficial
        case 0x7F: // RRA abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            return RRA(cycles, address);

        // Unofficial
        case 0x80: // NOP #
            std::tie(address, cycles) = Mode::immediate();
            return cycles;

        case 0x81: // STA X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 1;
            return STA(cycles, address);

        // Unofficial
        case 0x82: // NOP #
            std::tie(address, cycles) = Mode::immediate();
            return cycles;

        // Unofficial
        case 0x83: // SAX X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 1;
            return SAX(cycles, address);

        case 0x84: // STY zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            return STY(cycles, address);

        case 0x85: // STA zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            return STA(cycles, address);

        case 0x86: // STX zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            return STX(cycles, address);

        // Unofficial
        case 0x87: // SAX zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            return SAX(cycles, address);

        case 0x88: // DEY impl
            return DEY();

        // Unofficial
        case 0x89: // NOP #
            std::tie(address, cycles) = Mode::immediate();
            return cycles;

        case 0x8A: // TXA impl
            return TXA();

        case 0x8C: // STY abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 1;
            return STY(cycles, address);

        case 0x8D: // STA abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 1;
            return STA(cycles, address);

        case 0x8E: // STX abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 1;
            return STX(cycles, address);

        // Unofficial
        case 0x8F: // SAX abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 1;
            return SAX(cycles, address);
        
        case 0x90: // BCC rel
            std::tie(address, cycles) = Mode::relative();
            return BCC(cycles, address);

        case 0x91: // STA ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            cycles += 1;
            return STA(cycles, address);

        // Unofficial
        case 0x93: // SHA ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            return SHA(cycles, address);

        case 0x94: // STY zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return STY(cycles, address);

        case 0x95: // STA zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return STA(cycles, address);

        case 0x96: // STX zpg,Y
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_y);
            cycles += 1;
            return STX(cycles, address);

        // Unofficial
        case 0x97: // SAX zpg,Y
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_y);
            cycles += 1;
            return SAX(cycles, address);

        case 0x98: // TYA impl
            return TYA();

        case 0x99: // STA abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 2;
            return STA(cycles, address);

        case 0x9A: // TXS impl
            return TXS();

        // Unofficial
        case 0x9B: // SHS abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            return SHS(cycles, address);

        // Unofficial
        case 0x9C: // SHY abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 2;
            return SHY(cycles, address);

        case 0x9D: // STA abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 2;
            return STA(cycles, address);

        // Unofficial
        case 0x9E: // SHX abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 2;
            return SHX(cycles, address);

        // Unofficial
        case 0x9F: // SHA abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 2;
            return SHA(cycles, address);

        case 0xA0: // LDY #
            std::tie(val, cycles) = Mode::immediate();
            return LDY(cycles, val);

        case 0xA1: // LDA X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 1;
            val = cpu.read(address);
            return LDA(cycles, val);

        case 0xA2: // LDX #
            std::tie(val, cycles) = Mode::immediate();
            return LDX(cycles, val);

        // Unofficial
        case 0xA3: // LAX X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, true);
            cycles += 1;
            val = cpu.read(address);
            return LAX(cycles, val);

        case 0xA4: // LDY zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return LDY(cycles, val);

        case 0xA5: // LDA zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return LDA(cycles, val);

        case 0xA6: // LDX zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return LDX(cycles, val);

        // Unofficial
        case 0xA7: // LAX zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return LAX(cycles, val);

        case 0xA8: // TAY impl
            return TAY();

        case 0xA9: // LDA #
            std::tie(val, cycles) = Mode::immediate();
            return LDA(cycles, val);

        case 0xAA: // TAX impl
            return TAX();

        // Unofficial
        case 0xAB: // LXA #
            std::tie(val, cycles) = Mode::immediate();
            return LXA(cycles, val);

        case 0xAC: // LDY abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return LDY(cycles, val);

        case 0xAD: // LDA abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return LDA(cycles, val);

        case 0xAE: // LDX abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return LDX(cycles, val);

        // Unofficial
        case 0xAF: // LAX abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return LAX(cycles, val);

        case 0xB0: // BCS rel
            std::tie(address, cycles) = Mode::relative();
            return BCS(cycles, address);

        case 0xB1: // LDA ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return LDA(cycles, val);

        // Unofficial
        case 0xB3: // LAX ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return LAX(cycles, val);

        case 0xB4: // LDY zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            val = cpu.read(address);
            return LDY(cycles, val);

        case 0xB5: // LDA zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            val = cpu.read(address);
            return LDA(cycles, val);

        case 0xB6: // LDX zpg,Y
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_y);
            cycles += 1;
            val = cpu.read(address);
            return LDX(cycles, val);

        // Unofficial
        case 0xB7: // LAX zpg,Y
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_y);
            cycles += 1;
            val = cpu.read(address);
            return LAX(cycles, val);

        case 0xB8: // CLV impl
            return CLV();

        case 0xB9: // LDA abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return LDA(cycles, val);

        case 0xBA: // TSX impl
            return TSX();

        case 0xBC: // LDY abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return LDY(cycles, val);

        case 0xBD: // LDA abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return LDA(cycles, val);

        case 0xBE: // LDX abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return LDX(cycles, val);

        // Unofficial
        case 0xBF: // LAX abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return LAX(cycles, val);

        case 0xC0: // CPY #
            std::tie(val, cycles) = Mode::immediate();
            return CPY(cycles, val);

        case 0xC1: // CMP X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, true);
            cycles += 1;
            val = cpu.read(address);
            return CMP(cycles, val);

        // Unofficial
        case 0xC2: // NOP #
            std::tie(address, cycles) = Mode::immediate();
            return cycles;

        // Unofficial
        case 0xC3: // DCP X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 3;
            return DCP(cycles, address);

        case 0xC4: // CPY zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return CPY(cycles, val);

        case 0xC5: // CMP zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return CMP(cycles, val);

        // Unofficial
        case 0xC7: // DCP zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return DCP(cycles, address);

        case 0xC6: // DEC zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return DEC(cycles, address);

        case 0xC8: // INY impl
            return INY();

        case 0xC9: // CMP #
            std::tie(val, cycles) = Mode::immediate();
            return CMP(cycles, val);

        case 0xCA: // DEX impl
            return DEX();

        // Unofficial
        case 0xCB: // AXS #
            std::tie(val, cycles) = Mode::immediate();
            return AXS(cycles, val);

        case 0xCC: // CPY abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return CPY(cycles, val);

        case 0xCD: // CMP abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return CMP(cycles, val);

        case 0xCE: // DEC abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return DEC(cycles, address);

        // Unofficial
        case 0xCF: // DCP abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return DCP(cycles, address);

        case 0xD0: // BNE rel
            std::tie(address, cycles) = Mode::relative();
            return BNE(cycles, address);

        case 0xD1: // CMP ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return CMP(cycles, val);

        // Unofficial
        case 0xD3: // DCP ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            cycles += 3;
            return DCP(cycles, address);

        // Unofficial
        case 0xD4: // NOP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return cycles;

        case 0xD5: // CMP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            val = cpu.read(address);
            return CMP(cycles, val);

        case 0xD6: // DEC zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            return DEC(cycles, address);

        // Unofficial
        case 0xD7: // DCP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            return DCP(cycles, address);

        case 0xD8: // CLD impl
            return CLD();

        case 0xD9: // CMP abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return CMP(cycles, val);

        // Unofficial
        case 0xDA: // NOP impl
            return 2;

        // Unofficial
        case 0xDB: // DCP abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 4;
            return DCP(cycles, address);

        // Unofficial
        case 0xDC: // NOP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            return cycles;

        case 0xDD: // CMP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return CMP(cycles, val);

        case 0xDE: // DEC abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            return DEC(cycles, address);

        // Unofficial
        case 0xDF: // DCP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            return DCP(cycles, address);

        case 0xE0: // CPX #
            std::tie(val, cycles) = Mode::immediate();
            return CPX(cycles, val);

        case 0xE1: // SBC X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, true);
            cycles += 1;
            val = cpu.read(address);
            return SBC(cycles, val);

        // Unofficial
        case 0xE2: // NOP #
            std::tie(address, cycles) = Mode::immediate();
            return cycles;

        // Unofficial
        case 0xE3: // ISC X,ind
            std::tie(address, cycles) = Mode::indirect(cpu.reg_x, 1, false);
            cycles += 3;
            return ISC(cycles, address);

        case 0xE4: // CPX zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return CPX(cycles, val);

        case 0xE5: // SBC zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            val = cpu.read(address);
            return SBC(cycles, val);

        case 0xE6: // INC zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return INC(cycles, address);

        // Unofficial
        case 0xE7: // ISC zpg
            std::tie(address, cycles) = Mode::zeroPage(0);
            cycles += 2;
            return ISC(cycles, address);

        case 0xE8: // INX impl
            return INX();

        case 0xE9: // SBC #
            std::tie(val, cycles) = Mode::immediate();
            return SBC(cycles, val);

        case 0xEA: // NOP impl
            return 2;

        // Unofficial
        case 0xEB: // SBC #
            std::tie(val, cycles) = Mode::immediate();
            return SBC(cycles, val);

        case 0xEC: // CPX abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return CPX(cycles, val);

        case 0xED: // SBC abs
            std::tie(address, cycles) = Mode::absolute(0, true);
            cycles += 1;
            val = cpu.read(address);
            return SBC(cycles, val);

        case 0xEE: // INC abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return INC(cycles, address);

        // Unofficial
        case 0xEF: // ISC abs
            std::tie(address, cycles) = Mode::absolute(0, false);
            cycles += 3;
            return ISC(cycles, address);

        case 0xF0: // BEQ rel
            std::tie(address, cycles) = Mode::relative();
            return BEQ(cycles, address);

        case 0xF1: // SBC ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, true);
            val = cpu.read(address);
            return SBC(cycles, val);

        // Unofficial
        case 0xF3: // ISC ind,Y
            std::tie(address, cycles) = Mode::indirect(cpu.reg_y, 2, false);
            cycles += 3;
            return ISC(cycles, address);

        // Unofficial
        case 0xF4: // NOP zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return cycles;

        case 0xF5: // SBC zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            val = cpu.read(address);
            return SBC(cycles, val);

        case 0xF6: // INC zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 1;
            return INC(cycles, address);

        // Unofficial
        case 0xF7: // ISC zpg,X
            std::tie(address, cycles) = Mode::zeroPage(cpu.reg_x);
            cycles += 3;
            return ISC(cycles, address);

        case 0xF8: // SED impl
            return SED();

        case 0xF9: // SBC abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, true);
            cycles += 1;
            val = cpu.read(address);
            return SBC(cycles, val);

        // Unofficial
        case 0xFA: // NOP impl
            return 2;

        // Unofficial
        case 0xFB: // ISC abs,Y
            std::tie(address, cycles) = Mode::absolute(cpu.reg_y, false);
            cycles += 4;
            return ISC(cycles, address);

        // Unofficial
        case 0xFC: // NOP abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            return cycles;

        case 0xFD: // SBC abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            val = cpu.read(address);
            return SBC(cycles, val);

        case 0xFE: // INC abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, true);
            cycles += 1;
            return INC(cycles, address);

        // Unofficial
        case 0xFF: // ISC abs,X
            std::tie(address, cycles) = Mode::absolute(cpu.reg_x, false);
            cycles += 4;
            return ISC(cycles, address);

        default:
            std::cerr << "Error: invalid opcode " << hex(instr) << std::endl;
            throw std::exception();
            break;
    }
    /* This won't ever execute, 
    * but I get an annoying warning from the compiler
    * if I don't have it
    */
    return 0;
}

int ISA::ADC(int cycles, byte val)
{
    byte a = cpu.reg_a;
    ubyte c = static_cast<ubyte>(cpu.reg_sr.c);
    byte sum = a + val + c;

    // 16-bit addition result (used to determine if carry flag is set)
    uword big_sum = static_cast<uword>(static_cast<ubyte>(a)) 
        + static_cast<uword>(static_cast<ubyte>(val)) + static_cast<uword>(c);

    // Set accumulator
    cpu.reg_a = sum;

    // Set C flag if unsigned overflow to the next byte
    cpu.reg_sr.c = static_cast<bool>(big_sum >> 8); // TODO simplify

    // Set V flag if a and val are signed the same, but signed different from sum
    cpu.reg_sr.v = static_cast<bool>((a ^ sum) & (val ^ sum) & 0x80);

    cpu.reg_sr.z = (sum == 0);
    cpu.reg_sr.n = (sum < 0);

    return cycles;
}

int ISA::AND(int cycles, byte val)
{
    val &= cpu.reg_a;
    cpu.reg_sr.z = (val == 0);
    cpu.reg_sr.n = (val < 0);
    cpu.reg_a = val;
    return cycles;
}

int ISA::ASL(int cycles, uword address, byte val, bool flag_accumulator)
{
    // Set carry flag to true if left-most bit (bit 7) is 1, false if not
    cpu.reg_sr.c = (val < 0);

    // Shift left 1 bit
    val = val << 1;

    // Set zero flag to true if val is zero, false if not
    cpu.reg_sr.z = (val == 0);

    // Set negative flag to true if val is negative, false if not
    cpu.reg_sr.n = (val < 0);

    // Store val to accumulator or cpu
    if (flag_accumulator)
    {
        cpu.reg_a = val;
    }
    else
    {
        cpu.write(address, val);
    }
    return cycles;
}

int ISA::BCC(int cycles, uword address)
{
    if (!static_cast<bool>(cpu.reg_sr.c))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO verify
        cycles = 3;
    }

    return cycles;
}

int ISA::BCS(int cycles, uword address)
{    
    if (static_cast<bool>(cpu.reg_sr.c))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO verify
        cycles = 3;
    }

    return cycles;
}

int ISA::BEQ(int cycles, uword address)
{    
    if (static_cast<bool>(cpu.reg_sr.z))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO verify
        cycles = 3;
    }

    return cycles;
}

int ISA::BIT(int cycles, byte val)
{
    // Value that results from ANDing the cpu value and the accumulator value
    byte result = cpu.reg_a & val;

    // Z flag => true if result is 0, false if not
    cpu.reg_sr.z = (result == 0);

    // N flag => bit 7 of result
    cpu.reg_sr.n = (val < 0);

    // V flag => bit 6 of result
    val <<= 1;
    cpu.reg_sr.v = val < 0;

    return cycles;
}

int ISA::BMI(int cycles, uword address)
{    
    if (static_cast<bool>(cpu.reg_sr.n))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO verify
        cycles = 3;
    }

    return cycles;
}

int ISA::BNE(int cycles, uword address)
{
    if (!static_cast<bool>(cpu.reg_sr.z))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO verify
        cycles = 3;
    }

    return cycles;
}

int ISA::BPL(int cycles, uword address)
{
    if (!static_cast<bool>(cpu.reg_sr.n))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO figure out if this is 3 or 2
        cycles = 3;
    }

    return cycles;
}

int ISA::BRK()
{
    // read and throw away the next byte
    cpu.nextByte();

    // TODO handle break flag shenanigans here
    // Force NMI
    cpu.addInterrupt(NMI);

    // Set flags
    cpu.reg_sr.b = 1;

    return 7;
}

int ISA::BVC(int cycles, uword address)
{   
    if (!static_cast<bool>(cpu.reg_sr.v))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO double check this val
        cycles = 3;
    }

    return cycles;
}

int ISA::BVS(int cycles, uword address)
{
    if (static_cast<bool>(cpu.reg_sr.v))
    {
        cpu.reg_pc = address;
        cycles += 1;
    }
    else
    {
        // TODO verify
        cycles = 3;
    }

    return cycles;
}

int ISA::CLC()
{
    cpu.reg_sr.c = 0;
    return 2;
}

// Unsupported by 2A03
int ISA::CLD()
{
    cpu.reg_sr.d = 0;
    return 2;
}

int ISA::CLI()
{    
    cpu.reg_sr.i = (0);
    return 2;
}

int ISA::CLV()
{
    cpu.reg_sr.v = 0;
    return 2;
}

int ISA::CMP(int cycles, byte val)
{
    byte a = cpu.reg_a;
    byte result = a - val;
    cpu.reg_sr.c = static_cast<ubyte>(a) >= static_cast<ubyte>(val);
    cpu.reg_sr.z = (result == 0);
    cpu.reg_sr.n = (result < 0);

    return cycles;
}

int ISA::CPX(int cycles, byte val)
{
    byte x = cpu.reg_x;
    byte result = x - val;

    // C flag => true if X >= M
    cpu.reg_sr.c = static_cast<ubyte>(x) >= static_cast<ubyte>(val);

    // Z flag => true if X == M
    cpu.reg_sr.z = x == val;

    // N flag => true if result is negative
    cpu.reg_sr.n = (result < 0);

    return cycles;
}

int ISA::CPY(int cycles, byte val)
{
    byte y = cpu.reg_y;
    byte result = y - val;

    // C flag => true if Y >= M
    cpu.reg_sr.c = static_cast<ubyte>(y) >= static_cast<ubyte>(val);

    // Z flag => true if Y == M
    cpu.reg_sr.z = y == val;

    // N flag => true if result is negative
    cpu.reg_sr.n = (result < 0);

    return cycles;
}

int ISA::DEC(int cycles, uword address)
{
    // Decrement value in cpu
    byte val = cpu.read(address);
    val -= 1;
    cpu.write(address, val);

    // Z flag => true if the value is 0, false if not
    cpu.reg_sr.z = val == 0;

    // N flag => true if the value is negative, false if not
    cpu.reg_sr.n = (val < 0);

    return cycles;
}

int ISA::DEX()
{
    // Decrement X value
    byte val = cpu.reg_x;
    val -= 1;
    cpu.reg_x = val;

    // Z flag => true if the value is 0, false if not
    cpu.reg_sr.z = val == 0;

    // N flag => true if the value is negative, false if not
    cpu.reg_sr.n = (val < 0);

    return 2;
}

int ISA::DEY()
{
    // Decrement Y value
    byte val = cpu.reg_y;
    val -= 1;
    cpu.reg_y = val;

    // Z flag => true if the value is 0, false if not
    cpu.reg_sr.z = val == 0;

    // N flag => true if the value is negative, false if not
    cpu.reg_sr.n = (val < 0);

    return 2;
}

int ISA::EOR(int cycles, byte val)
{
    // A EOR M
    byte result = cpu.reg_a ^ val;

    // Z flag => true if the result is 0, false if not
    cpu.reg_sr.z = result == 0;

    // N flag => true if the result is negative, false if not
    cpu.reg_sr.n = (result < 0);

    // Store result in accumulator
    cpu.reg_a = result;

    return cycles;
}

int ISA::INC(int cycles, uword address)
{
    // Increment value in cpu
    byte val = cpu.read(address);
    val += 1;
    cpu.write(address, val);

    // Z flag => true if the value is 0
    cpu.reg_sr.z = val == 0;

    // N flag => true if the value is negative
    cpu.reg_sr.n = (val < 0);

    return cycles;
}

int ISA::INX()
{
    // Decrement X value
    byte val = cpu.reg_x;
    val += 1;
    cpu.reg_x = val;

    // Z flag => true if the value is 0
    cpu.reg_sr.z = (val == 0);

    // N flag => true if the value is negative
    cpu.reg_sr.n = (val < 0);

    return 2;
}

int ISA::INY()
{
    // Increment Y value
    byte val = cpu.reg_y;
    val += 1;
    cpu.reg_y = val;

    // Z flag => true if the value is 0
    cpu.reg_sr.z = (val == 0);

    // N flag => true if the value is negative
    cpu.reg_sr.n = (val < 0);

    return 2;
}

int ISA::JMP(int cycles, uword address)
{
    cpu.reg_pc = address;
    return cycles;
}

int ISA::JSR(uword address)
{
    // Push PC's value between operand 1 and 2
    uword pc = cpu.reg_pc - 1;
    ubyte pcl = static_cast<ubyte>(pc & 0x00FF);
    ubyte pch = static_cast<ubyte>((pc & 0xFF00) >> 8);
    cpu.push(pch);
    cpu.push(pcl);

    cpu.reg_pc = address;

    return 6;
}

int ISA::LDA(int cycles, byte val)
{
    // Load Accumulator with value
    cpu.reg_a = val;

    // Z flag => true if the value is 0, false if not
    cpu.reg_sr.z = (val == 0);

    // N flag => true if the value is negative, false if not
    cpu.reg_sr.n = (val < 0);

    return cycles;
}

int ISA::LDX(int cycles, byte val)
{
    // Load index X with value
    cpu.reg_x = val;

    // Z flag => true if the value is 0, false if not
    cpu.reg_sr.z = (val == 0);

    // N flag => true if the value is negative, false if not
    cpu.reg_sr.n = (val < 0);

    return cycles;
}

int ISA::LDY(int cycles, byte val)
{
    // Load index Y with value
    cpu.reg_y = val;

    // Z flag => true if the value is 0, false if not
    cpu.reg_sr.z = (val == 0);

    // N flag => true if the value is negative, false if not
    cpu.reg_sr.n = (val < 0);

    return cycles;
}

int ISA::LSR(int cycles, uword address, byte val, bool flag_accumulator)
{
    // Set carry flag if right-most bit (bit 0) is 1
    cpu.reg_sr.c = static_cast<bool>(val & 1);

    // Arithmetic shift right one bit
    val >>= 1;

    // Convert to logical shift right (set bit 7 to 0)
    val &= 0x7F;

    cpu.reg_sr.z = (val == 0);

    // N flag = false
    cpu.reg_sr.n = false;

    // Store value in cpu or accumulator
    if (flag_accumulator) cpu.reg_a = val;
    else cpu.write(address, val);

    return cycles;
}

int ISA::ORA(int cycles, byte val)
{
    // A OR M
    val = cpu.reg_a | val;

    // Z flag = true if val is 0, false otherwise
    cpu.reg_sr.z = ((val == 0));

    // N flag = true if val is negative, false otherwise
    cpu.reg_sr.n = (val < 0);

    // Store value to accumulator
    cpu.reg_a = val;

    return cycles;
}

int ISA::PHA()
{
    // Push accumulator to stack
    cpu.push(cpu.reg_a);

    return 3;
}

int ISA::PHP()
{
    // Push processor status to stack with bits 4 and 5 set
    cpu.push(cpu.reg_sr.reg | 0x30);
    return 3;
}

int ISA::PLA()
{
    // Pull accumulator from stack
    byte a = cpu.pop();
    cpu.reg_a = a;

    // Z flag => true if accumulator is zero
    cpu.reg_sr.z = (a == 0);

    // N flag => true if accumulator is negative
    cpu.reg_sr.n = (a < 0);


    return 4;
}

int ISA::PLP()
{
    // Pull processor status from stack, set bits 5 and 4 to 1 and 0
    cpu.reg_sr.reg = cpu.pop();
    cpu.reg_sr.u = true;
    cpu.reg_sr.b = false;

    return 4;
}

int ISA::ROL(int cycles, uword address, byte val, bool flag_accumulator)
{
    // Rotate one bit left

    // Step 1/3: set new carry flag to bit 7
    bool carry_flag = 0;
    if (val < 0) carry_flag = 1;

    // Step 2/3: shift left 1 bit
    val <<= 1;

    // Step 3/3: set bit 0 to old carry flag
    val += static_cast<byte>(cpu.reg_sr.c);

    // C flag
    cpu.reg_sr.c = carry_flag;

    // Z flag => true if val is zero
    cpu.reg_sr.z = (val == 0);

    // N flag = true if val is negative
    cpu.reg_sr.n = (val < 0);

    // Store to cpu/accumulator
    if (flag_accumulator) cpu.reg_a = val;
    else cpu.write(address, val);

    return cycles;
}

int ISA::ROR(int cycles, uword address, byte val, bool flag_accumulator)
{
    // Rotate one bit right

    // Step 1/3: set new carry flag to bit 0
    bool carry_flag = static_cast<bool>(val & 1);

    // Step 2/3: shift right 1 bit
    val = val >> 1;
    val &= 0x7F;

    // Step 3/3: set bit 7 to old carry flag
    val += static_cast<byte>(cpu.reg_sr.c) << 7;

    // C flag
    cpu.reg_sr.c = carry_flag;

    // Z flag => true if the value is zero
    cpu.reg_sr.z = (val == 0);

    // N flag => true if the value is negative
    cpu.reg_sr.n = (val < 0);

    // Store to cpu/accumulator
    if (flag_accumulator) cpu.reg_a = val;
    else cpu.write(address, val);

    return cycles;
}

int ISA::RTI()
{
    // Read next byte and throw away
    cpu.reg_pc++;
    
    // Pull SR, set bits 5 and 4 to 1 and 0
    cpu.reg_sr.reg = cpu.pop();
    cpu.reg_sr.u = true;
    cpu.reg_sr.b = false;

    // Pull PC
    ubyte pcl = static_cast<ubyte>(cpu.pop());
    ubyte pch = static_cast<ubyte>(cpu.pop());
    cpu.reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl);

    return 6;
}

int ISA::RTS()
{
    // Pull PC and increment
    ubyte pcl = static_cast<ubyte>(cpu.pop());
    ubyte pch = static_cast<ubyte>(cpu.pop());
    cpu.reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl) + 1;

    return 6;
}

int ISA::SBC(int cycles, byte val)
{
    return ADC(cycles, ~val);
}

int ISA::SEC()
{
    cpu.reg_sr.c = 1;
    return 2;
}

// Not supported on 2A03
int ISA::SED()
{
    cpu.reg_sr.d = 1;
    return 2;
}

int ISA::SEI()
{
    cpu.reg_sr.i = 1;
    return 2;
}

int ISA::STA(int cycles, uword address)
{
    cpu.write(address, cpu.reg_a);
    return cycles;
}

int ISA::STX(int cycles, uword address)
{
    cpu.write(address, cpu.reg_x);
    return cycles;
}

int ISA::STY(int cycles, uword address)
{
    cpu.write(address, cpu.reg_y);
    return cycles;
}

int ISA::TAX()
{
    byte val = cpu.reg_a;
    cpu.reg_sr.z = (val == 0);
    cpu.reg_sr.n = (val < 0);
    cpu.reg_x = val;

    return 2;
}

int ISA::TAY()
{
    byte val = cpu.reg_a;
    cpu.reg_sr.z = (val == 0);
    cpu.reg_sr.n = (val < 0);
    cpu.reg_y = val;

    return 2;
}

int ISA::TSX()
{
    byte val = cpu.reg_sp;
    cpu.reg_sr.z = (val == 0);
    cpu.reg_sr.n = (val < 0);
    cpu.reg_x = val;

    return 2;
}

int ISA::TXA()
{
    byte val = cpu.reg_x;
    cpu.reg_sr.z = (val == 0);
    cpu.reg_sr.n = (val < 0);
    cpu.reg_a = val;

    return 2;
}

int ISA::TXS()
{
    byte val = cpu.reg_x;
    cpu.reg_sp = val;

    return 2;
}

int ISA::TYA()
{
    byte val = cpu.reg_y;
    cpu.reg_sr.z = (val == 0);
    cpu.reg_sr.n = (val < 0);
    cpu.reg_a = val;

    return 2;
}

// Unofficial instructions

int ISA::ALR(int cycles, byte val)
{
    AND(0, val);
    LSR(0, 0, cpu.reg_a, true);
    return cycles;
}

int ISA::ANC(int cycles, byte val)
{
    AND(0, val);
    byte a = cpu.reg_a;
    cpu.reg_sr.c = (a < 0);
    return cycles;
}

int ISA::ARR(int cycles, byte val)
{
    AND(0, val);
    ROR(0, 0, cpu.reg_a, true);
    byte a = cpu.reg_a;
    bool bit6 = static_cast<bool>(a & 0x40);
    bool bit5 = static_cast<bool>(a & 0x20);
    cpu.reg_sr.c = bit6;
    cpu.reg_sr.v = bit6 ^ bit5;
    return cycles;
}

int ISA::AXS(int cycles, byte val)
{
    byte result = cpu.reg_a & cpu.reg_x;
    cpu.reg_sr.c = (static_cast<ubyte>(result) >= static_cast<ubyte>(val));
    result -= val;
    cpu.reg_sr.z = (result == 0);
    cpu.reg_sr.n = (result < 0);
    cpu.reg_x = result;
    return cycles;
}

int ISA::DCP(int cycles, uword address)
{
    DEC(0, address);
    byte val = cpu.read(address);
    CMP(0, val);
    return cycles;
}

int ISA::ISC(int cycles, uword address)
{
    INC(0, address);
    byte val = cpu.read(address);
    SBC(0, val);
    return cycles;
}

int ISA::LAX(int cycles, byte val)
{
    LDA(0, val);
    TAX();
    return cycles;
}

int ISA::LXA(int cycles, byte val)
{
    cpu.reg_a = val;
    cpu.reg_x = val;
    cpu.reg_sr.z = (val == 0);
    cpu.reg_sr.n = (val < 0);
    return cycles;
}

int ISA::RLA(int cycles, uword address)
{
    byte val = cpu.read(address);
    ROL(0, address, val, false);
    val = cpu.read(address);
    AND(0, val);
    return cycles;
}

int ISA::RRA(int cycles, uword address)
{
    byte val = cpu.read(address);
    ROR(0, address, val, false);
    val = cpu.read(address);
    ADC(0, val);
    return cycles;
}

int ISA::SAX(int cycles, uword address)
{
    byte result = cpu.reg_a & cpu.reg_x;
    cpu.write(address, result);
    return cycles;
}

//TODO flags?
int ISA::SHA(int cycles, uword address)
{
    byte a = cpu.reg_a;
    byte x = cpu.reg_x;
    byte result = a & x & ((address >> 8) + 1);
    cpu.write(address, result);
    return cycles;
}

//TODO flags?
int ISA::SHS(int cycles, uword address)
{
    address++; // To prevent the compiler from giving me an unused param warning
    cpu.reg_sp = cpu.reg_a & cpu.reg_x;
    return cycles;
}

int ISA::SHX(int cycles, uword address)
{
    // Detect page cross
    if (static_cast<ubyte>(cpu.reg_y) > static_cast<ubyte>(address & 0x00FF))
    {
        address &= ((static_cast<uword>(static_cast<ubyte>(cpu.reg_x)) << 8) + 0xFF);
    }
    cpu.write(address, cpu.reg_x & (static_cast<ubyte>(address >> 8) + 1));
    return cycles;
}

int ISA::SHY(int cycles, uword address)
{
    // Detect page cross
    if (static_cast<ubyte>(cpu.reg_x) > static_cast<ubyte>(address & 0x00FF))
    {
        address &= ((static_cast<uword>(static_cast<ubyte>(cpu.reg_y)) << 8) + 0xFF);
    }
    cpu.write(address, cpu.reg_y & (static_cast<ubyte>(address >> 8) + 1));
    return cycles;
}

int ISA::SLO(int cycles, uword address)
{
    byte val = cpu.read(address);
    ASL(0, address, val, false);
    val = cpu.read(address);
    ORA(0, val);
    return cycles;
}

int ISA::SRE(int cycles, uword address)
{
    byte val = cpu.read(address);
    LSR(0, address, val, false);
    val = cpu.read(address);
    EOR(0, val);
    return cycles;
}