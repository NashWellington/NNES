#pragma once

#include "globals.h"
#include "apu.h"
// TODO include CPU and PPU probably
#include "mappers/mapper.h"
#include "savestate.h"

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

class Bus
{
public:
    // TODO make an arg for cpuRead that doesn't allow register state change on read
    ubyte    cpuRead(uword address);
    void    cpuWrite(uword address, ubyte data);

    // PPU memory access
    ubyte    ppuRead(uword address);
    void    ppuWrite(uword address, ubyte data);

    // Register I/O
    ubyte cpuReadReg(uword address);
    void cpuWriteReg(uword address, ubyte data);

    // Write to OAM (if necessary) and decrement cpu_suspend_cycles
    bool oamWrite(bool odd_cycle);

    void save(Savestate& savestate);
    void load(Savestate& savestate);

    // Interrupts
    InterruptType getInterrupt();
    void addInterrupt(InterruptType interrupt);
    void clearInterrupt();

    void setMapper(std::shared_ptr<Mapper> m);

    //TODO start() and reset()
    void start();
    void reset();

private:
    std::shared_ptr<Mapper> mapper;
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
// PPU registers
// For more info: https://wiki.nesdev.com/w/index.php/PPU_registers
    /* PPU control register
    * $2000
    * Contains PPU operation control flags
    * 
    * 7 6 5 4   3 2 1 0
    * V P H B   S I N N
    *               Y X
    * 
    * NN - base nametable address
    *      0 = $2000, 1 = $2400, 2 = $2800, 3 = $2C00
    *      Note: just use this as the nametable index
    * YX - most significant bits of scrolling coords
    *      X=1: add 256 to X scroll pos
    *      Y=1: add 240 to Y scroll pos
    *  I - VRAM address increment per CPU read/write of PPUDATA
    *      0: add 1, going across; 1: add 32, going down
    *  S - Sprite pattern table address for 8x8 sprites
    *      0: $0000; 1: $1000; ignored in 8x16 mode
    *  B - Background pattern table address
    *      0 = $0000, 1 = $1000, 
    *      Note: just use this as the pattern table index
    *  H - Sprite size
    *      0: 8x8 px; 1: 8x16px
    *  P - PPU master/slave select
    *      0: read backdrop from EXT pins; 1: output color on EXT pins
    *      NOTE: P should never be set; it could potentially break a normal NES
    *  V - Generate an NMI at the start of vblank
    *      0: off, 1: on
    * 
    * On boot & reset: 0
    */
    union
    {
        struct
        {
            unsigned nn : 2;
            unsigned i  : 1;
            unsigned s  : 1;
            unsigned b  : 1;
            unsigned h  : 1;
            unsigned p  : 1;
            unsigned v  : 1;
        };
        ubyte reg;
    } reg_ppu_ctrl { .reg = 0 };
        

    /* PPU mask register
    * $2001
    * controls rendering of sprites and backgrounds
    * controls color effects
    * 
    * 7 6 5 4   3 2 1 0
    * B G R s   b M m G
    * 
    * G - Greyscale
    *     0 = normal color, 1 = greyscale
    * m - 1 = show background in leftmost 8 px of screen, 0 = hide
    * M - 1 = show sprites in leftmost 8 px of screen, 0 = hide
    * b - 1 = show background, 0 = hide
    * s - 1 = show sprites, 0 = hide
    * R - Emphasize red (green on PAL/Dendy)
    * G - Emphasize green (red on PAL/Dendy)
    * B - Emphasize blue
    * 
    * On boot & reset: 0
    */                          
    union
    {
        struct
        {
            unsigned greyscale          : 1;
            unsigned left_background    : 1;
            unsigned left_sprites       : 1;
            unsigned show_background    : 1;
            unsigned show_sprites       : 1;
            unsigned emphasize_red      : 1;
            unsigned emphasize_green    : 1;
            unsigned emphasize_blue     : 1;
        };
        ubyte reg;
    } reg_ppu_mask { .reg = 0 };

    /* PPU status register
    * $2002
    *
    * 7 6 5 4   3 2 1 0
    * V S O
    * 
    * O - sprite overflow
    *     read the docs for this one
    * S - sprite 0 hit
    *     read the docs for this one too
    * V - vblank start
    *     0 = not in vblank, 1 = in vblank
    * 
    * On boot:  +0+x xxxx
    * On reset: U??x xxxx
    * (key: ? = unknown, x = irrelevant, + = often set, U = unchanged)
    */
    union
    {
        struct
        {
            unsigned    : 5; // unused
            unsigned o  : 1;
            unsigned s  : 1;
            unsigned v  : 1;
        };
        ubyte reg;
    } reg_ppu_status { .reg = 0 };

    /* PPU scrolling position register
    * $2005
    * On boot & reset: 0
    */
    struct
    {
        ubyte x = 0; // ubytes so I don't have to deal w/ sign extension
        ubyte y = 0;
        unsigned int i = 0;
        void write (ubyte data)
        {
            if (i == 0) x = data;
            else y = data;
            i++;
            i %= 2;
        }
    } reg_ppu_scroll;

    /* PPU address register
    * $2006
    * On boot:  0
    * On reset: unchanged
    */
    struct
    {
        uword address = 0;
        unsigned int i = 0;
        void write (ubyte data) // Alternates between writing to low and high bytes of address
        {
            uword mask = static_cast<uword>(0xFF << (8 * i));
            address &= mask;
            address += static_cast<uword>(data << static_cast<ubyte>(8 * (1 - i)));
            address %= 0x4000;
            i++;
            i %= 2;
        }
    } reg_ppu_addr {.address = 0};

    /* PPU data port
    * $2007
    * On boot & reset: 0
    * Used as the internal read buffer, updated & returned at CPU read
    */
    ubyte reg_ppu_data = 0;

// I/O registers
    bool poll_inputs = false;

    /* Inputs go through this buffer first
    * They get transferred to joypad_data when the CPU writes
    * to $4016 with bit 0 set to 1
    */
    ubyte joypad_data_buffer[2] = {};

    // 8 inputs from joypads 1 and 2
    // NOTE: I made this a ubyte so I don't have to deal w/ ASR/LSR sign extension
    ubyte joypad_data[2] = {};

    /* Input ports 1 and 2
    * $4016 and $4017
    */
    union
    {
        struct
        {
            unsigned d0 : 1; // NES standard controller + Famicom hardwired controller
            unsigned d1 : 1; // TODO the rest of these
            unsigned d2 : 1;
            unsigned d3 : 1;
            unsigned d4 : 1;
            unsigned    : 3;
        };
        ubyte reg;
    } reg_input[2] {{.reg = 0}, {.reg = 0}};

// PPU Object Attribute Memory
    struct Sprite
    {
        std::array<ubyte,4> data = {};
        ubyte& y            = data[0]; // top pixels
        ubyte& tile_i       = data[1]; // pattern table tile index
        ubyte& attributes   = data[2];
        ubyte& x            = data[3]; // left pixels
    };

    // Holds 8 4-byte sprites for the next scanline
    std::array<Sprite,8> secondary_oam = {};
    std::array<Sprite,64> primary_oam = {}; // Contains 64 4-byte sprites

    /* OAM address
    * $2003
    * On boot:  0
    * On reset: unchanged
    */
    ubyte oam_addr = 0;

    /* OAM Data
    * $2004
    * This is the value returned if register $2004 is read.
    * Details on what the value should be on a given cycle are detailed here:
    * http://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation
    * On boot & reset: ?
    */
    ubyte oam_data = 0;

    /* Address in CPU memory for DMA transfer to OAM
    * High byte controlled by register $4014
    * On boot & reset: unspecified
    */
    uword dma_addr;

// Other PPU internals
    // Detailed here: https://wiki.nesdev.com/w/index.php/PPU_registers#Ports
    ubyte ppu_latch = 0;

// Other
    int cpu_suspend_cycles = 0; // Used to stall CPU during OAM DMA
};

extern Bus bus;