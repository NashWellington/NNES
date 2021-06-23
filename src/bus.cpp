#include "bus.h"

Bus bus;

ubyte Bus::cpuRead(uword address)
{
    std::optional<ubyte> data = mapper->cpuRead(address); // Cartridge mem
    if (data) return data.value();
    else if (address < 0x2000) // zpg, stack, RAM, or their mirrors
    {
        address %= 0x0800;
        if      (address >= 0x0200) return ram[address - 0x0200];
        else if (address >= 0x0100) return stack[address - 0x0100];
        else                        return zero_page[address];
    }
    else if (address < 0x4020) return cpuReadReg(address);
    else 
    {
        #ifndef NDEBUG
        std::cerr << "Warning: read from dummy cart memory at " << hex(address) << std::endl;
        #endif
        return dummy_cart_mem[address - 0x4020];
    }
}

void Bus::cpuWrite(uword address, ubyte data)
{
    if (mapper->cpuWrite(address, data)) return; // Cartridge mem
    else if (address < 0x2000) // zpg, stack, RAM, or their mirrors
    {
        address %= 0x0800;
        if      (address >= 0x0200) ram[address - 0x0200] = data;
        else if (address >= 0x0100) stack[address - 0x0100] = data;
        else                        zero_page[address] = data;
    }
    else if (address < 0x4020) cpuWriteReg(address, data);
    else 
    {
        #ifndef NDEBUG
        std::cerr << "Warning: write to dummy cart memory at " << hex(address) << std::endl;
        #endif
        dummy_cart_mem[address - 0x4020] = data;
    }
}

ubyte Bus::ppuRead(uword address)
{
    std::optional<ubyte> data = mapper->ppuRead(address); // Pattern tables
    if (data) return data.value();
    else if (address < 0x2000)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: read from dummy CHR-ROM at " << hex(address) << std::endl;
        #endif
        return dummy_pattern_tables[address];
    }
    else if (address < 0x3F00) // Nametables + mirrors
    {
        address -= 0x2000;
        address %= 0x1000;
        if (mapper->mirroring == MirrorType::HORIZONTAL) address %= 2 * 0x0800;
        else address = (address % 0x0400) + 0x0800 * (address / 0x0800);
        return name_tables[address / 0x0400][address % 0x0400];
    }
    else
    {
        address -= 0x3F00;
        address %= 0x0020;
        if ((address/16 > 0) && (address%4 == 0)) address -= 0x0010;
        return palette_ram[address];
    }
}

void Bus::ppuWrite(uword address, ubyte data)
{
    if (mapper->ppuWrite(address, data)) return;
    else if (address < 0x2000)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: write dummy CHR-ROM at " << hex(address) << std::endl;
        #endif
        dummy_pattern_tables[address] = data;
    }
    else if (address < 0x3F00) // Nametables + mirrors
    {
        address -= 0x2000;
        address %= 0x1000;
        if (mapper->mirroring == MirrorType::HORIZONTAL) address %= 2 * 0x0800;
        else address = (address % 0x0400) + 0x0800 * (address / 0x0800);
        name_tables[address / 0x0400][address % 0x0400] = data;
    }
    else
    {
        address -= 0x3F00;
        address %= 0x0020;
        if ((address/16 > 0) && (address%4 == 0)) address -= 0x0010;
        palette_ram[address] = data;
    }
}

ubyte Bus::cpuReadReg(uword address)
{
    ubyte data = 0;
    if (address < 0x4000)
    {
        address = 0x2000 + (address - 0x2000) % 0x0008;
        switch (address)
        {
            case 0x2000: // PPU ctrl
                return ppu_latch;

            case 0x2001: // PPU mask
                return ppu_latch;

            case 0x2002: // PPU status
                // TODO race condition
                data = (reg_ppu_status.reg & 0xE0) + (ppu_latch & 0x1F);
                reg_ppu_status.v = 0;
                reg_ppu_addr.i = 0;
                ppu_latch = data;
                return data;

            case 0x2003: // OAM addr
                return ppu_latch;
            
            case 0x2004: // OAM data
                //data = primary_oam[oam_addr/4].data[oam_addr%4];
                ppu_latch = oam_data;
                return oam_data;
            
            case 0x2005: // PPU scroll
                return ppu_latch;

            case 0x2006: // PPU address
                return ppu_latch;

            case 0x2007: // PPU data
                data = reg_ppu_data;
                reg_ppu_data = ppuRead(reg_ppu_addr.address);
                if (reg_ppu_addr.address >= 0x3F00) data = reg_ppu_data;
                if(reg_ppu_ctrl.i) reg_ppu_addr.address += 32;
                else reg_ppu_addr.address += 1;
                return data;

            default:
                break;
        }
    }
    else
    {
        switch (address)
        {
            case 0x4014: // OAM DMA
                return ppu_latch;

            case 0x4016: // Input port 1
                reg_input[0].d0 = (joypad_data[0] & 0x80) >> 7;
                joypad_data[0] <<= 1;
                joypad_data[0] |= 0x01; // All bits after the first 8 read as 1
                data = reg_input[0].reg & 0x1F;
                reg_input[0].reg = 0; 
                return data;

            case 0x4017: // Input port 2
                reg_input[1].d0 = (joypad_data[1] & 0x80) >> 7;
                joypad_data[1] <<= 1;
                joypad_data[1] |= 0x01;
                data = reg_input[1].reg & 0x1F;
                reg_input[1].reg = 0;
                return data;

            default:
                #ifndef NDEBUG
                std::cerr << "Warning: unsupported CPU Reg Read at " << hex(address) << std::endl;
                #endif
                break;
        }
    }
    return data;
}

void Bus::cpuWriteReg(uword address, ubyte data)
{
    if (address < 0x4000)
    {
        address = 0x2000 + (address - 0x2000) % 0x0008;
        switch (address)
        {
            // TODO ignore writes for "about 30k cycles" after power/reset
            case 0x2000: // PPU ctrl
                if (reg_ppu_ctrl.v && reg_ppu_status.v && static_cast<byte>(data) < 0)
                {
                    addInterrupt(NMI);
                }
                reg_ppu_ctrl.reg = data;
                #ifndef NDEBUG
                if (reg_ppu_ctrl.p)
                    std::cerr << "Warning: PPUCTRL bit 6 set" << std::endl;
                #endif
                ppu_latch = data;
                break;

            case 0x2001: // PPU mask
                reg_ppu_mask.reg = data;
                ppu_latch = data;
                break;

            case 0x2002: // PPU status
                ppu_latch = data;
                break;

            case 0x2003: // OAM addr
                // TODO 2C02 OAM corruption
                oam_addr = data;
                ppu_latch = data;
                break;

            // TODO no writing during vblank + more
            case 0x2004: // OAM data
                primary_oam[oam_addr/4].data[oam_addr%4] = data;
                oam_addr++;
                oam_data = data;
                ppu_latch = data;
                break;

            case 0x2005: // PPU scroll
                reg_ppu_scroll.write(data);
                ppu_latch = data;
                break;
            
            case 0x2006: // PPU addr
                reg_ppu_addr.write(data);
                ppu_latch = data;
                break;

            case 0x2007: // PPU data
                // TODO buffer
                ppuWrite(reg_ppu_addr.address, data);
                if(reg_ppu_ctrl.i) reg_ppu_addr.address += 32;
                else reg_ppu_addr.address += 1;
                ppu_latch = data;
                break;

            default:
                break;
        }
    }
    else 
    {
        switch (address)
        {
            case 0x4014: // OAM DMA
                dma_addr = static_cast<uword>(data) << 8;
                cpu_suspend_cycles = 514;
                ppu_latch = data;
                break;

            case 0x4016: // Joypad 1 input + general output port
                if (data & 0x01)
                {
                    joypad_data[0] = joypad_data_buffer[0];
                    joypad_data[1] = joypad_data_buffer[1];
                }
                // TODO expansion ports (if I ever get there)
                ppu_latch &= 0xF8;
                ppu_latch |= (data & 0x07);
                break;

            case 0x4017: // Joypad 2 input + APU Frame Counter
                // TODO Expand on this once I implement the APU
                apu_frame_counter.reg = data & 0xC0;
                if (!apu_frame_counter.i) addInterrupt(IRQ);
                ppu_latch = apu_frame_counter.reg;
                break;

            default:
                #ifndef NDEBUG
                std::cerr << "Unsupported CPU Reg Write at " << hex(address) << std::endl;
                #endif
                break;
        }
    }
}

bool Bus::oamWrite(bool odd_cycle)
{
    assert(cpu_suspend_cycles <= 514 && cpu_suspend_cycles >= 0);
    if (cpu_suspend_cycles == 0) return false;
    if (cpu_suspend_cycles > 512)
    {
        if (cpu_suspend_cycles == 514 && !odd_cycle) cpu_suspend_cycles -=1; // One less idle cycle if even
    }
    else
    {
        if (!(cpu_suspend_cycles % 2))
        {
            oam_data = cpuRead(dma_addr);
            dma_addr++;
        }
        else
        {
            primary_oam[oam_addr/4].data[oam_addr%4] = oam_data;
            oam_addr++;
        }
    }
    cpu_suspend_cycles -= 1;
    return true;
}

/*TODO Implement these
void Bus::save(Savestate& savestate)
{

}

void Bus::load(Savestate& savestate)
{

}
*/

InterruptType Bus::getInterrupt()
{
    return current_interrupt;
}

void Bus::addInterrupt(InterruptType interrupt)
{
    current_interrupt = interrupt;
}

void Bus::clearInterrupt()
{
    current_interrupt = NO_INTERRUPT;
}

void Bus::setMapper(std::shared_ptr<Mapper> m)
{
    mapper = m;
}

void Bus::start()
{
    cpu_suspend_cycles = 0;
    joypad_data[0] = 0;
    joypad_data[1] = 0;
    reg_input[0] = {};
    reg_input[1] = {};
    // TODO APU + I/O regs
}

void Bus::reset()
{
    // TODO APU + I/O regs
}