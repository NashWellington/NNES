#pragma once

#include "globals.h"
//#include "cpu.h"
#include <map>
#include <functional>

// TODO implement interrupt polling at the second to last tick or something

namespace ISA
{
    enum class InstrType
    {
        READ,
        WRITE,
        RMW,    // Read-Modify-Write
        NONE    // Used for JMP or modes where the instruction type doesn't matter
    };

    // Keeps track of things between cycles
    struct
    {
        // Address bitfield
        union
        {
            struct
            {
                ubyte addr_l;
                ubyte addr_h;
            };
            uword addr;
        } address {.addr = 0};

        uword fixed_address = 0; // Used to keep track of the address when indexing

        byte val = 0;

        bool flag_accu = false; // TODO make sure this returns to false after every instr
    } instr_data = {};

    void nextInstr(); // Chooses an instruction and calls its function

// Helper functions to be pushed by instructions
// TODO make these bools if they're all only 0 or 1 cycles??

    /* Read from address pointed to by instr_data.address.addr and throw val away
    */
    constexpr uint dummyRead();

    /* Read from address pointed to by PC and throw away
    * Does not increment PC on its own
    */
    constexpr uint dummyNextByte();

    /* Read from address pointed to by instr_data.address.addr
    * and store in instr_data.val
    */
    constexpr uint read();

    /* Write instr_data.val to address pointed by instr_data.address.addr
    */
    constexpr uint write();

// Addressing modes
    void impl(InstrType type);
    void  acc(InstrType type);
    void  imm(InstrType type);
    void  abs(InstrType type);
    void  zpg(InstrType type);
    void zpgX(InstrType type);
    void zpgY(InstrType type);
    void absX(InstrType type);
    void absY(InstrType type);
    void  rel(InstrType type);
    void indX(InstrType type);
    void indY(InstrType type);
    void  ind(InstrType type); // indirect,AKA absolute indirect

// Interrupts

    void BRK(byte instr);
    void IRQ();
    void NMI();

/* Instructions - Read
* 
* cycles 2-X: figure out addr, read from addr
* ------- You are here ----------
* +0 cycles: do operation
*/

    void ADC(byte instr);
    void ORA(byte instr);

/* Instructions - Read-Modify-Write
*
* if not accumulator addressing mode:
* cycles 2-X: figure out addr, read from addr, write same val back to addr
* ------- You are here ----------
* +1 cycle: do operation, write new val to addr
* 
* if accumulator addressing mode:
* cycle 2: read val from accu
* ------- You are here -----------
* +0 cycles: do operation, write new val to accu
*/

/* Instructions - Write
* 
* cycles 2-X: figure out addr
* -------- You are here -----------
* +1 cycle: write register to addr
*/

// Instructions - Other
    void JMP(byte instr);
};