#pragma once

#include "globals.h"

struct Savestate
{
    std::vector<byte> cpuMemory = {};
    std::vector<byte> ppuMemory = {};
    std::vector<byte> rom = {}; // For most mappers it will be unnecessary to use this
    struct
    {
        uword reg_pc = 0;
        ubyte reg_sp = 0;
        byte reg_a = 0;
        byte reg_x = 0;
        byte reg_y = 0;
        byte reg_sr = 0;
    } registers = {};
    int cpu_cycles_left = 0;
    bool cpu_odd_cycle = false;
};

extern std::vector<Savestate> savestates;

#ifndef NDEBUG
extern Savestate debug_state; // Current state of the emulated NNES
#endif