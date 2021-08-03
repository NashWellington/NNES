#pragma once

#include "globals.hpp"

struct Savestate
{
    std::vector<byte> cpuMemory = {};
    std::vector<byte> ppuMemory = {};
    std::vector<byte> rom = {}; // For most mappers it will be unnecessary to use this
    struct
    {
        uword reg_pc = 0;
        ubyte reg_sp = 0;
        ubyte reg_a = 0;
        ubyte reg_x = 0;
        ubyte reg_y = 0;
        ubyte reg_sr = 0;
    } registers = {};
    int cpu_cycles_left = 0;
    bool cpu_odd_cycle = false;
    uint32_t ppu_cycle = 0;
};

extern std::vector<Savestate> savestates;