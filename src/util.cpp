#include "util.h"

const std::array<std::string,256> instructions = 
{
//    X0     X1     X2     X3     X4     X5     X6     X7     X8     X9     XA     XB     XC     XD     XE     XF
    "BRK", "ORA", "STP", "SLO", "NOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO", // 0X
    "BPL", "ORA", "STP", "SLO", "NOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO", // 1X
    "JSR", "AND", "STP", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA", // 2X
    "BMI", "AND", "STP", "RLA", "NOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA", // 3X
    "RTI", "EOR", "STP", "SRE", "NOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE", // 4X
    "BVC", "EOR", "STP", "SRE", "NOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE", // 5X
    "RTS", "ADC", "STP", "RRA", "NOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA", // 6X
    "BVS", "ADC", "STP", "RRA", "NOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA", // 7X
    "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX", // 8X
    "BCC", "STA", "STP", "SHA", "STY", "STA", "STX", "SAX", "TYA", "STA", "TXS", "SHS", "SHY", "STA", "SHX", "SHA", // 9X
    "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "LXA", "LDY", "LDA", "LDX", "LAX", // AX
    "BCS", "LDA", "STP", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX", // BX
    "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP", // CX
    "BNE", "CMP", "STP", "DCP", "NOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP", // DX
    "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC", // EX
    "BEQ", "SBC", "STP", "ISC", "NOP", "SBC", "INC", "ISC", "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC"  // FX
};

// Disassemble addressing mode
std::string disassembleMode(uword& address, ubyte instr)
{
    std::string mode = " ";
    switch (instr%32)
    {
        case 0x00:
            if (instr == 0x20) // JSR Absolute
            {
                mode += "$";
                mode += hex(bus.cpuRead(address+1));
                mode += hex(bus.cpuRead(address));
                address += 2;
            }
            else if (instr >= 0x80) // Immediate
            {
                mode += "#$";
                mode += hex(bus.cpuRead(address++));
            }
            break;

        case 0x01: // X-indexed, indirect
        case 0x03:
            mode += "(#$";
            mode += hex(bus.cpuRead(address++));
            mode += ",X)";
            break;

        case 0x02: // Immediate
        case 0x09:
        case 0x0B:
            if (instr != 0x02 && instr != 0x22 && instr != 0x42 && instr != 0x62)
            {
                mode += "#$";
                mode += hex(bus.cpuRead(address++));
            }
            break;

        case 0x04: // Zero Page
        case 0x05:
        case 0x06:
        case 0x07:
            mode += "$";
            mode += hex(bus.cpuRead(address++));
            break;

        case 0x0C: // Absolute or indirect
        case 0x0D:
        case 0x0E:
        case 0x0F:
            if (instr == 0x6C) // Un-indexed indirect
            {
                mode += "($";
                mode += hex(bus.cpuRead(address+1));
                mode += hex(bus.cpuRead(address));
                mode += ")";
                address += 2;
            }
            else // Absolute
            {
                mode += "$";
                mode += hex(bus.cpuRead(address+1));
                mode += hex(bus.cpuRead(address));
                address += 2;
            }
            break;

        case 0x10: // Relative
            mode += "$";
            mode += hex(bus.cpuRead(address++));
            break;

        case 0x11: // indirect, Y-indexed
        case 0x13:
            mode += "(#$";
            mode += hex(bus.cpuRead(address++));
            mode += "),Y";
            break;

        case 0x14: // Zero Page, X-indexed=
        case 0x15:
            mode += "$";
            mode += hex(bus.cpuRead(address++));
            mode += ",X";
            break;

        case 0x16: // Zero Page, X & Y -indexed
        case 0x17:
            if (instr >= 0x96) // Y-indexed
            {
                mode += "$";
                mode += hex(bus.cpuRead(address++));
                mode += ",Y";
            }
            else // X-indexed
            {
                mode += "$";
                mode += hex(bus.cpuRead(address++));
                mode += ",X";
            }
            break;

        case 0x19: // Absolute, Y-indexed
        case 0x1B:
            mode += "$";
            mode += hex(bus.cpuRead(address+1));
            mode += hex(bus.cpuRead(address));
            mode += ",Y";
            address += 2;
            break;
        
        case 0x1C: // Absolute, X-indexed
        case 0x1D:
        case 0x1E:
        case 0x1F:
            if (instr == 0x9E || instr == 0xBE || instr == 0x9F || instr == 0xBF) // Y-indexed
            {
                mode += "$";
                mode += hex(bus.cpuRead(address+1));
                mode += hex(bus.cpuRead(address));
                mode += ",Y";
                address += 2;
            }
            else
            {
                mode += "$";
                mode += hex(bus.cpuRead(address+1));
                mode += hex(bus.cpuRead(address));
                mode += ",X";
                address += 2;
            }
            break;

        default: // implied/accumulator mode or unsupported opcode
            break;
    }   
    return mode; 
}

std::optional<std::string> disassemble(uword& address)
{
    uword start_address = address;
    std::string line = hex(address) + ": ";
    ubyte instr = bus.cpuRead(address++);
    line += instructions[instr];
    line += disassembleMode(address, instr);
    if (start_address > (address - 1)) return {};
    for (int i = 0; i < (20 - static_cast<int>(line.size())); i++) line += ' ';
    return line;
}

std::string peekMem(uword address)
{
    std::string line = hex(address) + ":";
    for (int i = 0; i < 16; i++)
    {
        line += ' ';
        line += hex(bus.cpuRead(address++));
    }
    return line;
}

ubyte reverseByte(ubyte b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}