#pragma once

#include "globals.h"
#include "bus.h"
#include "display.h"
#include "savestate.h"
#include "util.h"

// TODO implement PAL/Dendy vals

// TODO move to globals?
// NOTE: these are just NTSC vals
const int SCANLINES_PER_FRAME = 262;
const int VISIBLE_SCANLINE_START = 0;   // scanlines 0-239
const int POST_RENDER_START = 240;      // scanlines 240-260
const int PRE_RENDER_START = -1;        // scanline  261
const int CYCLES_PER_SCANLINE = 341;    
const int PIXELS_PER_SCANLINE = 256;
const int FRAME_WIDTH = 256;
const int FRAME_HEIGHT = 240;

typedef std::array<std::array<Pixel,8>,8> Tile;

template <size_t H, size_t W>
struct Table
{
    std::array<std::array<Pixel,W>,H> tiles = {};
    void addTile(Tile tile, uint row, uint col)
    {
        for (uint y = 0; y < 8; y++)
        {
            for (uint x = 0; x < 8; x++)
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

    // void save(Savestate& savestate);
    // void load(Savestate& savestate);

private:
    std::array<Pixel, 64> system_palette = {};

    // The current cycle (resets at the start of a new scanline)
    int cycle = 0;

    // The current scanline
    int scanline = -1;

    // Keeps track of NT tile index while rendering
    uword reg_nt_row = 0;
    uword reg_nt_col = 0;

    // Background pixel latches, shift registers, etc.
    ubyte bg_nt_byte = 0;      // Nametable byte
    ubyte bg_at_byte = 0;      // Attribute table byte
    ubyte bg_pt_byte_low = 0;  // Background pattern table bytes
    ubyte bg_pt_byte_high = 8;
    std::queue<Pixel> bg_pipeline = {}; // Holds the palette index and the palette ram index

    // Sprite pixel latches, shift registers, etc.
    std::array<int,8> spr_x_pos = {};
    std::array<ubyte,8> spr_at_byte = {};       // Attribute bytes
    std::array<ubyte,8> spr_pt_byte_low = {};
    std::array<ubyte,8> spr_pt_byte_high = {};  
    std::array<bool,8> show_spr = {};           // Toggles to false if sprite data not loaded from secondary oam
    std::queue<std::optional<Pixel>> spr_pipeline = {};

    uint32_t pixel_i = 0;                   // Index in the frame array
    std::array<Pixel,61440> frame = {};          // TODO change to variables for PAL support

    bool odd_frame = false;

    // Push 8 pixels to a pipeline (queue)
    void pushBackgroundPixels();
    void pushSpritePixels();

    // Add a horizontal line of 8 pixels from the pipeline to the frame
    void addPixels();

    void getPalette(std::array<Pixel,4>& palette, uint palette_index);
    Pixel getColor(ubyte color_byte);

#ifdef DEBUGGER
private: // Debugging tools
    std::array<Pixel,4> curr_palette = {}; // Holds the pixels for the next palette to be displayed
    Table<240,256> nametable = {};
    Table<128,128> pattern_table = {};
    std::array<std::array<Pixel,64>,64> small_sprites = {};
    std::array<std::array<Pixel,64>,128> big_sprites = {};

    Tile getPTTile(uword address, std::array<Pixel,4>& palette);
    void getPatternTable(uint pt_i, std::array<Pixel,4>& palette);

    void getNTTile(uint i, uint j, uint nt_i, uint pt_i);
    void getNametable(uint nt_i, uint pt_i);

    void getSmallSprite(uint spr_i); // 8x8 sprite
    void getBigSprite(uint spr_i); // 8x16 sprite
public:
    // TODO name this better
    void addDebug(); // Adds all palettes, tables, sprites, etc. to display
    void displayPalette(uint pal_i);
    void displayPatternTable(uint pt_i, uint palette_index);
    void displayNametable(uint nt_i);
    void displaySprites();
#endif
};

extern PPU ppu;