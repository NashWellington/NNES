#pragma once

// TODO make this agnostic to memory/NES specifics

#include "globals.h"
#include <queue>
#include <algorithm>

/* Disassemble one instruction (1-3 bytes)
* Example format: LDA $2727,X
*/
std::vector<std::string_view> disassemble(std::queue<ubyte>& byte_queue) noexcept;

// TODO parameterize
// Reverse the order of bits in a byte
ubyte reverseByte(ubyte b) noexcept;

// Returns the value for a command line option, if it exists
std::optional<std::string_view> getOpt(std::vector<std::string_view> args, std::string_view option) noexcept;