#pragma once

// Include statements
#include "globals.h"

// Foreward declaration
byte nextByte();

#include "cpu.h"

// +1 if page cross & read instruction: absolute X&Y, indirect Y (not x?)
// +1 if page cross (always): relative

namespace Mode
{
    /* Returns a memory address specified by one operand
    * For un-indexed, offset = 0
    * For indexed, offset = X or Y
    * cycles: 3 (minimum) - internal
    *           +0 if read instr
    *           +0 if write instr
    *           +2 if read-modify-write instr - external
    *           extra +1 if indexed (all instrs) - external
    */
    std::pair<uword,int> zeroPage(byte offset);

    /* Returns a memory address specified by two operands
    * For un-indexed, offset = 0
    * For indexed, offset = X or Y
    * cycles: 3 (minimum) - internal
    *           +0 if JMP instr
    *           +1 if read instr - external
    *               extra +1 if indexed AND page cross - internal (only add if page cross)
    *           +1 if write instr - external
    *               extra +1 if indexed - external
    *           +3 if read-modify-write instr - external
    *               extra +1 if indexed - external
    */
    std::pair<uword,int> absolute(byte offset, bool read_instr);

    /* Returns a memory address specified indirectly by two operands
    * Read the docs to understand
    * Un-indexed: index_type = 0
    * Pre-indexed: index_type = 1
    * Post-indexed: index_type = 2
    * cycles: 5 (minimum) - internal
    *           +0 for un-indexed
    *           +1 for pre-indexed - external
    *               extra +2 for read-modify-write instrs - external
    *           +0 for post-indexed
    *               extra +0 for read instrs
    *                   extra +1 if page boundary is crossed - internal
    *               extra +1 for write instrs - external
    *               extra +3 for read-modify-write instrs - external
    *               
    */
    std::pair<uword,int> indirect(byte offset, ubyte index_type, bool read_instr);

    /* Returns one operand (the next byte in memory) to be operated on
    * cycles: 2
    */
    std::pair<byte,int> immediate();

    /* Calculates the address the PC should branch to
    * cycles: 3 (minimum)
    *           +1 if branch is taken - external
    *               extra +1 if page cross - internal
    */
    std::pair<uword,int> relative();
};