#pragma once

// Forward declaration
class NES;
class Video;

#include "globals.hpp"
#include "processor.hpp"
#include "mem.hpp"
#include "video.hpp"
#include "savestate.hpp"
#include "util.hpp"

class PPU : public Processor
{
public:
    PPU(NES& _nes, Video& _video);
    void setRegion(Region _region);
    void reset();
    // void save(Savestate& savestate) { return; }
    // void load(Savestate& savestate) { return; }

    void sendFrame();

    /* Advance the PPU 1 cycle
    */
    void tick();

    ubyte read(uword address);
    void write(uword address, ubyte data);

    enum Revision
    {
        REV_2C02, REV_2C03, REV_2C04,
        REV_2C05, REV_2C07, UMC 
    } revision = REV_2C02;

private:
    /* Returns one Pixel
    * palette - one of eight palettes to be used
    * pal_i   - the index within that palette (one of four)
    */
    inline Pixel getColor(ubyte palette, ubyte pal_i);
    inline ubyte getNTByte();
    inline ubyte getAttribute();
    inline ubyte getPTByte(uint table, uint tile, uint bit_plane, uint fine_y);
    inline void evalSprites();
    inline void fetchSprites();
    inline void fetchTiles();
    inline void dummyFetchTiles(); // Unused tile fetch
    inline bool sprEnabled();
    inline bool  bgEnabled();
    inline bool renderEnabled();
    inline void incrHoriV();
    inline void incrVertV();
    inline bool checkRange(ubyte y);
    inline void incrCycle();
    inline void pushPixel(Pixel p);
    inline void renderPixel();
    inline void processScanline();

    NES& nes;
    Video& video;

    // The current cycle (resets at the start of a new scanline)
    int cycle = 0;

    // The current scanline
    int scanline = -1;

    bool odd_frame = false;

    int post_render_start = 240;
    int vblank_start = 241;
    int render_end = 260; // last scanline

    ubyte ppu_io_open_bus = 0;
    ubyte ppu_vram_open_bus = 0;

    // Prevents CPU writes from start/reset until the end of vblank
    bool reset_signal = true;

    std::array<Pixel,61440> ppu_frame = {};

// Background data
    // Scrolling
    union VramAddr
    {
        struct
        {
            unsigned coarse_x  : 5;
            unsigned coarse_y  : 5;
            unsigned nametable : 2;
            unsigned fine_y    : 3;
            unsigned           : 1;
        };
        uword addr = 0;
    };
    VramAddr vram_addr = {};
    VramAddr tmp_vram_addr = {};
    ubyte fine_x_scroll = 0;
    bool write_toggle = 0; // First/second write toggle for $2005/$2006

    // TODO byte reversal to make this easier?
    template<typename T>
    struct ShiftReg // TODO Left-shift, right?
    {
        T reg = 0;
        uint bits = 0;
        void load(ubyte val)
        {
            assert(bits%8 == 0);
            if (bits > 0) shift(8);
            reg |= val;
            bits += 8;
        }
        void shift(uint num_bits)
        {
            reg <<= num_bits;
            bits -= num_bits;
        }
        ubyte peekBit(uint bit)
        {
            uint bit_i = sizeof(T)*8 - (bit+1);
            return (reg & (1 << bit_i)) >> bit_i; 
        }
        ubyte peekByte(uint byte_i)
        {
            return reg >> ((sizeof(T)-1-byte_i) * 8);
        }
    };

    ShiftReg<uword> bg_pt_low = {};
    ShiftReg<uword> bg_pt_high = {};
    ShiftReg<uword> bg_at = {};

    ubyte bg_nt_latch = 0;
    ubyte bg_at_latch = 0;
    ubyte bg_pt_low_latch = 0;
    ubyte bg_pt_high_latch = 0;

// Sprite data

    // Determines which part of the 4-step process evalSprites() executes
    int spr_eval_seq = 1;

    // Sprite zero is present on the next scanline
    bool spr_zero_next = false;
    // Sprite zero is present on the current scanline
    bool spr_zero_curr = false;

    // Primary OAM

    // Secondary OAM

    // Pattern table data for 8 sprites
    // unused sprites loaded w/ transparent set of vals
    // shift regs
    std::array<ShiftReg<ubyte>,8> spr_pt_low = {};
    std::array<ShiftReg<ubyte>,8> spr_pt_high = {};

    // Attribute bytes for 8 sprites
    std::array<ubyte,8> spr_at = {};

    // X-position counters for 8 sprites
    // In an actual NES these would count down every cycle, but that's not necessary here
    std::array<ubyte,8> spr_x_pos = {};

    // Sprite eval counters
    ubyte spr_eval_m = 0;
    ubyte sec_oam_addr = 0; // Index in secondary OAM

    // Temporary values for holding sprite data while doing fetches
    ubyte tmp_spr_y = 0; // y-coord within pattern tile
    ubyte tmp_spr_tile_i = 0;

// Register data
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
            unsigned nn         : 2;
            unsigned incr       : 1;
            unsigned spr_table  : 1;
            unsigned bg_table   : 1;
            unsigned spr_size   : 1;
            unsigned p          : 1;
            unsigned vblank_nmi : 1;
        };
        ubyte reg = 0;
    } reg_ctrl = {};
        

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
        ubyte reg = 0;
    } reg_mask = {};

    /* PPU status register
    * $2002
    *
    * 7 6 5 4   3 2 1 0
    * V S O
    * 
    * O - sprite overflow
    * S - sprite 0 hit
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
            unsigned             : 5; // unused
            unsigned overflow    : 1;
            unsigned sprite_zero : 1;
            unsigned vblank      : 1;
        };
        ubyte reg = 0xA0;
    } reg_status = {};

    /* PPU scrolling position register
    * $2005
    * On boot & reset: 0
    */
    // sets tmp_vram_addr and fine_x_scroll

    /* PPU address register
    * $2006
    * On boot:  0
    * On reset: unchanged
    */
    uword ppu_addr = 0;

    /* PPU data port
    * $2007
    * On boot & reset: 0
    * Used as the internal read buffer, updated & returned at CPU read
    */
    ubyte ppu_data = 0;

// PPU Object Attribute Memory
    struct Sprite
    {
        std::array<ubyte,4> data = {};
        ubyte& y            = data[0]; // top pixels
        ubyte& tile_i       = data[1]; // pattern table tile index
        ubyte& attributes   = data[2];
        ubyte& x            = data[3]; // left pixels
    };

public:

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
    uword dma_addr = 0;
};