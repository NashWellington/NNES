#include "cpu.hpp"

void CPU::setRegion(Region _region)
{
    region = _region;
    switch (region)
    {
        case Region::NTSC:
            name = "Ricoh 2A03";
            time_scale = 12;
            break;
        case Region::PAL:
            name = "Ricoh 2A07";
            time_scale = 16;
            break;
        case Region::Dendy:
            name = "UMC UA6527P";
            time_scale = 15;
            break;
        default:
            std::cerr << "Error: unsupported region" << std::endl;
            throw std::exception();
            break;
    }
}

ubyte CPU::read(uword address)
{
    return nes.mem->cpuRead(address);
}

void CPU::write(uword address, ubyte data)
{
    nes.mem->cpuWrite(address, data);
}

ubyte CPU::pop()
{
    ubyte old_sp = reg_sp;
    reg_sp++;
    uword address = 0x0100 + reg_sp;

    #ifndef NDEBUG
    if (reg_sp < old_sp)
    {
        std::cerr << "Warning: stack underflow" << std::endl;
    }
    #endif

    return nes.mem->cpuRead(address);
}

void CPU::push(ubyte data)
{
    ubyte old_sp = reg_sp;
    uword address = 0x0100 + reg_sp;
    reg_sp--;

    #ifndef NDEBUG
    if (reg_sp > old_sp)
    {
        std::cerr << "Warning: stack overflow" << std::endl;
    }
    #endif

    nes.mem->cpuWrite(address, data);
}

ubyte CPU::nextByte()
{
    // This is a bit of a misnomer because it could also be an operand
    ubyte instruction = nes.mem->cpuRead(reg_pc);
    reg_pc++;
    return instruction;
}

int CPU::executeInstruction()
{
    // Step 1: get next instruction
    ubyte instruction = nextByte();

    // Step 2: process opcode
    return ISA::executeOpcode(*this, instruction);
}

bool CPU::ready()
{
    return cycle == 0;
}

void CPU::tick()
{
    if (cycle == 0)
    {
        if (!nes.mem->oamWrite(odd_cycle)) 
        {
            step();
        }
    }
    if (cycle > 0) cycle--;
    odd_cycle = !odd_cycle;
}

void CPU::step()
{
    // Check for interrupts
    if (InterruptType i = nes.mem->getInterrupt())
    {
        cycle += handleInterrupt(i);
        nes.mem->clearInterrupt();
    }
    
    if (cycle == 0)
        cycle += executeInstruction();
}

void CPU::start()
{
    ubyte pcl = nes.mem->cpuRead(0xFFFC);
    ubyte pch = nes.mem->cpuRead(0xFFFD);
    reg_pc = (pch << 8) | pcl;
    cycle = 7;
}

void CPU::reset()
{
    ubyte pcl = nes.mem->cpuRead(0xFFFC);
    ubyte pch = nes.mem->cpuRead(0xFFFD);
    reg_pc = (pch << 8) | pcl;
    reg_sp -= 3;
    reg_sr.i = true;
    cycle = 7;
}

void CPU::save(Savestate& savestate)
{
    savestate.registers.reg_pc = reg_pc;
    savestate.registers.reg_sr = reg_sr.reg;
    savestate.registers.reg_sp = reg_sp;
    savestate.registers.reg_a = reg_a;
    savestate.registers.reg_x = reg_x;
    savestate.registers.reg_y = reg_y;
    savestate.cpu_cycles_left = cycle; // Probably unnecessary; should be 0 every time this is called
    savestate.cpu_odd_cycle = odd_cycle;
}

void CPU::load(Savestate& savestate)
{
    reg_pc = savestate.registers.reg_pc;
    reg_sr.reg = savestate.registers.reg_sr;
    reg_sp = savestate.registers.reg_sp;
    reg_a = savestate.registers.reg_a;
    reg_x = savestate.registers.reg_x;
    reg_y = savestate.registers.reg_y;
    cycle = savestate.cpu_cycles_left;
    odd_cycle = savestate.cpu_odd_cycle;
}

void CPU::addInterrupt(InterruptType interrupt)
{
    nes.mem->addInterrupt(interrupt);
}

// TODO emulate interrupt hijacking
int CPU::handleInterrupt(InterruptType type)
{
    assert(type != NO_INTERRUPT);

    switch (type)
    {
        case IRQ:
            return ISA::IRQ(*this);
        case NMI:
            return ISA::NMI(*this);
        case RESET:
            reset();
            break;
        default:
            break;
    }
    return 0;
}