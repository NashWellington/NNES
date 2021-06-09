#pragma once

#include "globals.h"
#include "bus.h" // For accessing and decompiling mem

#include <map>

extern std::map<byte,std::string> instructions;

// Decompile one line (1-3 bytes)
std::optional<std::string> decompile(uword& address);