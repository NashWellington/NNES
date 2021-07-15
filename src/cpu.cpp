#include "cpu.h"

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
    }
}

ubyte CPU::read(uword address)
{
    return mem->cpuRead(address);
}

void CPU::write(uword address, ubyte data)
{
    mem->cpuWrite(address, data);
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

    return mem->cpuRead(address);
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

    mem->cpuWrite(address, data);
}

ubyte CPU::nextByte()
{
    // This is a bit of a misnomer because it could also be an operand
    ubyte instruction = mem->cpuRead(reg_pc);
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
        if (!mem->oamWrite(odd_cycle)) 
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
    if (InterruptType i = mem->getInterrupt())
    {
        cycle += handleInterrupt(i);
        mem->clearInterrupt();
    }
    
    if (cycle == 0)
        cycle += executeInstruction();
}

void CPU::start()
{
    ubyte pcl = mem->cpuRead(0xFFFC);
    ubyte pch = mem->cpuRead(0xFFFD);
    reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl);
    mem->start();
    cycle = 7;
}

void CPU::reset()
{
    ubyte pcl = mem->cpuRead(0xFFFC);
    ubyte pch = mem->cpuRead(0xFFFD);
    reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl);
    reg_sp += 3;
    reg_sr.i = true;     // I flag
    mem->reset();
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
    mem->addInterrupt(interrupt);
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