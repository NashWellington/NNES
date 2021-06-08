#pragma once

// Include
#include "globals.h"
#include "bus.h"
#include "isa.h"

extern CPU cpu;

class CPU
{
public:
    byte read(uword address);
    void write(uword address, byte data);

    byte pop();
    void push(byte data);

    /* Return the byte pointed at by PC and increment PC
    */
    byte nextByte();

    /* Call nextByte() and execute that byte as an instruction
    */
    int executeInstruction();


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

    // CPU registers

    /* Program Counter
    * 16-bit register 
    * Holds the address of the next instruction executed
    */
    uword reg_pc = 0x8000;

    /* Stack Pointer
    * 8-bit register 
    * Used as an offset from $0100
    * No stack overflow detection (will wrap around)
    */
    ubyte reg_sp = 0xF0;

    /* Accumulator
    * 8-bit register
    * Stores the result of arithmetic and logic operations
    */
    byte reg_a = 0;

    /* Index Register X
    * 8-bit register
    * Counter or offset for certain addressing modes
    * Can be used to get or set the stack pointer
    */
    byte reg_x = 0;

    /* Index Register Y
    * 8-bit register
    * Counter or offset
    * Cannot affect the stack pointer
    */
    byte reg_y = 0;

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
        byte reg;
    } reg_sr = { .reg = 0x24 };

private:
    int cycle = 0;
    bool odd_cycle = false;

    int handleInterrupt(InterruptType type);
};