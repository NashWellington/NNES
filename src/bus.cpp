#include "bus.h"

Bus bus;

byte Bus::cpuRead(uword address)
{
    auto data = mapper->cpuRead(address); // Cartridge mem
    if (data) return data.value();
    else if (address < 0x2000) // zpg, stack, RAM, or their mirrors
    {
        address %= 0x0800;
        if      (address >= 0x0200) return ram[address - 0x0200];
        else if (address >= 0x0100) return stack[address - 0x0100];
        else                        return zero_page[address];
    }
    else if (address < 0x4020) return cpuReadReg(address);
    else return dummy_cart_mem[address - 0x4020];
}

void Bus::cpuWrite(uword address, byte data)
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
    else dummy_cart_mem[address - 0x4020] = data;
}

byte Bus::ppuRead(uword address)
{
    std::optional<byte> data = mapper->ppuRead(address); // Pattern tables
    if (data) return data.value();
    else if (address < 0x2000)
    {
        std::cerr << "Error: CHR-ROM missing from cart" << std::endl;
        throw std::exception();
    }
    else if (address < 0x3F00) // Nametables + mirrors
    {
        address -= 0x2000;
        address %= 0x1000;
        if (mapper->mirroring == MirrorType::HORIZONTAL) address %= 2 * 0x0800;
        else address = (address % 0x0400) + 0x0800 * (address / 0x0800); // TODO verify this rounds down
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

void Bus::ppuWrite(uword address, byte data)
{
    if (mapper->ppuWrite(address, data)) return;
    else if (address < 0x2000)
    {
        std::cerr << "Error: CHR-ROM missing from cart" << std::endl;
        throw std::exception();
    }
    else if (address < 0x3F00) // Nametables + mirrors
    {
        address -= 0x2000;
        address %= 0x1000;
        if (mapper->mirroring == MirrorType::HORIZONTAL) address %= 2 * 0x0800;
        else address = (address % 0x0400) + 0x0800 * (address / 0x0800); // TODO verify this rounds down
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

byte Bus::cpuReadReg(uword address)
{
    // #ifndef NDEBUG
    // std::cerr << "CPU register read at " << address << std::endl;
    // #endif

    byte data = 0;
    if (address < 0x4000)
    {
        address = 0x2000 + (address - 0x2000) % 0x0008;
        switch (address)
        {
            //case 0x2000: //TODO latch stuff


            case 0x2002: // PPU Status reg
                data = (reg_ppu_status.reg & 0xF8) + (reg_ppu_data * 0x07); // TODO figure this out
                reg_ppu_status.v = 0;
                reg_ppu_addr.i = 0;
                return data;
            
            case 0x2004: // OAM Data
                data = oam[oam_addr/4][oam_addr%4];
                break;

            case 0x2007: // PPU data
                // TODO increment VRAM address
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
    /*
    else
    {
        switch (address)
        {

        }
    }*/
    std::cerr << "Warning: unsupported CPU Reg Read at " << hex(address) << std::endl;
    return data;
}

void Bus::cpuWriteReg(uword address, byte data)
{
    if (address < 0x4000)
    {
        address = 0x2000 + (address - 0x2000) % 0x0008;
        switch (address)
        {
            // TODO ignore writes for "about 30k cycles" after power/reset
            // TODO weird NMI behavior
            case 0x2000: // PPU ctrl
                reg_ppu_ctrl.reg = data;
                break;

            case 0x2001: // PPU mask
                reg_ppu_mask.reg = data;
                break;

            case 0x2003: // OAM addr
                oam_addr = data;
                break;

            // TODO no writing during vblank + more
            case 0x2004: // OAM data
                oam[oam_addr/4][oam_addr%4] = data;
                oam_addr++;
                break;

            case 0x2005: // PPU scroll
                reg_ppu_scroll.write(data);
                break;
            
            case 0x2006: // PPU addr
                reg_ppu_addr.write(data);
                break;

            case 0x2007: // PPU data
                // TODO buffer
                ppuWrite(reg_ppu_addr.address, data);
                if(reg_ppu_ctrl.i) reg_ppu_addr.address += 32;
                else reg_ppu_addr.address += 1;
                break;

            default:
                std::cerr << "Unsupported CPU Reg Write at " << address << std::endl;
                break;
        }
    }
    else 
    {
        switch (address)
        {
            case 0x4014: // OAM DMA
                // TODO transfer data at $XX00-$XXFF to PPU OAM
                dma_addr = static_cast<uword>(data) << 8;
                cpu_suspend_cycles = 514;
                break;

            default:
                std::cerr << "Unsupported CPU Reg Write at " << hex(address) << std::endl;
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
        }
        else
        {
            oam[oam_addr/4][oam_addr%4] = oam_data;
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