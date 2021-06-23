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
        ubyte reg_a = 0;
        ubyte reg_x = 0;
        ubyte reg_y = 0;
        ubyte reg_sr = 0;
    } registers = {};
    int cpu_cycles_left = 0;
    bool cpu_odd_cycle = false;
    uint32_t ppu_cycle = 0;

    #ifdef DEBUGGER
    int pixel = 0;
    int scanline = 0;

    std::string filename = "";
    std::string header_type = "";
    uint mapper = 0;
    uint submapper = 0;
    uint64_t prg_rom_size = 0;
    uint64_t prg_ram_size = 0;
    uint64_t chr_rom_size = 0;
    uint64_t chr_ram_size = 0;
    #endif
};

extern std::vector<Savestate> savestates;

#ifdef DEBUGGER
extern Savestate debug_state; // Current state of the emulated NNES
#endif