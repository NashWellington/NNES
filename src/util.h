#pragma once

#include "globals.h"
#include "bus.h" // For accessing and decompiling mem

// Disassemble one line (1-3 bytes)
std::optional<std::string> disassemble(uword& address);

// Get 16 bytes of memory to display on a line
std::string peekMem(uword address);

// TODO parameterize
// Reverse the order of bits in a byte
ubyte reverseByte(ubyte b);