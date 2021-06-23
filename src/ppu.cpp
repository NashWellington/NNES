#include "ppu.h"

PPU ppu;

PPU::PPU() 
{
    loadSystemPalette();

    /* Initializing to a # larger than the width of the screen so sprite pixels
    * don't get drawn on screen
    */
    spr_x_pos.fill(0x00FF);
    spr_at_byte.fill(0xFF);
    spr_pt_byte_low.fill(0xFF);
    spr_pt_byte_high.fill(0xFF);
    show_spr.fill(false);
}

void PPU::loadSystemPalette()
{
    std::ifstream file("./palettes/ntsc.pal", std::ios::binary); // TODO put name in config file
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

/*TODO
void PPU::save(Savestate& savestate)
{

}
void PPU::load(Savestate& savestate)
{

}
*/

void PPU::tick()
{
    // TODO OAM ADDR set during sprite eval
    // TODO check if background/sprites should be loaded
    // TODO sprite checking/resolution/whatever

    if ((scanline >= 0 && scanline < POST_RENDER_START) || (scanline == PRE_RENDER_START)) // pre-render + visible
    {
        if ((cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 336)) // Get table bytes
        {
            // Background tile evaluation
            switch (cycle%8)
            {
                case 2: // NT byte
                {
                    uword nt_addr_base = 0x2000 
                        + (static_cast<uword>(bus.reg_ppu_ctrl.nn) * 0x0400);
                    bg_nt_byte = bus.ppuRead(nt_addr_base + 32 * reg_nt_row + reg_nt_col);
                    if (scanline != PRE_RENDER_START && scanline < 239 && cycle <= 256)
                        pushSpritePixels(); // TODO don't do this when pre-render or scanline 239
                    break;
                }
                case 4: // AT byte
                {
                    uword nt_addr_base = 0x2000 
                        + (static_cast<uword>(bus.reg_ppu_ctrl.nn) * 0x0400);
                    uword at_col = reg_nt_col / 4;
                    uword at_row = reg_nt_row / 4;
                    bg_at_byte = bus.ppuRead(nt_addr_base + 0x3C0 + at_row * 8 + at_col);

                    // TODO figure out if I need to do this now or later
                    // get palette info from attribute depending on quadrant
                    uint offset = 0;
                    if ((reg_nt_col/2) % 2 == 1) offset += 2;       // right quadrants -> 1100 0000 or 0000 1100
                    if ((reg_nt_row/2) % 2 == 1) offset += 4;       // bottom quadrants -> 1100 000 or 0011 0000
                    bg_at_byte = ((bg_at_byte & (0x03 << offset)) >> offset);
                    break;
                }
                case 6: // Low PT byte
                {
                    uword pt_addr = bus.reg_ppu_ctrl.b * 0x1000 + (static_cast<uword>(bg_nt_byte) << 4);
                    uword y_offset = scanline;
                    if (cycle >= 256) y_offset++;
                    y_offset %= 8;
                    bg_pt_byte_low = bus.ppuRead(pt_addr + y_offset);
                    break;
                }
                case 0: // High PT byte + inc hori
                {
                    uword pt_addr = bus.reg_ppu_ctrl.b * 0x1000 + (static_cast<uword>(bg_nt_byte) << 4);
                    uword y_offset = scanline;
                    if (cycle >= 256) y_offset++;
                    y_offset %= 8;
                    bg_pt_byte_high = bus.ppuRead(pt_addr + y_offset + 8);
                    if ((scanline != PRE_RENDER_START && cycle <= 240) 
                        || (cycle >= 321 && scanline != 239)) 
                    {
                        pushBackgroundPixels();
                        reg_nt_col++;
                        if (cycle == 240)
                        {
                            assert(reg_nt_col == 32);
                            reg_nt_col = 0;
                            if (scanline % 8 == 7)
                                reg_nt_row++;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            // Sprite clear
            if (cycle == 1 && scanline != PRE_RENDER_START)
            {
                for (uint i = 0; i < 8; i++)
                {
                    for (uint j = 0; j < 4; j++)
                    {
                        // Clear secondary OAM
                        bus.secondary_oam[i].data[j] = 0xFF;
                    }
                }

                bus.oam_data = 0xFF;
            }
            else if (cycle >= 2 && cycle <= 64 && scanline != PRE_RENDER_START)
            {
                // TODO not sure if more than one of these is necessary
                bus.oam_data = 0xFF;
            }
            // Sprite evaluation
            // Note: for now, this does it all in one go
            // TODO: make this cycle-accurate
            // TODO: emulate sprite overflow bug
            else if (cycle == 65 && scanline != PRE_RENDER_START) // cycles 65 to 256
            {
                uint sec_oam_i = 0; // Secondary OAM index
                ubyte y_range = bus.reg_ppu_ctrl.h ? 16 : 8;
                for (uint n = 0; n < 64; n++) // Primary oam index
                {
                    if (sec_oam_i < 8)
                    {
                        ubyte y = bus.primary_oam[n].y;
                        bus.secondary_oam[sec_oam_i].y = y;
                        if (y >= scanline+1 && y < scanline+1 + y_range)
                        {
                            bus.secondary_oam[sec_oam_i].x = bus.primary_oam[n].x;
                            bus.secondary_oam[sec_oam_i].tile_i = bus.primary_oam[n].tile_i;
                            bus.secondary_oam[sec_oam_i].attributes = bus.primary_oam[n].attributes;
                            sec_oam_i++;
                        }
                    }
                    else
                    {
                        for (uint m = 0; m < 4; m++) // m increment is a hardware bug
                        {
                            ubyte y = bus.primary_oam[n].data[m];
                            if (y >= scanline && y < scanline + y_range)
                            {
                                bus.reg_ppu_status.o = true;
                            }
                        }
                    }
                }
            }
        }
        else if ((cycle >= 257) && (cycle <= 320))
        {
            // Load secondary OAM data into latches, shift regs, etc.
            // TODO make this cycle-accurate
            if (scanline != PRE_RENDER_START && scanline < 239 && cycle == 257)
            {
                show_spr.fill(false); // Sprites are not shown by default
                ubyte y_range = bus.reg_ppu_ctrl.h ? 16 : 8;
                for (uint i = 0; i < 8; i++)
                {
                    ubyte y = bus.secondary_oam[i].y;
                    if (y >= scanline+1 && y < scanline+1 + y_range) 
                    {
                        show_spr[i] = true;
                        ubyte pt_y = y - (scanline+1); // y coord within the pattern table tile
                        ubyte attributes = bus.secondary_oam[i].attributes;
                        if (attributes & 0x80) 
                            pt_y = y_range - pt_y - 1; // Flip vertically

                        uword pt_addr = static_cast<uword>(bus.secondary_oam[i].tile_i);
                        uword pt_i;
                        if (y_range == 8) 
                        {
                            pt_addr *= 16;
                            pt_i = bus.reg_ppu_ctrl.s * 0x1000;
                            pt_addr += pt_i;
                            pt_addr += pt_y;
                        }
                        else
                        {
                            pt_i = (pt_addr & 0x01) * 0x1000;
                            pt_addr &= 0xFFFE;
                            pt_addr *= 16;
                            pt_addr += pt_y % 8;
                            if (pt_y >= 8) pt_addr += 16;
                        }

                        ubyte pt_low = bus.ppuRead(pt_addr);
                        ubyte pt_high = bus.ppuRead(pt_addr + 8);

                        if (attributes & 0x40) // Flip horizontally
                        {
                            pt_low = reverseByte(pt_low);
                            pt_high = reverseByte(pt_high);
                        }

                        spr_x_pos[i] = static_cast<int>(bus.secondary_oam[i].x);
                        spr_at_byte[i] = attributes;
                        spr_pt_byte_low[i] = pt_low;
                        spr_pt_byte_high[i] = pt_high;
                    }
                }
            }
            bus.cpuWrite(0x2003, 0);
        }
        else if ((cycle == 338) || (cycle == 340)) // Unused NT fetches
        {
            // This probably won't have an effect on anything
        }
        if (scanline == PRE_RENDER_START)
        {
            if (cycle == 1) // clear vblank, sprite 0 hit, and sprite overflow
            {
                bus.reg_ppu_status.o = 0;
                bus.reg_ppu_status.s = 0;
                bus.reg_ppu_status.v = 0;
                pixel_i = 0;
            }
            else if (cycle >= 280 && cycle <= 304) // Clear vblank???
            {

            }
            else if (cycle == 339) // skip last cycle of odd frame
            {
                if (odd_frame) cycle++;
                odd_frame = !odd_frame;
            }
        }
        // TODO where should I add pixels?
        else if (cycle % 8 == 4 && cycle <= 256)
        {
            addPixels();
        }
    }
    else // post-render
    {
        // set vblank and render image
        if ((scanline == 241) && (cycle == 1))
        {
            assert(reg_nt_row == 30);
            assert(reg_nt_col == 0);
            reg_nt_row = 0;
            bus.reg_ppu_status.v = 1;
            if (bus.reg_ppu_ctrl.v) bus.addInterrupt(NMI);
            sendFrame();
        }
    }
    cycle++;
    if (cycle >= CYCLES_PER_SCANLINE)
    {
        scanline++;
        cycle -= CYCLES_PER_SCANLINE;
    }
    if (scanline >= SCANLINES_PER_FRAME - 1)
    {
        scanline -= SCANLINES_PER_FRAME;
    }
    #ifdef DEBUGGER
    debug_state.pixel = cycle;
    debug_state.scanline = scanline;
    #endif
}

// TODO fine x scrolling
void PPU::pushBackgroundPixels()
{
    uint  bg_pal_i = bg_at_byte; // Palette RAM index (i.e. which palette is selected)
    uint  bg_color_i = 0;        // Color index (i.e. which one of four colors is selected)
    bool show_bg = (bus.reg_ppu_mask.left_background || (pixel_i % 256) >= 8) && bus.reg_ppu_mask.show_background;

    std::array<Pixel,4> palette = {};
    getPalette(palette, bg_pal_i);
    for (uint i = 0; i < 8; i++)
    {
        if (show_bg)
        {
            bg_color_i = (bg_pt_byte_low & (0x80 >> i)) >> (7-i);
            bg_color_i += ((bg_pt_byte_high & (0x80 >> i)) >> (7-i)) << 1;
        }
        else bg_color_i = 0;

        bg_pipeline.push(palette[bg_color_i]);
    }
}

// FIXME find a way to not draw some pixels
// FIXME fix transparency
void PPU::pushSpritePixels()
{
    std::array<Pixel,4> palette = {};
    for (uint i = 0; i < 8; i++)
    {
        uint spr_pal_i = 0;
        uint spr_color_i = 0;

        for (int j = 7; j >= 0; j--) // Iterates in reverse because lower index sprites have higher priority
        {
            if (show_spr[j])
            {
                if (spr_x_pos[j] > 0)
                    spr_x_pos[j]--;
                else if (spr_x_pos[j] > -8) 
                {
                    uint px = 0;
                    px = (spr_pt_byte_low[j] & 0x80) >> 7;
                    px += (spr_pt_byte_high[j] & 0x80) >> 6;
                    spr_pt_byte_low[j] <<= 1;
                    spr_pt_byte_high[j] <<= 1;
                    // TODO are bg-priority sprites considered?
                    if (px > 0 && (spr_at_byte[j] & 0x20) == 0)
                    {
                        spr_color_i = px;
                        spr_pal_i = (spr_at_byte[j] & 0x03) | 4;
                    }
                }
                else spr_x_pos[j] = 0x00FF;
            }
        }
        if (spr_color_i != 0)
        {
            assert(spr_pal_i < 8);
            assert(spr_color_i < 4);
            getPalette(palette, spr_pal_i);
            spr_pipeline.push(palette[spr_color_i]);
        }
        else
        {
            spr_pipeline.push({});
        }
    }
}

void PPU::addPixels()
{
    for (uint i = 0; i < 8; i++)
    {
        std::optional<Pixel> spr_px;
        if (!spr_pipeline.empty()) 
        {
            spr_px = spr_pipeline.front();
            spr_pipeline.pop();
        }
        else spr_px = {};

        Pixel bg_px = bg_pipeline.front();
        bg_pipeline.pop();

        if (spr_px) frame[pixel_i] = spr_px.value();
        else frame[pixel_i] = bg_px;
        pixel_i++;
    }
}

#ifdef DEBUGGER
void PPU::addDebug()
{
    displayPatternTable(0, display.palette_selected);
    displayPatternTable(1, display.palette_selected);
    for (uint i = 0; i < 4; i++) displayNametable(i);
    for (uint i = 0; i < 8; i++) displayPalette(i);
    displaySprites();
}

Tile PPU::getPTTile(uword address, std::array<Pixel,4>& palette)
{
    Tile tile = {};
    // Iterate thru 8 (x2) bytes per tile
    for (uint y = 0; y < 8; y++)
    {
        // TODO pattern table index
        ubyte pattern_lsb = bus.ppuRead(address + y);
        ubyte pattern_msb = bus.ppuRead(address + y + 8);

        // Iterate thru bits in each byte
        for (uint x = 0; x < 8; x++)
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
    attribute = bus.ppuRead(0x2000 + 0x0400 * nt_i + 0x03C0 + nt_col/4 + 8 * (nt_row/4));

    // get palette info from attribute depending on quadrant
    if (((nt_col / 2) % 2) == 1) offset += 2; // right quadrants -> 1100 0000 or 0000 1100
    if (((nt_row / 2) % 2) == 1) offset += 4; // bottom quadrants -> 1100 000 or 0011 0000
    palette_index = ((attribute & (0x03 << offset)) >> offset);

    getPalette(palette, palette_index);

    // get tile coords from nametable(i,j)
    ubyte pt_coords = bus.ppuRead(0x2000 + 0x0400 * nt_i + nt_col + 32 * nt_row);
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
    ubyte spr_index = bus.primary_oam[spr_i].data[1];
    uword pt_addr = static_cast<uword>(spr_index & 0x01) << 12;
    pt_addr += static_cast<uword>(spr_index & 0xFE);
    ubyte attributes = bus.primary_oam[spr_i].data[2];
    uint pal_i = (attributes & 0x03) | 4;
    std::array<Pixel,4> palette = {};
    getPalette(palette, pal_i);
    bool flip_hori = attributes & 0x40;
    bool flip_vert = attributes & 0x80;
    Tile pt_tiles[2] = {getPTTile(pt_addr, palette), getPTTile(pt_addr+1, palette)};
    uint spr_x = 0;
    uint spr_y = 0;
    for (uint y = 0; y < 16; y++)
    {
        spr_y = flip_vert ? 16-y : y;
        for (uint x = 0; x < 8; x++)
        {
            if (flip_hori) spr_x = 8-x;
            else spr_x = x;
            big_sprites[(spr_i/8) * 8 + spr_y][(spr_i%8) * 8 + spr_x] = pt_tiles[y/8][y%8][x];
        }
    }
}

void PPU::getSmallSprite(uint spr_i)
{
    uword pt_addr = static_cast<uword>(bus.reg_ppu_ctrl.s) << 12; // Palette index (0 or 1)
    pt_addr += static_cast<uword>(bus.primary_oam[spr_i].data[1]);
    ubyte attributes = bus.primary_oam[spr_i].data[2];
    uint pal_i = (attributes & 0x03) | 4;
    std::array<Pixel,4> palette = {};
    getPalette(palette, pal_i);
    bool flip_hori = static_cast<bool>((attributes & 0x40) >> 6);
    bool flip_vert = static_cast<bool>((attributes & 0x80) >> 7);
    Tile pt_tile = getPTTile(pt_addr, palette);
    uint spr_x = 0;
    uint spr_y = 0;
    for (uint y = 0; y < 8; y++)
    {
        spr_y = flip_vert ? 16-y : y;
        for (uint x = 0; x < 8; x++)
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
    display.pal_tex[pal_i].update(4, 1, reinterpret_cast<void*>(&curr_palette));
}

void PPU::displayPatternTable(uint pt_i, uint palette_index)
{
    assert(pt_i < 2);
    std::array<Pixel,4> palette;
    getPalette(palette, palette_index);
    getPatternTable(pt_i, palette);
    display.pt_tex[pt_i].update(128, 128, reinterpret_cast<void*>(&pattern_table));
}

void PPU::displayNametable(uint nt_i)
{
    getNametable(nt_i, bus.reg_ppu_ctrl.b);
    display.nt_tex[nt_i].update(256, 240, reinterpret_cast<void*>(&nametable));
}

void PPU::displaySprites()
{
    if (!bus.reg_ppu_ctrl.h) // 8x8 sprites
    {
        for (uint i = 0; i < 64; i++)
        {
            getSmallSprite(i);
        }
        display.spr_tex.update(64, 64, reinterpret_cast<void*>(&small_sprites));
    }
    else // 8x16 sprites
    {
        for (uint i = 0; i < 64; i++)
        {
            getBigSprite(i);
        }
        display.spr_tex.update(64, 128, reinterpret_cast<void*>(&big_sprites));
    }
}
#endif

// TODO handle ppu mask color modifier
void PPU::getPalette(std::array<Pixel,4>& palette, uint palette_index)
{
    assert(palette_index < 8);
    palette[0] = getColor(bus.ppuRead(0x3F00));
    palette[1] = getColor(bus.ppuRead(0x3F00 + 4 * palette_index + 1));
    palette[2] = getColor(bus.ppuRead(0x3F00 + 4 * palette_index + 2));
    palette[3] = getColor(bus.ppuRead(0x3F00 + 4 * palette_index + 3));
}

// TODO grayscale + color emphasis PPUMASK flags
Pixel PPU::getColor(ubyte color_byte)
{
    assert(color_byte < 0x40);
    return system_palette[color_byte];
}

void PPU::sendFrame()
{
    display.frame_tex.update(256, 240, reinterpret_cast<void*>(&frame));
}