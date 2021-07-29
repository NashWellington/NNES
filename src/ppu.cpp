#include "ppu.hpp"

const std::array<ubyte,192> ntsc_palette =
{
     84,  84,  84,   0,  30, 116,   8,  16, 144,  48,   0, 136,  68,   0, 100,  92,   0,  48,  84,   4,   0,  60,  24,   0,  32,  42,   0,   8,  58,   0,   0,  64,   0,   0,  60,   0,   0,  50,  60,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    152, 150, 152,   8,  76, 196,  48,  50, 236,  92,  30, 228, 136,  20, 176, 160,  20, 100, 152,  34,  32, 120,  60,   0,  84,  90,   0,  40, 114,   0,   8, 124,   0,   0, 118,  40,   0, 102, 120,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    236, 238, 236,  76, 154, 236, 120, 124, 236, 176,  98, 236, 228,  84, 236, 236,  88, 180, 236, 106, 100, 212, 136,  32, 160, 170,   0, 116, 196,   0,  76, 208,  32,  56, 204, 108,  56, 180, 204,  60,  60,  60,   0,   0,   0,   0,   0,   0,
    236, 238, 236, 168, 204, 236, 188, 188, 236, 212, 178, 236, 236, 174, 236, 236, 174, 212, 236, 180, 176, 228, 196, 144, 204, 210, 120, 180, 222, 120, 168, 226, 144, 152, 226, 180, 160, 214, 228, 160, 162, 160,   0,   0,   0,   0,   0,   0
};

PPU::PPU(NES& _nes, Video& _video) : nes(_nes), video(_video)
{
    setRegion(Region::NTSC);
}

void PPU::setRegion(Region _region)
{
    region = _region;
    switch (region)
    {
        case Region::NTSC:
            name = "Ricoh 2C02";
            time_scale = 4;
            revision = REV_2C02;
            post_render_start = 240;
            vblank_start = 241;
            render_end = 260;
            break;
        case Region::PAL:
            name = "Ricoh 2C07";
            time_scale = 5;
            revision = REV_2C07;
            post_render_start = 239;
            vblank_start = 241;
            render_end = 311;
            break;
        case Region::Dendy:
            name = "UMC UA6538";
            time_scale = 5;
            revision = UMC;
            post_render_start = 239;
            vblank_start = 289;
            render_end = 311;
            break;
        default:
            std::cerr << "Error: unsupported region" << std::endl;
            throw std::exception();
            break;
    }
}

// http://wiki.nesdev.com/w/index.php/PPU_power_up_state
void PPU::reset()
{
    reg_ctrl.reg = 0;
    reg_mask.reg = 0;
    write_toggle = false; // $2005/6 latch clear
    tmp_vram_addr.addr = 0;
    vram_addr.addr = 0;
    fine_x_scroll = 0;
    ppu_data = 0;
    odd_frame = false;
    cycle = 0;
    scanline = -1;
    reset_signal = true;
    oam_dma_cycles_left = 0;
}

ubyte PPU::read(uword address)
{
    assert((address >= 0x2000 && address <= 0x2007) || address == 0x4014);
    ubyte data = 0;
    switch (address)
    {
        case 0x2000: // PPU control
            return ppu_io_open_bus;
        case 0x2001: // PPU mask
            return ppu_io_open_bus;
        case 0x2002: // PPU status
            // TODO race condition?
            write_toggle = 0;
            data |= (reg_status.reg & 0xE0) | (ppu_io_open_bus & 0x1F);
            reg_status.vblank = false;
            ppu_io_open_bus = data;
            return data;
        case 0x2003: // OAM address
            return ppu_io_open_bus;
        case 0x2004: // OAM data
            if (reg_status.vblank)
            {
                data = primary_oam[oam_addr/4].data[oam_addr%4];
            }
            else
            {
                data = oam_data;
            }
            ppu_io_open_bus = data;
            return data;
        case 0x2005: // PPU scroll
            return ppu_io_open_bus;
        case 0x2006: // PPU address
            return ppu_io_open_bus;
        case 0x2007: // PPU data
            data = ppu_data;
            ppu_data = nes.mem->ppuRead(vram_addr.addr);
            if (vram_addr.addr%0x4000 >= 0x3F00) data = ppu_data;
            if (renderEnabled() && scanline < post_render_start)
            {
                incrHoriV();
                incrVertV();
            }
            // else reg_ctrl.incr ? incrVertV() : incrHoriV();
            else vram_addr.addr += reg_ctrl.incr ? 32 : 1;
            return data;
        case 0x4014: // OAM DMA
            return ppu_io_open_bus;
        default:
            return 0;
    }
}

// Writes to any PPU port
void PPU::write(uword address, ubyte data)
{
    assert((address >= 0x2000 && address <= 0x2007) || address == 0x4014);
    switch (address)
    {
        case 0x2000: // PPU control
            if (!reset_signal)
            {
                // Changing NMI flag from 0 to 1 during vblank generates an NMI
                if (reg_status.vblank && !reg_ctrl.vblank_nmi && (data & 0x80))
                    nes.mem->addInterrupt(NMI);
                reg_ctrl.reg = data;
                ppu_io_open_bus = data;
                tmp_vram_addr.nametable = reg_ctrl.nn;
            }
            break;
        case 0x2001: // PPU mask
            if (!reset_signal)
            {
                reg_mask.reg = data;
                ppu_io_open_bus = data;
            }
            break;
        case 0x2002: // PPU status
            ppu_io_open_bus = data;
            break;
        case 0x2003: // OAM address
            // TODO 2C02 OAM corruption
            oam_addr = data;
            ppu_io_open_bus = data;
            break;
        case 0x2004: // OAM data
            // TODO no writing during vblank? or is that only a suggestion
            // TODO probably more
            primary_oam[oam_addr/4].data[oam_addr%4] = data;
            oam_addr++;
            oam_data = data;
            ppu_io_open_bus = data;
            break;
        case 0x2005: // PPU scroll
            if (!reset_signal)
            {
                if (!write_toggle) // X scroll
                {
                    tmp_vram_addr.coarse_x = data >> 3;
                    fine_x_scroll = data & 0x07;
                }
                else
                {
                    tmp_vram_addr.coarse_y = data >> 3;
                    tmp_vram_addr.fine_y = data & 0x07;
                }
                write_toggle = !write_toggle;
                ppu_io_open_bus = data;
            }
            break;
        case 0x2006: // PPU address
            if (!reset_signal)
            {
                tmp_vram_addr.addr &= 0x00FF << (write_toggle ? 8 : 0);
                tmp_vram_addr.addr |= data << (write_toggle ? 0 : 8);
                if (write_toggle) 
                {
                    tmp_vram_addr.addr &= 0x7FFF;
                    vram_addr.addr = tmp_vram_addr.addr;
                }
                write_toggle = !write_toggle;
                ppu_io_open_bus = data;
            }
            break;
        case 0x2007: // PPU data
            nes.mem->ppuWrite(vram_addr.addr, data);
            if (renderEnabled() && scanline < post_render_start)
            {
                incrHoriV();
                incrVertV();
            }
            // else reg_ctrl.incr ? incrVertV() : incrHoriV();
            else vram_addr.addr += reg_ctrl.incr ? 32 : 1;
            ppu_io_open_bus = data;
            break;
        case 0x4014: // OAM DMA
            dma_addr = data << 8;
            ppu_io_open_bus = data;
            // Be careful of this if implementing multithreading in the scheduler
            oam_dma_cycles_left = 514;
            break;
        default:
            // Should never be here b/c of assertion
            break;
    }
}

bool PPU::oamWrite(bool odd_cycle)
{
    assert(oam_dma_cycles_left <= 514);
    if (oam_dma_cycles_left == 0) return false;
    if (oam_dma_cycles_left > 512)
    {
        if (oam_dma_cycles_left == 514 && !odd_cycle) oam_dma_cycles_left -=1; // One less idle cycle if even
    }
    else
    {
        if (!(oam_dma_cycles_left % 2))
        {
            oam_data = nes.mem->cpuRead(dma_addr);
            dma_addr++;
        }
        else
        {
            primary_oam[oam_addr/4].data[oam_addr%4] = oam_data;
            oam_addr++;
        }
    }
    oam_dma_cycles_left--;
    return true;
}

void PPU::sendFrame()
{
    video.frame_tex.update(256, 240, reinterpret_cast<void*>(&ppu_frame));
}

// http://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
ubyte PPU::getNTByte()
{
    uword address = 0x2000 | (vram_addr.addr & 0x0FFF);
    assert(address >= 0x2000 && address < 0x3000);
    return nes.mem->ppuRead(0x2000 | (vram_addr.addr & 0x0FFF));
}

ubyte PPU::getAttribute()
{
    uword attr_addr = 0x23C0 | (vram_addr.addr & 0x0C00) 
        | ((vram_addr.addr >> 4) & 0x0038) | ((vram_addr.addr >> 2) & 0x0007);
    ubyte attribute = nes.mem->ppuRead(attr_addr);
    
    if((vram_addr.coarse_y/2) % 2 == 0) attribute &= 0x0F; // Top quads
    else { attribute &= 0xF0; attribute >>= 4; }  // bottom quads

    if ((vram_addr.coarse_x/2) % 2 == 0) attribute &= 0x03; // Left quads
    else { attribute &= 0x0C; attribute >>= 2; } // right quads

    assert(attribute < 4);
    return attribute;
}

ubyte PPU::getPTByte(uint table, uint tile, uint bit_plane, uint fine_y)
{
    return nes.mem->ppuRead((table << 12) | (tile << 4) | (bit_plane << 3) | fine_y);
}

bool PPU::sprEnabled()
{
    return !reg_status.vblank && reg_mask.show_sprites;
}

bool PPU::bgEnabled()
{
    return !reg_status.vblank && reg_mask.show_background;
}

bool PPU::renderEnabled()
{
    return reg_mask.show_sprites || reg_mask.show_background;
}

inline void PPU::incrHoriV()
{
    if (vram_addr.coarse_x == 31) 
    {
        vram_addr.coarse_x = 0;
        vram_addr.nametable ^= 1;
    }
    else vram_addr.coarse_x++;
}

inline void PPU::incrVertV()
{
    if (vram_addr.fine_y < 7) // incr fine y
    {
        vram_addr.fine_y++;
    }
    else // overflow fine y to coarse y
    {
        vram_addr.fine_y = 0;
        if (vram_addr.coarse_y == 29)
        {
            vram_addr.coarse_y = 0;
            vram_addr.nametable ^= 2;
        }
        else if (vram_addr.coarse_y == 31)
        {
            vram_addr.coarse_y = 0;
        }
        else vram_addr.coarse_y++;
    }
}

bool PPU::checkRange(ubyte y)
{
    return scanline >= y && scanline < y + (reg_ctrl.spr_size ? 16 : 8);
}

// http://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation#Details
// Some of the steps are rearranged so they work on a cycle-accurate basis
void PPU::evalSprites()
{
    assert(sec_oam_addr <= 8);
    if (cycle == 65) 
    {
        oam_addr = 0;
        spr_eval_seq = 1;
        sec_oam_addr = 0;
    }
    // Read from primary OAM on odd cycles
    if (cycle % 2 == 1)
    {
        assert(spr_eval_seq != 2);
        oam_data = primary_oam[oam_addr/4].data[oam_addr%4];
    }

    // Write to secondary OAM on even cycles
    else
    {
        if (spr_eval_seq == 1)
        {
            if (sec_oam_addr < 8)
            {
                // 1.  Read a sprite's y-coord to secondary OAM
                if ((oam_addr % 4) == 0)
                {
                    secondary_oam[sec_oam_addr].y = oam_data;
                }
                // 1.a Read the rest of sprite data to secondary OAM
                else
                {
                    // TODO is this all done in one step?
                    secondary_oam[sec_oam_addr].data[oam_addr%4] = oam_data;
                }
            } 
            //else read val from secondary OAM
            spr_eval_seq = 2;
        }
        else if (spr_eval_seq == 3)
        {
            if (checkRange(oam_data))
            {
                if (renderEnabled() && oam_data < 240)
                    reg_status.overflow = true;
                // TODO read next 3 entries
                oam_addr++;
            }
            else 
            {
                oam_addr += 5;
                if (oam_addr/4 == 0) spr_eval_seq = 4;
            }
        }

        if (spr_eval_seq == 2)
        {
            // 2.  Increment n
            if ((oam_addr%4 == 0) && !checkRange(oam_data))
            {
                // 2.a If n has overflowed back go 0, goto 4
                if (oam_addr >= 252) spr_eval_seq = 4;
                oam_addr += 4;
            }
            else 
            {
                // Sprite zero will be present on the next scanline
                if (oam_addr == 0) spr_zero_next = true;

                // 2.a If n has overflowed back go 0, goto 4
                if (oam_addr == 255) spr_eval_seq = 4;
                oam_addr++;
                if (oam_addr%4 == 0) sec_oam_addr++;
            }

            if (spr_eval_seq != 4)
            {

                // 2.b If less than 8 sprites have been found, go to 1
                if (sec_oam_addr < 8) spr_eval_seq = 1;

                // 2.c If exactly 8 sprites have been found, disable writes to secondary OAM
                // This is already done by keeping track of sec_oam_addr
                else spr_eval_seq = 3;
            }
        }  
    }
}

void PPU::fetchSprites()
{
    uint spr_i = (cycle-257)/8;
    switch (cycle % 8)
    {
        // Read garbage NT byte
        // Read y-coord from secondary OAM (unnecessary?)
        case 1:
            if (renderEnabled()) ppu_vram_open_bus = nes.mem->ppuRead(vram_addr.addr);
            oam_data = secondary_oam[spr_i].y;
            tmp_spr_y = scanline - oam_data; 
            break;

        // Read sprite tile number
        case 2:
            oam_data = secondary_oam[spr_i].tile_i;
            tmp_spr_tile_i = oam_data;
            break;

        // Read garbage NT byte
        // Read sprite attribute
        case 3:
            if (renderEnabled()) ppu_vram_open_bus = nes.mem->ppuRead(vram_addr.addr);
            oam_data = secondary_oam[spr_i].attributes;
            if (renderEnabled()) spr_at[spr_i] = oam_data;
            // Flip vertically
            if (oam_data & 0x80) tmp_spr_y = (reg_ctrl.spr_size ? 15 : 7) - tmp_spr_y;
            if (reg_ctrl.spr_size) 
            {
                tmp_spr_pt_i = tmp_spr_tile_i & 1;
                tmp_spr_tile_i &= 0xFE;
                if (tmp_spr_y >= 8)
                {
                    tmp_spr_tile_i += 1;
                    tmp_spr_y %= 8;
                }
            }
            break;

        // Read sprite x-pos
        case 4:
            oam_data = secondary_oam[spr_i].x;
            if (renderEnabled()) spr_x_pos[spr_i] = oam_data;
            break;

        // Read sprite pattern table byte (low)
        // Read sprite x-pos
        case 5:
            if (renderEnabled()) ppu_vram_open_bus = getPTByte(reg_ctrl.spr_size ? tmp_spr_pt_i : reg_ctrl.spr_table, tmp_spr_tile_i, 0, tmp_spr_y);
            oam_data = secondary_oam[spr_i].x;
            if (renderEnabled()) spr_x_pos[spr_i] = oam_data;
            break;

        // Read sprite x-pos
        case 6:
            spr_pt_low[spr_i].load(ppu_vram_open_bus);
            oam_data = secondary_oam[spr_i].x;
            if (renderEnabled()) spr_x_pos[spr_i] = oam_data;
            break;

        // Read sprite pattern table byte (high)
        // Read sprite x-pos
        case 7:
            if (renderEnabled()) ppu_vram_open_bus = getPTByte(reg_ctrl.spr_size ? tmp_spr_pt_i : reg_ctrl.spr_table, tmp_spr_tile_i, 1, tmp_spr_y);
            oam_data = secondary_oam[spr_i].x;
            if (renderEnabled()) spr_x_pos[spr_i] = oam_data;
            break;

        // Read sprite x-pos
        case 0:
            if (renderEnabled()) spr_pt_high[spr_i].load(ppu_vram_open_bus);
            oam_data = secondary_oam[spr_i].x;
            if (renderEnabled()) spr_x_pos[spr_i] = oam_data;
            break;
    }
}

void PPU::fetchTiles()
{
    switch (cycle % 8)
    {
        // NT fetch
        case 1:
            if (bgEnabled()) ppu_vram_open_bus = getNTByte();
            break;
        case 2:
            bg_nt_latch = ppu_vram_open_bus;
            break;
        // AT fetch
        case 3:
            if (bgEnabled()) ppu_vram_open_bus = getAttribute();
            break;
        case 4:
            bg_at_latch = ppu_vram_open_bus;
            break;
        // PT fetch low
        case 5:
            if (bgEnabled()) ppu_vram_open_bus = getPTByte(reg_ctrl.bg_table, bg_nt_latch, 0, vram_addr.fine_y);
            break;
        case 6:
            bg_pt_low_latch = ppu_vram_open_bus;
            break;
        // PT fetch high
        case 7:
            if (bgEnabled()) ppu_vram_open_bus = getPTByte(reg_ctrl.bg_table, bg_nt_latch, 1, vram_addr.fine_y);
            break;
        case 0:
            if (renderEnabled())
            {
                bg_at.load(bg_at_latch);
                bg_pt_low.load(bg_pt_low_latch);
                bg_pt_high.load(ppu_vram_open_bus);
                incrHoriV();
            }
            break;
    }
}

void PPU::dummyFetchTiles()
{
    switch (cycle % 8)
    {
        // NT fetch
        case 1:
            if (bgEnabled()) ppu_vram_open_bus = getNTByte();
            bg_nt_latch = ppu_vram_open_bus;
            break;
        // AT fetch
        case 3:
            if (bgEnabled()) ppu_vram_open_bus = getAttribute();
            break;
        // PT fetch low
        case 5:
            if (bgEnabled()) ppu_vram_open_bus = getPTByte(reg_ctrl.bg_table, bg_nt_latch, 0, vram_addr.fine_y);
            break;
        // PT fetch high
        case 7:
            if (bgEnabled()) ppu_vram_open_bus = getPTByte(reg_ctrl.bg_table, bg_nt_latch, 1, vram_addr.fine_y);
            break;
        case 0:
            if (renderEnabled()) incrVertV();
            break;
    }
}

// TODO color emphasis, integer math
Pixel PPU::getColor(ubyte palette, ubyte pal_i)
{
    assert(region == Region::NTSC);
    assert(palette < 8 && pal_i < 4);
    uint system_palette_i;
    if (nes.config->HIDE_OVERSCAN && (scanline <= 8 || scanline > (post_render_start - 8)))
    {
        system_palette_i = nes.mem->ppuRead(0x3F00);
    }
    else if (pal_i == 0) system_palette_i = nes.mem->ppuRead(0x3F00);
    else system_palette_i = nes.mem->ppuRead(0x3F00 + 4*palette + pal_i);
    system_palette_i %= 0x40;
    if (reg_mask.greyscale) system_palette_i &= 0x30;
    // TODO color emphasis
    system_palette_i *= 3;

    Pixel color;
    switch (region)
    {
        case Region::NTSC:
            color = {ntsc_palette[system_palette_i],
                     ntsc_palette[system_palette_i + 1],
                     ntsc_palette[system_palette_i + 2]};
            break;
        default:
            return {0,0,0};
    }
    return color;
}

void PPU::pushPixel(Pixel p)
{
    assert(scanline != -1);
    assert(static_cast<uint>(scanline * 256 + cycle-1) < ppu_frame.size());
    ppu_frame[scanline * 256 + cycle-1] = p;
}

void PPU::renderPixel()
{
    // Background pixel
    uint bg_px = 0;
    uint bg_bit = 0;
    if (bgEnabled() && (reg_mask.left_background || cycle >= 9))
    {
        bg_bit = ((cycle-1) % 8) + fine_x_scroll;
        bg_px = (bg_pt_high.peekBit(bg_bit) << 1) | bg_pt_low.peekBit(bg_bit);
    }

    // Sprite pixel
    uint spr_px = 0;
    uint spr_i = 0;
    if (sprEnabled() && (reg_mask.left_sprites || cycle >= 9) && scanline >= 1)
    {
        for (spr_i = 0; spr_i < 8; spr_i++)
        {
            ubyte x = spr_x_pos[spr_i];
            if ((cycle-1) >= x && (cycle-1) < x+8 && (cycle-1) < 255)
            {
                uint spr_bit;
                if (spr_at[spr_i] & 0x40) // Flip horizontally
                    spr_bit = 7 - ((cycle-1) - x);
                else spr_bit = (cycle-1) - x;
                spr_px = (spr_pt_high[spr_i].peekBit(spr_bit) << 1) | spr_pt_low[spr_i].peekBit(spr_bit);
            }
            if (spr_px != 0) break;
        }
    }

    // Sprite 0 hit
    if (spr_i == 0 && spr_zero_curr && spr_px > 0 && bg_px > 0 
        && bgEnabled() && (reg_mask.left_background || cycle >= 8) 
        && sprEnabled() && (reg_mask.left_sprites || cycle >= 8))
    {
        reg_status.sprite_zero = true;
    }

    // Multiplexer
    // Sprite pixel gets drawn if non-zero && foreground priority
    if (spr_px > 0 && !(spr_at[spr_i] & 0x20))
    {
        pushPixel(getColor((spr_at[spr_i] & 0x03) + 4, spr_px));
    }
    else
    {
        pushPixel(getColor((bg_at.peekByte(bg_bit/8) & 0x03), bg_px));
    }
}

void PPU::processScanline()
{
    if (scanline == -1 && cycle == 1)
    {
        reg_status.vblank = false;
        reg_status.sprite_zero = false;
        reg_status.overflow = false;
        if (odd_frame) reset_signal = false;
    }

    if (scanline == -1 && cycle >= 280 && cycle <= 304 && renderEnabled())
    {
        // vert (v) = vert (t)
        vram_addr.addr = tmp_vram_addr.addr;
    }

    // Sprites
    if (scanline != -1 && cycle >= 1 && cycle <= 320)
    {
        if (cycle <= 64 && (cycle%2 == 0))
        {
            // Secondary OAM clear
            secondary_oam[(cycle-2)/8].data[((cycle-2)/2)%4] = 0xFF;
        }
        else if (cycle >= 65 && cycle <= 256)
        {
            if (renderEnabled()) evalSprites();
        }
        else if (cycle >= 257 && cycle <= 320)
        {
            fetchSprites();
        }
    }

    // Background
    if ((cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 340))
    {
        if ((cycle >= 1 && cycle <= 248) || (cycle >= 321 && cycle <= 336))
        {
            fetchTiles();
        }
        else if (cycle >= 249 && cycle <= 256)
        {
            dummyFetchTiles();
        }
        else if (cycle >= 321 && cycle <= 336)
        {
            fetchTiles();
        }
        else if (cycle == 338 || cycle == 340)
        {
            getNTByte();
        }
    }

    if (cycle == 257 && renderEnabled())
    {
        vram_addr.coarse_x = tmp_vram_addr.coarse_x;
        vram_addr.nametable = tmp_vram_addr.nametable;
    }
}

void PPU::tick()
{
    if (scanline >= 0 && scanline < post_render_start && cycle >= 1 && cycle <= 256)
        renderPixel();
    if (scanline < post_render_start)
        processScanline();
    else if (scanline == vblank_start && cycle == 1)
    {
        reg_status.vblank = true;
        if (reg_ctrl.vblank_nmi) nes.mem->addInterrupt(NMI);
    }
    // Skip 1 cycle every other frame
    if (scanline == -1 && cycle == 340) incrCycle();
    incrCycle();
}

void PPU::incrCycle()
{
    if (cycle == 340)
    {
        cycle = 0;
        if (scanline == render_end)
        {
            scanline = -1;
            odd_frame = !odd_frame;
        }
        else scanline++;
        spr_zero_curr = spr_zero_next;
        spr_zero_next = false;
    }
    else cycle++;
}