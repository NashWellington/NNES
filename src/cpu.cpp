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
    // TODO
    nes.log_buffer.byte_queue.push(instruction);
    return instruction;
}

void CPU::dummyNextByte() { nes.mem->cpuRead(reg_pc); }

void CPU::tick()
{
    assert(instr.cycle > 0);
    if (!nes.ppu->oamWrite(odd_cycle))
    {
        if (instr.cycle == 1)
        {
            clearInterrupt();
            serviceInterrupt();
            instr.address = 0;
            instr.value = 0;
            instr.page_cross = false;
            instr.branch_taken = false;
        }
        else if (instr.cycle == 5 && instr.code == 0)
        {
            // Interrupt hijacking for BRK, IRQ, NMI
            serviceInterrupt();
        }
        if (serviced_interrupt != Interrupt::NONE)
        {
            if (serviced_interrupt == Interrupt::IRQ) IRQ();
            else if (serviced_interrupt == Interrupt::NMI) NMI();
        }
        else
        {
            if (instr.cycle == 1) 
            {
                // TODO
                if (!nes.log_buffer.byte_queue.empty())
                    nes.log();
                nes.log_buffer.pc = reg_pc;
                instr.code = nextByte();
            }
            else executeInstruction();
        }
        instr.cycle++;
    }
    odd_cycle = !odd_cycle;
}

void CPU::start()
{
    ubyte pcl = nes.mem->cpuRead(0xFFFC);
    ubyte pch = nes.mem->cpuRead(0xFFFD);
    reg_pc = (pch << 8) | pcl;
    cycle = 7;
    // TODO
    // reg_pc = 0xC000;
}

void CPU::reset()
{
    ubyte pcl = nes.mem->cpuRead(0xFFFC);
    ubyte pch = nes.mem->cpuRead(0xFFFD);
    reg_pc = (pch << 8) | pcl;
    reg_sp -= 3;
    reg_sr.i = true;
    cycle = 7;
    instr.cycle = 1;
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

void CPU::queueInterrupt(Interrupt interrupt)
{
    if (interrupt > queued_interrupt) queued_interrupt = interrupt;
}

void CPU::serviceInterrupt()
{
    if (queued_interrupt == Interrupt::NONE 
        && (nes.apu->frame_interrupt || nes.apu->dmc.interrupt))
        queueInterrupt(Interrupt::IRQ);
    if (queued_interrupt == Interrupt::IRQ && reg_sr.i)
        queued_interrupt = Interrupt::NONE;
    if (queued_interrupt > serviced_interrupt)
        serviced_interrupt = queued_interrupt;
    queued_interrupt = Interrupt::NONE;
}

void CPU::clearInterrupt()
{
    serviced_interrupt = Interrupt::NONE;
}