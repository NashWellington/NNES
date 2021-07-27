#pragma once

// Forward declaration
class NES;

#include "globals.hpp"
#include "console.hpp"
#include "savestate.hpp"

/* 64 KiB CPU memory map
* $0000 - Zero Page
* $0100 - Stack
* $0200 - RAM
* $0800 - Mirrors
*           - three consecutive mirrors of $0000 to $07FF
* $2000 - I/O Registers
* $2008 - Mirrors
*           - mirrors $2000 to $2007 every 8 bytes
* $4000 - I/O Registers
* $4020 - Expansion ROM (cartridge)
* $6000 - SRAM (cartridge)
*           - Save RAM; addresses used to access RAM in the cartridges for storing save games
* $8000 - PRG-ROM (Lower Bank) (cartridge)
* $C000 - PRG-ROM (Upper Bank) (cartridge)
*           - $FFFA-$FFFF vector table of addresses to jump to in case of an interrupt
*/

/* 16 KiB PPU memory map
* CHR-ROM (located on cartridge)
* $0000 - Pattern table 0
* $1000 - Pattern table 1
*
* VRAM:
* $2000 - Nametable 0
* $2400 - Nametable 1       <- mirrors 0 if vert mirroring
* $2800 - Nametable 2       <- mirrors 0 if horiz mirroring
* $2C00 - Nametable 3       <- mirrors 1 if horiz, 2 if vert
* $3000 - Mirrors of $2000 to $2EFF
*
* $3F00 - Palette RAM indices
* $3F20 - Mirrors of $3F00 to $3F1F
*/

class Memory
{
public:
    Memory(NES& _nes) : nes(_nes) {}

    // Randomize contents of RAM
    void start();

    // Probably unnecessary because RAM contents are retained after reset
    // void reset();

    ubyte    cpuRead(uword address);
    void    cpuWrite(uword address, ubyte data);

    // PPU memory access
    ubyte    ppuRead(uword address);
    void    ppuWrite(uword address, ubyte data);

    // Register I/O
    ubyte cpuReadReg(uword address);
    void cpuWriteReg(uword address, ubyte data);

    void save(Savestate& savestate);
    void load(Savestate& savestate);

    // Interrupts
    InterruptType getInterrupt();
    void addInterrupt(InterruptType interrupt);
    void clearInterrupt();

private:
    NES& nes;
    InterruptType current_interrupt = NO_INTERRUPT;

// CPU memory arrays
    /* Zero Page
    * $0000 to $00FF
    */
    std::array<ubyte, 0x0100> zero_page = {};

    /* Stack
    * $0100 to $01FF
    * Note: this is implemented as an array to keep SP operations simple
    */
    std::array<ubyte, 0x0100> stack = {};

    /* RAM
    * $0200 to $07FF
    */
    std::array<ubyte, 0x0600> ram = {};

    /* APU and I/O registers
    * $4000 to $4017
    * $4018 to $401F (test mode, should not access)
    */
    std::array<ubyte, 0x0020> io_regs = {};

// Cartridge memory not provided by the ROM
    /* Cartridge mem that should be at $4020 to $5FFF
    */
    
    std::array<ubyte, 0x1FD0> dummy_misc_ram = {};
    
    /* Cartridge mem that should be at $6000 to $7FFF
    */
    std::array<ubyte, 0x2000> dummy_prg_ram = {};

// PPU memory arrays
    /* Dummy pattern tables
    * Note: only used if pattern tables not present on the cartridge,
    *       generally for compatibility with test roms
    */
    std::array<ubyte, 0x2000> dummy_pattern_tables = {};

    /* Name tables
    * $2000 to $2FFF
    * Note: 4 nametables are addressable, but only 2 are present in memory.
    *       How these nametables (A and B) are addressed is determined 
    *       by mirroring (wihch is determined by the cartridge)
    * 
    * Horizontal Mirroring:
    *   A - $2000, $2400
    *   B - $2800, $2C00
    * 
    * Vertical Mirroring:
    *   A - $2000, $2800
    *   B - $2400, $2C00
    * 
    * Single-Screen:
    *   A - All (if SINGLE_SCREEN_LOWER)
    *   B - All (if SINGLE_SCREEN_UPPER)
    * 
    * Four-Screen:
    *   A - $2000
    *   B - $2400
    *   C - $2800 (on cart)
    *   D - $2C00 (on cart)
    * // TODO other types of mirroring, such as:
    *   Diagonal, L-Shaped, 3-screen (vert, horizontal, diagonal), 1-screen fixed
    */
    std::array<std::array<ubyte, 0x0400>, 2> name_tables = {};

    /* Palette RAM indices
    * $3F00 to $3F19
    */
    std::array<ubyte, 0x0020> palette_ram = {};

public:
    ubyte cpu_open_bus = 0;
};