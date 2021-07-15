#pragma once

// Forward declaration
class Memory;

#include "globals.h"
#include "mem.h" // For accessing and decompiling mem

// Disassemble one line (1-3 bytes)
std::optional<std::string> disassemble(uword& address, std::shared_ptr<Memory> mem);

// Get 16 bytes of memory to display on a line
std::string peekMem(uword address, std::shared_ptr<Memory> mem);

// TODO parameterize
// Reverse the order of bits in a byte
ubyte reverseByte(ubyte b);