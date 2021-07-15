#pragma once

// Forward declaration
class Memory;

// Include
#include "globals.h"
#include "processor.h"
#include "mem.h"
#include "isa.h"
#include "savestate.h"

class CPU : public Processor
{
public:
    void setRegion(Region _region);

    ubyte read(uword address);
    void write(uword address, ubyte data);

    ubyte pop();
    void push(ubyte data);

    /* Return the byte pointed at by PC and increment PC
    */
    ubyte nextByte();

    /* Call nextByte() and execute that byte as an instruction
    */
    int executeInstruction();

    // Returns true if CPU is ready to execute another instruction
    bool ready();

    void tick();

    /* Execute 1 instruction or handle 1 interrupt
    */
    void step();

    /* Create a new interrupt
    * For use by isa.h
    */
    void addInterrupt(InterruptType interrupt);

    /* Set PC to the value located in the reset vector
    */
    void start();

    /* Reset CPU registers
    */
    void reset();

    void save(Savestate& savestate);
    void load(Savestate& savestate);

    std::shared_ptr<Memory> mem;

    // CPU registers

    /* Program Counter
    * 16-bit register 
    * Holds the address of the next instruction executed
    */
    uword reg_pc = 0xFFFC;

    /* Stack Pointer
    * 8-bit register 
    * Used as an offset from $0100
    * No stack overflow detection (will wrap around)
    */
    ubyte reg_sp = 0xFD;

    /* Accumulator
    * 8-bit register
    * Stores the result of arithmetic and logic operations
    */
    ubyte reg_a = 0;

    /* Index Register X
    * 8-bit register
    * Counter or offset for certain addressing modes
    * Can be used to get or set the stack pointer
    */
    ubyte reg_x = 0;

    /* Index Register Y
    * 8-bit register
    * Counter or offset
    * Cannot affect the stack pointer
    */
    ubyte reg_y = 0;

    /* Status Register
    *
    * 7 6 5 4   3 2 1 0
    * N V   B   D I Z C
    * 
    * C - Carry Flag
    *     Set if the last instructed resulted in overflow from bit 7 or underflow from bit 0
    * Z - Zero Flag
    *     Set if the last instructed resulted in a 0
    * I - Interrupt Disable
    *     Prevents the system from responding to IRQs
    *     Set by SEI (Set Interrupt Disable) instruction
    *     Unset by CLI (Clear Interrupt Disable) instruction
    * D - Decimal Mode
    *     Not implemented on the 2A03 (see documentation)
    * B - Break Command
    *     Indicates break instruction has been executed
    *     Causes IRQ
    *     Note: this one isn't a real flag, and has quirky behavior. Read the docs
    * V - Overflow Flag
    *     Set if the last instruction resulted in an invalid two's complemented result
    * N - Negative Flag
    *     Set if sign bit (TODO: of what?) is 1 (meaning negative)
    */
    union
    {
        struct 
        {
            unsigned c : 1;
            unsigned z : 1;
            unsigned i : 1;
            unsigned d : 1;
            unsigned b : 1;
            unsigned u : 1; // "unused" bit that is sometimes set or cleared
            unsigned v : 1;
            unsigned n : 1;
        };
        ubyte reg;
    } reg_sr = { .reg = 0x34 };

private:
    bool odd_cycle = false;

    int handleInterrupt(InterruptType type);
};