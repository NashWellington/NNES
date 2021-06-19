#include "isa-ca.h"

// TODO delete this once I copy all the instructions to instructions
const std::array<std::function<void(byte)>,256> instructions = 
{
//      X0         X1         X2         X3         X4         X5         X6         X7         X8         X9         XA         XB         XC         XD         XE         XF
    &ISA::BRK, &ISA::ORA, &ISA::STP, &ISA::SLO, &ISA::NOP, &ISA::ORA, &ISA::ASL, &ISA::SLO, &ISA::PHP, &ISA::ORA, &ISA::ASL, &ISA::ANC, &ISA::NOP, &ISA::ORA, &ISA::ASL, &ISA::SLO, // 0X
    &ISA::BPL, &ISA::ORA, &ISA::STP, &ISA::SLO, &ISA::NOP, &ISA::ORA, &ISA::ASL, &ISA::SLO, &ISA::CLC, &ISA::ORA, &ISA::NOP, &ISA::SLO, &ISA::NOP, &ISA::ORA, &ISA::ASL, &ISA::SLO, // 1X
    &ISA::JSR, &ISA::AND, &ISA::STP, &ISA::RLA, &ISA::BIT, &ISA::AND, &ISA::ROL, &ISA::RLA, &ISA::PLP, &ISA::AND, &ISA::ROL, &ISA::ANC, &ISA::BIT, &ISA::AND, &ISA::ROL, &ISA::RLA, // 2X
    &ISA::BMI, &ISA::AND, &ISA::STP, &ISA::RLA, &ISA::NOP, &ISA::AND, &ISA::ROL, &ISA::RLA, &ISA::SEC, &ISA::AND, &ISA::NOP, &ISA::RLA, &ISA::NOP, &ISA::AND, &ISA::ROL, &ISA::RLA, // 3X
    &ISA::RTI, &ISA::EOR, &ISA::STP, &ISA::SRE, &ISA::NOP, &ISA::EOR, &ISA::LSR, &ISA::SRE, &ISA::PHA, &ISA::EOR, &ISA::LSR, &ISA::ALR, &ISA::JMP, &ISA::EOR, &ISA::LSR, &ISA::SRE, // 4X
    &ISA::BVC, &ISA::EOR, &ISA::STP, &ISA::SRE, &ISA::NOP, &ISA::EOR, &ISA::LSR, &ISA::SRE, &ISA::CLI, &ISA::EOR, &ISA::NOP, &ISA::SRE, &ISA::NOP, &ISA::EOR, &ISA::LSR, &ISA::SRE, // 5X
    &ISA::RTS, &ISA::ADC, &ISA::STP, &ISA::RRA, &ISA::NOP, &ISA::ADC, &ISA::ROR, &ISA::RRA, &ISA::PLA, &ISA::ADC, &ISA::ROR, &ISA::ARR, &ISA::JMP, &ISA::ADC, &ISA::ROR, &ISA::RRA, // 6X
    &ISA::BVS, &ISA::ADC, &ISA::STP, &ISA::RRA, &ISA::NOP, &ISA::ADC, &ISA::ROR, &ISA::RRA, &ISA::SEI, &ISA::ADC, &ISA::NOP, &ISA::RRA, &ISA::NOP, &ISA::ADC, &ISA::ROR, &ISA::RRA, // 7X
    &ISA::NOP, &ISA::STA, &ISA::NOP, &ISA::SAX, &ISA::STY, &ISA::STA, &ISA::STX, &ISA::SAX, &ISA::DEY, &ISA::NOP, &ISA::TXA, &ISA::XAA, &ISA::STY, &ISA::STA, &ISA::STX, &ISA::SAX, // 8X
    &ISA::BCC, &ISA::STA, &ISA::STP, &ISA::SHA, &ISA::STY, &ISA::STA, &ISA::STX, &ISA::SAX, &ISA::TYA, &ISA::STA, &ISA::TXS, &ISA::SHS, &ISA::SHY, &ISA::STA, &ISA::SHX, &ISA::SHA, // 9X
    &ISA::LDY, &ISA::LDA, &ISA::LDX, &ISA::LAX, &ISA::LDY, &ISA::LDA, &ISA::LDX, &ISA::LAX, &ISA::TAY, &ISA::LDA, &ISA::TAX, &ISA::LXA, &ISA::LDY, &ISA::LDA, &ISA::LDX, &ISA::LAX, // AX
    &ISA::BCS, &ISA::LDA, &ISA::STP, &ISA::LAX, &ISA::LDY, &ISA::LDA, &ISA::LDX, &ISA::LAX, &ISA::CLV, &ISA::LDA, &ISA::TSX, &ISA::LAS, &ISA::LDY, &ISA::LDA, &ISA::LDX, &ISA::LAX, // BX
    &ISA::CPY, &ISA::CMP, &ISA::NOP, &ISA::DCP, &ISA::CPY, &ISA::CMP, &ISA::DEC, &ISA::DCP, &ISA::INY, &ISA::CMP, &ISA::DEX, &ISA::AXS, &ISA::CPY, &ISA::CMP, &ISA::DEC, &ISA::DCP, // CX
    &ISA::BNE, &ISA::CMP, &ISA::STP, &ISA::DCP, &ISA::NOP, &ISA::CMP, &ISA::DEC, &ISA::DCP, &ISA::CLD, &ISA::CMP, &ISA::NOP, &ISA::DCP, &ISA::NOP, &ISA::CMP, &ISA::DEC, &ISA::DCP, // DX
    &ISA::CPX, &ISA::SBC, &ISA::NOP, &ISA::ISC, &ISA::CPX, &ISA::SBC, &ISA::INC, &ISA::ISC, &ISA::INX, &ISA::SBC, &ISA::NOP, &ISA::SBC, &ISA::CPX, &ISA::SBC, &ISA::INC, &ISA::ISC, // EX
    &ISA::BEQ, &ISA::SBC, &ISA::STP, &ISA::ISC, &ISA::NOP, &ISA::SBC, &ISA::INC, &ISA::ISC, &ISA::SED, &ISA::SBC, &ISA::NOP, &ISA::ISC, &ISA::NOP, &ISA::SBC, &ISA::INC, &ISA::ISC  // FX
};

// NOTE: instructions with no addressing mode (e.g. STP) had their modes set to "impl"
const std::array<std::function<void(ISA::InstrType)>,256> modes =
{
//      X0          X1          X2          X3          X4          X5          X6          X7          X8          X9          XA          XB          XC          XD          XE          XF
    &ISA::impl, &ISA::indX, &ISA::impl, &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::acc,  &ISA::imm,  &ISA::abs,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // 0X
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absX, &ISA::absX, // 1X
    &ISA::impl, &ISA::indX, &ISA::impl, &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::acc,  &ISA::imm,  &ISA::abs,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // 2X
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absX, &ISA::absX, // 3X
    &ISA::impl, &ISA::indX, &ISA::impl, &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::acc,  &ISA::imm,  &ISA::abs,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // 4X
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absX, &ISA::absX, // 5X
    &ISA::impl, &ISA::indX, &ISA::impl, &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::acc,  &ISA::imm,  &ISA::ind,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // 6X
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absX, &ISA::absX, // 7X
    &ISA::imm,  &ISA::indX, &ISA::imm,  &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::impl, &ISA::imm,  &ISA::abs,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // 8X
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgY, &ISA::zpgY, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absY, &ISA::absY, // 9X
    &ISA::imm,  &ISA::indX, &ISA::imm,  &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::impl, &ISA::imm,  &ISA::abs,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // AX
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgY, &ISA::zpgY, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absY, &ISA::absY, // BX
    &ISA::imm,  &ISA::indX, &ISA::imm,  &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::impl, &ISA::imm,  &ISA::abs,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // CX
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absX, &ISA::absX, // DX
    &ISA::imm,  &ISA::indX, &ISA::imm,  &ISA::indX, &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::zpg,  &ISA::impl, &ISA::imm,  &ISA::impl, &ISA::imm,  &ISA::abs,  &ISA::abs,  &ISA::abs,  &ISA::abs,  // EX
    &ISA::rel,  &ISA::indY, &ISA::impl, &ISA::indY, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::zpgX, &ISA::impl, &ISA::absY, &ISA::impl, &ISA::absY, &ISA::absX, &ISA::absX, &ISA::absX, &ISA::absX  // FX
};

std::queue<std::function<uint()>> tasks = {}; // TODO deque?

void ISA::nextInstr()
{
    // TODO reset some instr_data flags
    // TODO add interrupt polling when appropriate (in addressing mode methods?)
    byte instr = cpu.nextByte(); // 1
    instructions[instr](instr);
}

void ISA::impl(InstrType type)
{
    assert(type == InstrType::NONE);
    tasks.push(&dummyNextByte); // 2 - Read next instr byte & throw away
}

void ISA::acc(InstrType type)
{
    tasks.push(&dummyNextByte); // 2 - Read next instr byte & throw away, then get accu val
    tasks.push([]()
    {
        instr_data.val = cpu.reg_a;
        instr_data.flag_accu = true;
        return 0;
    });
}

void ISA::imm(InstrType type)
{
    tasks.push(&dummyNextByte); // 2 Read next instr byte & throw away, then incr PC
    tasks.push([]()
    {
        cpu.reg_pc++;
        return 0;
    });
}

void ISA::abs(InstrType type)
{
    // 2 - Fetch low addr byte + incr PC
    tasks.push([]()
    {
        instr_data.address.addr_l = cpu.nextByte();
        return 1;
    });
    switch (type)
    {
        case InstrType::READ:
            // 3 - Fetch high addr byte + incr PC
            tasks.push([]()
            {
                instr_data.address.addr_h = cpu.nextByte();
                return 1;
            });
            // 4 - Read from addr
            tasks.push(&read);
            break;
        case InstrType::RMW:
            // 3 - Fetch high addr byte + incr PC
            tasks.push([]()
            {
                instr_data.address.addr_h = cpu.nextByte();
                return 1;
            });
            // 4 - Read from addr
            tasks.push(&read);
            // 5 - Write val back to addr
            tasks.push(&write);
            break;
        case InstrType::WRITE:
            // 3 - Fetch high addr byte + incr PC
            tasks.push([]()
            {
                instr_data.address.addr_h = cpu.nextByte();
                return 1;
            });
            break;
        case InstrType::NONE:
            // 3 - Fetch high addr byte (no PC incr)
            tasks.push([]()
            {
                instr_data.address.addr_h = cpu.nextByte();
                cpu.reg_pc--;
                return 1;
            });
            break;
    }
}

void ISA::zpg(InstrType type)
{
    assert(type != InstrType::NONE);
    // 2 - Fetch addr, incr PC
    tasks.push([]()
    {
        instr_data.address.addr_h = 0;
        instr_data.address.addr_l = cpu.nextByte();
        return 1;
    });
    switch (type)
    {
        case InstrType::READ:
            // 3 - Read from addr
            tasks.push(&read);
            break;
        case InstrType::RMW:
            // 3 - Read from addr
            tasks.push(&read);
            // 4 - Write val back to addr
            tasks.push(&write);
            break;
        case InstrType::WRITE:
            break;
        default:
            break;
    }
}

void ISA::zpgX(InstrType type)
{
    assert(type != InstrType::NONE);
    // TODO 2 - Fetch addr
    // 3 - Read from addr & index
    tasks.push(&read);
    tasks.push([]()
    {
        instr_data.address.addr_l += cpu.reg_x; // Note: page boundary cross not handled
        return 0;
    });
    switch (type)
    {
        case InstrType::READ:
            // 4 - Read from addr
            tasks.push(&read);
            break;
        case InstrType::RMW:
            // 4 - Read from addr
            tasks.push(&read);
            // 5 - Write val back to addr
            tasks.push(&write);
            break;
        case InstrType::WRITE:
            break;
        default:
            break;
    }
}

void ISA::zpgY(InstrType type)
{
    assert(type != InstrType::NONE);
    // TODO 2 - Fetch addr
    // 3 - Read from addr & index
    tasks.push(&read);
    tasks.push([]()
    {
        instr_data.address.addr_l += cpu.reg_y; // Note: page boundary cross not handled 
        return 0;
    });
    switch (type)
    {
        case InstrType::READ:
            // 4 - Read from addr
            tasks.push(&read);
            break;
        case InstrType::RMW:
            // 4 - Read from addr
            tasks.push(&read);
            // 5 - Write val back to addr
            tasks.push(&write);
            break;
        case InstrType::WRITE:
            break;
        default:
            break;
    }
}

void ISA::absX(InstrType type)
{
    assert(type != InstrType::NONE);
    // TODO 2 - Fetch addr low byte
    // TODO 3 - Fetch addr high byte and index
    // TODO
    tasks.push([]()
    {
        instr_data.fixed_address = instr_data.address.addr;
        instr_data.address.addr_l += cpu.reg_x;
        return 0;
    });
    // 4 - read from addr, fix high byte
    tasks.push(&read);
    tasks.push([]()
    {
        instr_data.fixed_address += static_cast<uword>(static_cast<ubyte>(val));
        return 0;
    });
    switch (type)
    {
        case InstrType::READ:
            // 5+ - If addr is fixed, re-read
            tasks.push([]()
            {
                if (instr_data.fixed_address != instr_data.address.addr)
                {
                    instr_data.address.addr = instr_data.fixed_address;
                    return read();
                }
                return 0;
            });
            break;
        case InstrType::RMW:
            // 5 - Re-read fixed addr
            tasks.push([]()
            {
                instr_data.address.addr = instr_data.fixed_address;
                return read();
            });
            // 6 - Write val back to addr
            tasks.push(&write);
            break;
        case InstrType::WRITE:
            // Fix address
            tasks.push([]()
            {
                instr_data.address.addr = instr_data.fixed_address;
                return 0;
            });
        default:
            break;
    }
}

void ISA::absY(InstrType type)
{
    assert(type != InstrType::NONE);
    // TODO 2 - Fetch addr low byte
    // TODO 3 - Fetch addr high byte and index
    // TODO
    tasks.push([]()
    {
        instr_data.fixed_address = instr_data.address.addr;
        instr_data.address.addr_l += cpu.reg_y;
        return 0;
    });
    // 4 - read from addr, fix high byte
    tasks.push(&read);
    tasks.push([]()
    {
        instr_data.fixed_address += static_cast<uword>(static_cast<ubyte>(val));
        return 0;
    });
    switch (type)
    {
        case InstrType::READ:
            // 5+ - If addr is fixed, re-read
            tasks.push([]()
            {
                if (instr_data.fixed_address != instr_data.address.addr)
                {
                    instr_data.address.addr = instr_data.fixed_address;
                    return read();
                }
                return 0;
            });
            break;
        case InstrType::RMW:
            // 5 - Re-read fixed addr
            tasks.push([]()
            {
                instr_data.address.addr = instr_data.fixed_address;
                return read();
            });
            // 6 - Write val back to addr
            tasks.push(&write);
            break;
        case InstrType::WRITE:
            // Fix address
            tasks.push([]()
            {
                instr_data.address.addr = instr_data.fixed_address;
                return 0;
            });
        default:
            break;
    }
}

void ISA::rel(InstrType type)
{
    assert(type == InstrType::NONE);
    tasks.push(&nextByte);
    // TODO finish this and figure out what should be here and what should be in branch funcs
}

void ISA::indX(InstrType type)
{
    assert(type != InstrType::NONE);
    // TODO fetch ptr addr
    // TODO read from addr, add X to it
    // TODO fetch effective addr low
    // TODO fetch effective addr high
    switch(type)
    {
        case InstrType::READ:
        case InstrType::RMW:
            // TODO read from effective addr
            break;
        case InstrType::WRITE:
            break;
    }
}


void ISA::indY(InstrType type)
{
    assert(type != InstrType::NONE);
    // TODO fetch ptr addr
    // TODO fetch effective addr low
    // TODO fetch effective addr high, add Y to addr low
    // TODO read from effective addr, fix addr high
    switch(type)
    {
        case InstrType::READ:
            tasks.push([]()
            {
                // TODO if addr fixed, re-read
                return 0;
            });
            break;
        case InstrType::RMW:
            // TODO read from effective addr
            break;
        case InstrType::WRITE:
            break;
    }
}

void ISA::ind(InstrType type)
{
    assert(type == InstrType::NONE);
    // TODO fetch ptr addr low
    // TODO fetch ptr addr high
    // TODO fetch low addr to latch
    // TODO fetch PCH, copy latch to PCL
}

void ISA::BRK(byte instr)
{
    modes[instr](InstrType::NONE); // TODO maybe delete?
    tasks.push([]()
    {
        cpu.reg_pc++;
        return 0;
    });
    // tasks.push(&pushPC) 3 & 4
    // --------- fetch interrupt vector addr here
    // tasks.push(&pushSR) 5 // TODO make sure b flag is set
    // tasks.push(&fetchPC) 6 & 7
}

void ISA::IRQ()
{
    // tasks.push(&nextByte); // TODO ignore instr // 1
    // tasks.push(&nextByte); // TODO ignore operand and don't increment PC // 2
    // tasks.push(&pushPC) 3 & 4
    // ---------- fetch interrupt vector
    // tasks.push(&pushSR) 5 // TODO make sure b flag is clear
    // tasks.push(&fetchPC) 6 & 7
}

void ISA::NMI()
{
    // should be almost the same as IRQ
}

void ISA::ADC(byte instr)
{
    modes[instr](InstrType::READ);
    tasks.push([]()
    {
        // TODO ADC code
        return 0;
    });
}

void ISA::JMP(byte instr)
{
    modes[instr](InstrType::NONE);
    tasks.push([]()
    {
        cpu.reg_pc = instr_data.addr;
        return 0;
    });
}

void ISA::ORA(byte instr)
{
    modes[instr](InstrType::READ);
    tasks.push([]
    {
        return 0;
    });
}