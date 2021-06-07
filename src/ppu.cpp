#include "ppu.h"

PPU ppu;

PPU::PPU() 
{
    loadSystemPalette();
}

void PPU::loadSystemPalette()
{
    std::ifstream file("../build/palettes/ntsc.pal", std::ios::binary); // TODO put name in config file
    if (!file.is_open())
    {
        std::cerr << "Error opening palette file" << std::endl;
        throw std::exception();
    }
    ubyte buffer = 0;
    for (Pixel& pixel : system_palette)
    {
        file.read(reinterpret_cast<char*>(&buffer), sizeof(buffer));
        pixel.r = buffer;
        file.read(reinterpret_cast<char*>(&buffer), sizeof(buffer));
        pixel.g = buffer;
        file.read(reinterpret_cast<char*>(&buffer), sizeof(buffer));
        pixel.b = buffer;
    }
    file.close();
}

void PPU::clock()
{
    // TODO bounds checking
    // TODO check vblank
    // TODO OAM ADDR set during sprite eval

    if ((scanline < POST_RENDER_START) || (scanline == PRE_RENDER_START)) // pre-render + visible
    {
        if (scanline == PRE_RENDER_START)
        {
            if (cycle == 1) // clear vblank
            {
                bus.reg_ppu_status.v = 0;
            }
            if (cycle == 339) // skip last cycle of odd frame
            {
                if (odd_frame) cycle++;
                odd_frame = !odd_frame;
            }
        }
        else
        {
            if ((cycle >= 1) && (cycle <= 256))
            {
                // TODO get palette somewhere around here
                //renderBackgroundPixel(pixel_x, pixel_y, bus.reg_ppu_ctrl.nn);
            }
        }
        if ((cycle >= 257) && (cycle <= 320))
        {
            bus.ppuWrite(0x2003, 0);
        }
    }
    else if (scanline < PRE_RENDER_START) // post-render
    {
        // set vblank and render image
        if ((scanline == 241) && (cycle == 1))
        {
            bus.reg_ppu_status.v = 1;
            if (bus.reg_ppu_ctrl.v) bus.addInterrupt(NMI);
            //sendFrame();
            // TODO testing
            #ifndef NDEBUG
            //TODO debug
            displayPatternTable(0, display.palette_selected);
            displayPatternTable(1, display.palette_selected);
            for (int i = 0; i < 4; i++) displayNametable(i);
            for (int i = 0; i < 8; i++) displayPalette(i);
            #endif
        }
    }
    cycle++;
    if (cycle >= CYCLES_PER_SCANLINE)
    {
        scanline++;
        cycle -= CYCLES_PER_SCANLINE;
    }
    if (scanline >= SCANLINES_PER_FRAME)
    {
        scanline -= SCANLINES_PER_FRAME;
    }
    Current_State.cycle = cycle;
}

#ifdef NDEBUG
void PPU::renderBackgroundPixel(int x, int y, int nt_i, std::array<Pixel, 4>& palette)
{
    // Palette for current tile

    // TODO move this out
    

    // get tile coords from nametable(x,y)
    ubyte coords = static_cast<ubyte>(bus.ppuRead(NAMETABLE_START 
        + NAMETABLE_OFFSET * nt_i + x + 32 * y));

    // TODO pattern table addressing is probably more complicated than this
    byte pattern_low = bus.ppuRead(PATTERN_TABLE_START + coords);
    byte pattern_high = bus.ppuRead(PATTERN_TABLE_START + coords + 8);

    int pattern = ((pattern_low & 0x80) >> 7) + ((pattern_high & 0x80) >> 6);
    background[x][y] = palette[pattern];
}

void PPU::renderBackgroundScanline(int y, int nt_i)
{
    std::array<Pixel, 4> palette = {};
    ubyte attribute = 0; // from the attribute table; used to find palette indices
    ubyte palette_index = 0; // can be 0-4 for 4 background palettes
    int offset = 0; // bit offset used to get palette number from palette byte

    for (int x = 0; x < FRAME_WIDTH; x++)
    {
        // get attribute every 4 tiles from attribute table
        if ((x % 32) == 0)
        {
            // get attribute byte
            attribute = bus.ppuRead(NAMETABLE_START 
                + NAMETABLE_OFFSET * nt_i 
                + ATTRIBUTE_TABLE_OFFSET + x/32 + 30 * y / 32);
        }

        // get palette info from attribute depending on quadrant
        offset = 0;
        if (((x / 16) % 2) == 0) offset += 2; // left quadrants
        if (((y / 16) % 2) == 1) offset += 4; // bottom quadrants
        palette_index = ((attribute & (0x03 << offset)) >> offset);

        // Copy palette colors to palette array
        palette[0] = getColor(bus.ppuRead(PALETTE_RAM_START)); // TODO not sure if first or last
        palette[1] = getColor(bus.ppuRead(PALETTE_RAM_START + 4 * palette_index) + 1);
        palette[2] = getColor(bus.ppuRead(PALETTE_RAM_START + 4 * palette_index) + 2);
        palette[3] = getColor(bus.ppuRead(PALETTE_RAM_START + 4 * palette_index) + 3);
        renderBackgroundPixel(x, y, nt_i, palette);
    }
}

void PPU::renderBackground()
{
    for (int y = 0; y < FRAME_HEIGHT; y++)
    {
        renderBackgroundScanline(y, bus.reg_ppu_ctrl.nn);
    }
}

#else

// TODO handle ppu mask color modifier
void PPU::getPalette(std::array<Pixel,4>& palette, uint palette_index)
{
    assert(palette_index < 8);
    palette[0] = getColor(bus.ppuRead(0x3F00));
    palette[1] = getColor(bus.ppuRead(0x3F00 + 4 * palette_index + 1));
    palette[2] = getColor(bus.ppuRead(0x3F00 + 4 * palette_index + 2));
    palette[3] = getColor(bus.ppuRead(0x3F00 + 4 * palette_index + 3));
}

Tile PPU::getPTTile(uword address, std::array<Pixel,4>& palette)
{
    Tile tile = {};
    // Iterate thru 8 (x2) bytes per tile
    for (int y = 0; y < 8; y++)
    {
        // TODO pattern table index
        ubyte pattern_lsb = bus.ppuRead(address + y);
        ubyte pattern_msb = bus.ppuRead(address + y + 8);

        // Iterate thru bits in each byte
        for (int x = 0; x < 8; x++)
        {
            ubyte pattern = (pattern_lsb & 0x01) + ((pattern_msb & 0x01) << 1);
            tile[y][7-x] = palette[pattern];
            pattern_lsb >>= 1;
            pattern_msb >>= 1;
        }
    }
    return tile;
}

void PPU::getPatternTable(uint pt_i, std::array<Pixel,4>& palette)
{
    uword address = 0;
    for (uword row = 0; row < 16; row++)
    {
        for (uword col = 0; col < 16; col++)
        {
            address = pt_i * 0x1000 + 16 * col + 256 * row;
            pattern_table.addTile(getPTTile(address, palette), row, col);
        }
    }
}

void PPU::getNTTile(uint nt_col, uint nt_row, uint nt_i, uint pt_i)
{
    std::array<Pixel, 4> palette = {};
    ubyte attribute = 0;
    int offset = 0;
    ubyte palette_index = 0;

    // get a byte from the attribute table used to find the palette index
    attribute = bus.ppuRead(0x2000 + 0x0400 * nt_i + 0x03C0 + ((nt_col/2) % 8) + (8 * (nt_row/2)));

    // get palette info from attribute depending on quadrant
    if (((nt_col / 2) % 2) == 0) offset += 2; // left quadrants -> 1100 0000 or 0000 1100
    if (((nt_row / 2) % 2) == 1) offset += 4; // bottom quadrants -> 1100 000 or 0011 0000
    palette_index = ((attribute & (0x03 << offset)) >> offset);

    getPalette(palette, palette_index);

    // get tile coords from nametable(i,j)
    ubyte pt_coords = static_cast<ubyte>(bus.ppuRead(0x2000 + 0x0400 * nt_i + nt_col + 32 * nt_row));
    uword pt_address = pt_i * 0x1000 + (static_cast<uword>(pt_coords) << 4) /*TODO + fine y offset*/;

    nametable.addTile(getPTTile(pt_address, palette), nt_row, nt_col);
}

void PPU::getNametable(uint nt_i, uint pt_i)
{
    for (uint nt_row = 0; nt_row < 30; nt_row++)
    {
        for (uint nt_col = 0; nt_col < 32; nt_col++)
        {
            getNTTile(nt_col, nt_row, nt_i, pt_i);
        }
    }
}

void PPU::getBigSprite(uint spr_i)
{
    ubyte spr_index = bus.oam[spr_i][1];
    uword pt_addr = static_cast<uword>(spr_index & 0x01) << 12;
    pt_addr += static_cast<uword>(spr_index & 0xFE);
    ubyte attributes = bus.oam[spr_i][2];
    uint pal_i = (attributes & 0x03) + 4;
    std::array<Pixel,4> palette = {};
    getPalette(palette, pal_i);
    bool flip_hori = static_cast<bool>((attributes & 0x40) >> 6);
    bool flip_vert = static_cast<bool>((attributes & 0x80) >> 7);
    Tile pt_tiles[2] = {getPTTile(pt_addr, palette), getPTTile(pt_addr+1, palette)};
    int spr_x = 0;
    int spr_y = 0;
    for (int y = 0; y < 16; y++)
    {
        if (flip_vert) spr_y = 16-y;
        else spr_y = y;
        for (int x = 0; x < 8; x++)
        {
            if (flip_hori) spr_x = 8-x;
            else spr_x = x;
            small_sprites[(spr_i/8) * 8 + spr_y][(spr_i%8) * 8 + spr_x] = pt_tiles[y/8][y%8][x];
        }
    }
}

void PPU::getSmallSprite(uint spr_i)
{
    uword pt_addr = static_cast<uword>(bus.reg_ppu_ctrl.s) << 12; // Palette index (0 or 1)
    pt_addr += static_cast<uword>(bus.oam[spr_i][1]);
    ubyte attributes = bus.oam[spr_i][2];
    uint pal_i = (attributes & 0x03) + 4;
    std::array<Pixel,4> palette = {};
    getPalette(palette, pal_i);
    bool flip_hori = static_cast<bool>((attributes & 0x40) >> 6);
    bool flip_vert = static_cast<bool>((attributes & 0x80) >> 7);
    Tile pt_tile = getPTTile(pt_addr, palette);
    int spr_x = 0;
    int spr_y = 0;
    for (int y = 0; y < 8; y++)
    {
        if (flip_vert) spr_y = 8-y;
        else spr_y = y;
        for (int x = 0; x < 8; x++)
        {
            if (flip_hori) spr_x = 8-x;
            else spr_x = x;
            small_sprites[(spr_i/8) * 8 + spr_y][(spr_i%8) * 8 + spr_x] = pt_tile[y][x];
        }
    }
}

void PPU::displayPalette(uint pal_i)
{
    getPalette(curr_palette, pal_i);
    display.addPalette(reinterpret_cast<ubyte*>(&curr_palette), pal_i);
}

void PPU::displayPatternTable(uint pt_i, uint palette_index)
{
    assert(pt_i < 2);
    std::array<Pixel,4> palette;
    getPalette(palette, palette_index);
    getPatternTable(pt_i, palette);
    display.addPatternTable(reinterpret_cast<ubyte*>(&pattern_table), pt_i);
}

void PPU::displayNametable(uint nt_i)
{
    getNametable(nt_i, bus.reg_ppu_ctrl.b);
    display.addNametable(reinterpret_cast<ubyte*>(&nametable), nt_i);
}

void PPU::displaySprites()
{
    if (!static_cast<bool>(bus.reg_ppu_ctrl.h)) // 8x8 sprites
    {
        for (int i = 0; i < 64; i++)
        {
            getSmallSprite(i);
        }
        display.addSprites(reinterpret_cast<ubyte*>(&small_sprites), 8);
    }
    else // 8x16 sprites
    {
        for (int i = 0; i < 64; i++)
        {
            getBigSprite(i);
        }
        display.addSprites(reinterpret_cast<ubyte*>(&big_sprites), 16);
    }
}
#endif

Pixel PPU::getColor(byte color_byte)
{
    assert(static_cast<ubyte>(color_byte) < 0x40);
    return system_palette[color_byte];
}

void PPU::sendFrame()
{
    //display.processFrame(background);
}