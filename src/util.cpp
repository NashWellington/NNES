#include "util.h"

enum Mode
{
    IMPL, REL, ACC, IND, INDX, INDY, IMM, ZPG, ZPGX, ZPGY, ABS, ABSX, ABSY
};

const std::array<std::string_view,256> instructions = 
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

const std::array<Mode,256> modes =
{
//   X0    X1    X2    X3    X4    X5    X6    X7    X8    X9    XA    XB    XC    XD    XE    XF
    IMPL, INDX, IMPL, INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  ACC,  IMM,  ABS,  ABS,  ABS,  ABS,  // 0X
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGX, ZPGX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX, // 1X
    IMPL, INDX, IMPL, INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  ACC,  IMM,  ABS,  ABS,  ABS,  ABS,  // 2X
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGX, ZPGX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX, // 3X
    IMPL, INDX, IMPL, INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  ACC,  IMM,  ABS,  ABS,  ABS,  ABS,  // 4X
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGX, ZPGX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX, // 5X
    IMPL, INDX, IMPL, INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  ACC,  IMM,  IND,  ABS,  ABS,  ABS,  // 6X
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGX, ZPGX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX, // 7X
    IMM,  INDX, IMM,  INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  IMPL, IMM,  ABS,  ABS,  ABS,  ABS,  // 8X
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGY, ZPGY, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSY, ABSY, // 9X
    IMM,  INDX, IMM,  INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  IMPL, IMM,  ABS,  ABS,  ABS,  ABS,  // AX
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGY, ZPGY, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSY, ABSY, // BX
    IMM,  INDX, IMM,  INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  IMPL, IMM,  ABS,  ABS,  ABS,  ABS,  // CX
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGX, ZPGX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX, // DX
    IMM,  INDX, IMM,  INDX, ZPG,  ZPG,  ZPG,  ZPG,  IMPL, IMM,  IMPL, IMM,  ABS,  ABS,  ABS,  ABS,  // EX
    REL,  INDY, IMPL, INDY, ZPGX, ZPGX, ZPGX, ZPGX, IMPL, ABSY, IMPL, ABSY, ABSX, ABSX, ABSX, ABSX  // FX
};

std::vector<std::string_view> disassemble(std::queue<ubyte>& byte_queue) noexcept
{
    std::vector<std::string_view> dis; // Disassembled instruction
    ubyte instr = byte_queue.front();
    dis.push_back(instructions[instr]); byte_queue.pop();
    switch (modes[instr])
    {
        case IMPL:
            break;
        case ACC:
            dis.push_back("A");
            break;
        case IMM:
            if (byte_queue.empty()) return dis;
            else
            {
                dis.push_back("#$" + hex(byte_queue.front()));
                byte_queue.pop();
            }
            break;
        case ABS:
            if (byte_queue.size() < 2)
            {
                for (uint i = 0; i < byte_queue.size(); i++) byte_queue.pop();
                return dis;
            }
            else
            {
                uword low_byte = byte_queue.front(); byte_queue.pop();
                uword high_byte = byte_queue.front(); byte_queue.pop();
                dis.push_back("$" + hex((high_byte << 8) | low_byte));
            }
            break;
        case ABSX:
            if (byte_queue.size() < 2)
            {
                for (uint i = 0; i < byte_queue.size(); i++) byte_queue.pop();
                return dis;
            }
            else
            {
                uword low_byte = byte_queue.front(); byte_queue.pop();
                uword high_byte = byte_queue.front(); byte_queue.pop();
                dis.push_back("$" + hex((high_byte << 8) | low_byte) + ",");
                dis.push_back("X");
            }
            break;
        case ABSY:
            if (byte_queue.size() < 2)
            {
                for (uint i = 0; i < byte_queue.size(); i++) byte_queue.pop();
                return dis;
            }
            else
            {
                uword low_byte = byte_queue.front(); byte_queue.pop();
                uword high_byte = byte_queue.front(); byte_queue.pop();
                dis.push_back("$" + hex((high_byte << 8) | low_byte) + ",");
                dis.push_back("Y");
            }
            break;
        case ZPG:
            if (byte_queue.empty()) return dis;
            else
            {
                dis.push_back("$" + hex(byte_queue.front())); byte_queue.pop();
            }
            break;
        case ZPGX:
            if (byte_queue.empty()) return dis;
            else
            {
                dis.push_back("$" + hex(byte_queue.front()) + ","); byte_queue.pop();
                dis.push_back("X");
            }
            break;
        case ZPGY:
            if (byte_queue.empty()) return dis;
            else
            {
                dis.push_back("$" + hex(byte_queue.front()) + ","); byte_queue.pop();
                dis.push_back("Y");
            }
            break;
        case IND:
            if (byte_queue.size() < 2) 
            {
                for (uint i = 0; i < byte_queue.size(); i++) byte_queue.pop();
                return dis;
            }
            else
            {
                ubyte low_byte = byte_queue.front(); byte_queue.pop();
                ubyte high_byte = byte_queue.front(); byte_queue.pop();
                dis.push_back("($" + hex((high_byte << 8) | low_byte));
                dis.push_back(")");
            }
            break;
        case INDX:
            if (byte_queue.empty()) return dis;
            else
            {
                dis.push_back("($" + hex(byte_queue.front())); byte_queue.pop();
                dis.push_back(",X)");
            }
            break;
        case INDY:
            if (byte_queue.empty()) return dis;
            else
            {
                dis.push_back("($" + hex(byte_queue.front())); byte_queue.pop();
                dis.push_back("),Y");
            }
            break;
        case REL:
            if (byte_queue.empty()) return dis;
            else
            {
                dis.push_back("$" + hex(byte_queue.front())); byte_queue.pop();
            }
            break;
    }
    return dis;
}

ubyte reverseByte(ubyte b) noexcept
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

std::optional<std::string_view> getOpt(std::vector<std::string_view> args, std::string_view option) noexcept
{
    auto iter = std::find(args.begin(), args.end(), option);
    if (iter == args.end()) return {};
    else return *(++iter);
}