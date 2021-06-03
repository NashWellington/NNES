#include "cpu.h"

// Addresses of interrupt handling routine vectors
const uword IRQ_ADDRESS = 0xFFFE;
const uword NMI_ADDRESS = 0xFFFA;
const uword RESET_ADDRESS = 0xFFFC;

// Address of the stack (used for push and pop)
const uword STACK_START = 0x0100;

CPU::CPU(Bus& bus) : bus(bus)
{
    start();
}

byte CPU::read(uword address)
{
    return bus.cpuRead(address);
}

void CPU::write(uword address, byte data)
{
    bus.cpuWrite(address, data);
}

byte CPU::pop()
{
    ubyte old_sp = reg_sp;
    reg_sp++;
    uword address = STACK_START + reg_sp;

    #ifndef NDEBUG
    if (reg_sp < old_sp)
    {
        std::cerr << "Error: stack underflow" << std::endl;
        throw std::exception();
    }
    #endif

    return bus.cpuRead(address);
}

void CPU::push(byte data)
{
    ubyte old_sp = reg_sp;
    uword address = STACK_START + reg_sp;
    reg_sp--;

    #ifndef NDEBUG
    if (reg_sp > old_sp)
    {
        std::cerr << "Error: stack overflow" << std::endl;
        throw std::exception();
    }
    #endif

    bus.cpuWrite(address, data);
}

byte CPU::nextByte()
{
    // This is a bit of a misnomer because it could also be an operand
    ubyte instruction = bus.cpuRead(reg_pc);
    reg_pc++;

    #ifndef NDEBUG
    Current_State.addOp(instruction);
    #endif

    return instruction;
}

int CPU::executeInstruction()
{
    // Step 1: get next instruction
    byte instruction = nextByte();

    // Step 2: process opcode
    return ISA::executeOpcode(*this, instruction);
}

void CPU::clock()
{
    if (cycle == 0) step();
    else cycle--;
}

void CPU::step()
{
    #ifndef NDEBUG
    Current_State.reg_PC = reg_pc;
    Current_State.reg_SP = reg_sp;
    Current_State.reg_A = reg_a;
    Current_State.reg_X = reg_x;
    Current_State.reg_Y = reg_y;
    Current_State.reg_SR = reg_sr.reg;
    #endif

    // Check for interrupts
    // TODO fix interrupts
    if (bus.getInterrupt())
    {
        cycle += handleInterrupt(bus.getInterrupt());
        bus.clearInterrupt();
    }
    
    if (cycle == 0)
        cycle += executeInstruction();

    // Log the current state
    #ifndef NDEBUG
    #ifdef DISP_INSTR
    std::clog << Current_State;
    #endif
    Current_State.clearState();
    #endif
}

void CPU::start()
{
    ubyte pcl = bus.cpuRead(RESET_ADDRESS);
    ubyte pch = bus.cpuRead(RESET_ADDRESS + 1);
    reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl);
}

void CPU::reset()
{
    reg_sp -= 3;
    reg_sr.i = true;     // I flag
    // TODO APU/IO register reset stuff
}

void CPU::addInterrupt(InterruptType interrupt)
{
    bus.addInterrupt(interrupt);
}

int CPU::handleInterrupt(InterruptType type)
{
    assert(type != NO_INTERRUPT);

    // If IRQ, check for interrupt disable flag
    if ((type == IRQ) && reg_sr.i) return 0;

    // Default behavior of IRQ and NMI
    if (type != RESET)
    {
        // Push PC (high byte), PC (low byte), and SR onto stack
        byte pch = byte((reg_pc & 0xF0) >> 4);
        byte pcl = byte(reg_pc & 0x0F);
        push(pch);
        push(pcl);
        push(reg_sr.reg);
    }
    /* Emulate post-reset memory state according to:
    * http://wiki.nesdev.com/w/index.php/CPU_power_up_state#After_reset
    */
    else
    {
        // Reset registers
        reset();
    }

    // Set interrupt disable flag
    reg_sr.i = true;

    // Load the address of the interrupt handling routine to PC
    ubyte pcl;
    ubyte pch;
    switch (type)
    {
        case IRQ:
            LOG_INSTR("IRQ", 0);
            pcl = static_cast<ubyte>(bus.cpuRead(IRQ_ADDRESS));
            pch = static_cast<ubyte>(bus.cpuRead(IRQ_ADDRESS + 1));
            reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl);
            break;
        
        case NMI:
            LOG_INSTR("NMI", 0);
            pcl = static_cast<ubyte>(bus.cpuRead(NMI_ADDRESS));
            pch = static_cast<ubyte>(bus.cpuRead(NMI_ADDRESS + 1));
            reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl);
            break;

        case RESET:
            LOG_INSTR("RST", 0);
            pcl = static_cast<ubyte>(bus.cpuRead(RESET_ADDRESS));
            pch = static_cast<ubyte>(bus.cpuRead(RESET_ADDRESS + 1));
            reg_pc = (static_cast<uword>(pch) << 8) + static_cast<uword>(pcl);
            break;

        default:
            break;
    }
    // TODO test if RESET takes 7 cycles

    return 7;
}