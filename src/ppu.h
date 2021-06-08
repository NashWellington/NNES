#pragma once

#include "globals.h"
#include "bus.h"
#include "display.h"
#include "savestate.h" // TODO

// TODO implement PAL/Dendy vals

// TODO move to globals?
// NOTE: these are just NTSC vals
const int SCANLINES_PER_FRAME = 262;
const int VISIBLE_SCANLINE_START = 0;   // scanlines 0-239
const int POST_RENDER_START = 240;      // scanlines 240-260
const int PRE_RENDER_START = -1;       // scanline  261
const int CYCLES_PER_SCANLINE = 341;    
const int PIXELS_PER_SCANLINE = 256;    // TODO change to actual val
const int FRAME_WIDTH = 256;
const int FRAME_HEIGHT = 240;

typedef std::array<std::array<Pixel,8>,8> Tile;

template <size_t H, size_t W>
struct Table
{
    std::array<std::array<Pixel,W>,H> tiles = {};
    void addTile(Tile tile, int row, int col)
    {
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                tiles[8*row + y][8*col + x] = tile[y][x];
            }
        }
    }
};

class PPU
{
public:
    PPU();

    // Send frame to display
    void sendFrame();

    void tick();

    /* Initialize palette
    */
    void loadSystemPalette();

    void save(Savestate& savestate);
    void load(Savestate& savestate);

private:
    std::array<Pixel, 64> system_palette = {};

    std::array<std::array<byte,4>,8> secondary_oam = {};

    // The current cycle (resets at the start of a new scanline)
    int cycle = 0;

    // The current scanline
    int scanline = -1;

    // Values fetched during rendering
    ubyte nt_byte = 0;      // Nametable byte
    ubyte at_byte = 0;      // Attribute table byte
    ubyte pt_byte_low = 0;  // Background pattern table bytes
    ubyte pt_byte_high = 8;
    std::queue<Pixel> pixel_pipeline = {};
    //std::array<std::array<Pixel,256>,240> frame = {};

    bool odd_frame = false;

    // TODO implement
    Pixel getColor(byte color_byte);

#ifndef NDEBUG
public:
    void testTick(); // Used for testing PPU render timings. will probably deprecate at some point
    // TODO change back to 341 width
    std::array<std::array<Pixel,341>,262> test_frame = {};

private: // Debugging tools
    std::array<Pixel,4> curr_palette = {};
    Table<240,256> nametable = {};
    Table<128,128> pattern_table = {};
    std::array<std::array<Pixel,64>,64> small_sprites = {};
    std::array<std::array<Pixel,64>,128> big_sprites = {};

    void getPalette(std::array<Pixel,4>& palette, uint palette_index);

    Tile getPTTile(uword address, std::array<Pixel,4>& palette);
    void getPatternTable(uint pt_i, std::array<Pixel,4>& palette);

    void getNTTile(uint i, uint j, uint nt_i, uint pt_i);
    void getNametable(uint nt_i, uint pt_i);

    void getSmallSprite(uint spr_i); // 8x8 sprite
    void getBigSprite(uint spr_i); // 8x16 sprite
public:
    void displayPalette(uint pal_i);
    void displayPatternTable(uint pt_i, uint palette_index);
    void displayNametable(uint nt_i);
    void displaySprites();
#endif
};

extern PPU ppu;