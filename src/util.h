#pragma once

#include "globals.h"
#include "bus.h" // For accessing and decompiling mem

#include <map>

extern std::map<byte,std::string> instructions;

// Only decompiles 1 line for now
// TODO take an iterator as second arg?
std::optional<std::string> decompile(uword& address);