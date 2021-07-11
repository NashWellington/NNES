#include "mapper.h"

// Needed for generating IRQs
#include "../bus.h"

// TODO submappers

Mapper004::Mapper004(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;

    assert(mirroring == MirrorType::HORIZONTAL 
        || mirroring == MirrorType::VERTICAL
        || mirroring == MirrorType::FOUR_SCREEN);
    assert(!header.trainer);
    assert(header.prg_ram_size == 0 || header.prg_ram_size == 0x2000);
    assert(header.prg_rom_size >= 0x8000 && header.prg_rom_size <= 0x200'000);
    assert((header.chr_rom_size > 0) != (header.chr_ram_size > 0));

    if (header.prg_ram_size > 0)
    {
        rom.read(reinterpret_cast<char*>(prg_ram.data()), 0x2000);
        prg_ram_exists = true;
    }

    uint banks = header.prg_rom_size / 0x2000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x2000);
    }
    prg_bank[3] = prg_rom.size() - 1; // Fix last bank

    if (header.chr_rom_size > 0)
    {
        banks = header.chr_rom_size / 0x1000;
    }
    else
    {
        banks = header.chr_ram_size / 0x1000;
        chr_ram = true;
    }
    chr_mem.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(chr_mem[i].data()), 0x1000);
    }
}

std::optional<ubyte> Mapper004::cpuRead(uword address)
{
    if (address < 0x6000) return {};
    else if (address < 0x8000) // PRG RAM
    {
        if (prg_ram_exists)
        {
            if (prg_ram_enabled)
            {
                return prg_ram[address-0x6000];
            }
            else
            {
                // TODO emulate open bus behavior
                return 0;
            }
        }
        else
        {
            return {};
        }
    }
    else
    {
        address -= 0x8000;
        uint bank = address / 0x2000;
        address %= 0x2000;
        return prg_rom[prg_bank[bank]][address];
    }
}

bool Mapper004::cpuWrite(uword address, ubyte data)
{
    if (address < 0x6000) return false;
    else if (address < 0x8000) // PRG RAM
    {
        if (prg_ram_exists)
        {
            if (prg_ram_enabled && !prg_ram_write_protect)
            {
                prg_ram[address-0x6000] = data;
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (address >= 0x8000 && address < 0xA000)
    {
        if (address % 2 == 0)   // bank select
        {
            reg_bank_select.reg = data;
        }
        else                    // bank data
        {
            uint bank = 0;
            switch (reg_bank_select.bank)
            {
                case 0:
                    bank = (reg_bank_select.chr_bank_mode) ? 4 : 0;
                    data &= 0xFE;
                    chr_bank[bank] = data;
                    chr_bank[bank+1] = data+1;
                    break;
                case 1:
                    bank = (reg_bank_select.chr_bank_mode) ? 6 : 2;
                    data &= 0xFE;
                    chr_bank[bank] = data;
                    chr_bank[bank+1] = data+1;
                    break;
                case 2:
                    bank = (reg_bank_select.chr_bank_mode) ? 0 : 4;
                    chr_bank[bank] = data;
                    break;
                case 3:
                    bank = (reg_bank_select.chr_bank_mode) ? 1 : 5;
                    chr_bank[bank] = data;
                    break;
                case 4:
                    bank = (reg_bank_select.chr_bank_mode) ? 2 : 6;
                    chr_bank[bank] = data;
                    break;
                case 5:
                    bank = (reg_bank_select.chr_bank_mode) ? 3 : 7;
                    chr_bank[bank] = data;
                    break;
                case 6:
                    prg_bank[3] = prg_rom.size()-1; // Probably unnecessary
                    if (prg_rom.size() < 0x40) data %= 0x40;
                    if (!reg_bank_select.prg_bank_mode)
                    {
                        prg_bank[0] = data;
                        prg_bank[2] = prg_rom.size()-2;
                    }
                    else
                    {
                        prg_bank[0] = prg_rom.size()-2;
                        prg_bank[2] = data;
                    }
                    break;
                case 7:
                    prg_bank[3] = prg_rom.size()-1; // Probably unnecessary
                    if (prg_rom.size() < 0x40) data %= 0x40;
                    prg_bank[1] = data;
                    break;
                default:
                    break;
            }
        }
    }
    else if (address >= 0xA000 && address < 0xC000)
    {
        if (address % 2 == 0)   // Mirroring
        {
            if (mirroring != MirrorType::FOUR_SCREEN)
            {
                if (data & 0x01)
                {
                    mirroring = MirrorType::HORIZONTAL;
                }
                else
                {
                    mirroring = MirrorType::VERTICAL;
                }
            }
        }
        else                    // PRG-RAM protect
        {
            // TODO MMC6 behavior
            prg_ram_write_protect = data & 0x40;
            prg_ram_enabled = data & 0x80;
        }
    }
    else if (address >= 0xC000 && address < 0xE000)
    {
        if (address % 2 == 0)   // IRQ latch
        {
            irq_latch = data;
        }
        else                    // IRQ reload
        {
            irq_reload = true;
            irq_counter = 0;
        }
    }
    else if (address >= 0xE000)
    {
        if (address % 2 == 0)   // IRQ disable
        {
            irq_enable = false;
            // TODO acknowledge pending interrupts?
        }
        else                    // IRQ enable
        {
            irq_enable = true;
        }
    }
    return true;
}

std::optional<ubyte> Mapper004::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else
    {
        if (address < 0x1000)
        {
            a12 = 0; 
        }
        else
        {
            if (a12 == 0) // a12 0 -> 1: counter clocked
            {
                if (irq_counter == 0 || irq_reload)
                {
                    irq_counter = irq_latch;
                }
                else
                {
                    irq_counter--;
                }
                // TODO differentiate between MMC3 rev A or rev B
                if (irq_counter == 0 && irq_enable)
                {
                    bus.addInterrupt(InterruptType::IRQ);
                }
            }
            a12 = 1;
        }
        return chr_mem[chr_bank[address / 0x1000]][address % 0x1000];
    }
}

bool Mapper004::ppuWrite(uword address, ubyte data)
{
    if (address >= 0x2000) return false;
    else
    {
        if (chr_ram)
        {
            chr_mem[chr_bank[address / 0x1000]][address % 0x1000] = data;
        }
        return true;
    }
}