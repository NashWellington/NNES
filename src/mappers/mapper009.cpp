#include "mapper.h"

Mapper009::Mapper009(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;

    assert(!header.trainer);
    assert(mirroring == MirrorType::HORIZONTAL || mirroring == MirrorType::VERTICAL);
    assert(header.prg_ram_size == 0 || header.prg_ram_size == 0x2000);
    assert(header.prg_rom_size >= 0x8'000 && header.prg_rom_size <= 0x20'000);
    assert((header.chr_rom_size > 0) != (header.chr_ram_size > 0));
    assert((header.chr_rom_size <= 0x20'000) && (header.chr_ram_size <= 0x20'000));

    if (header.prg_ram_size > 0)
    {
        prg_ram_exists = true;
    }

    uint banks = header.prg_rom_size / 0x2000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x2000);
    }

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

std::optional<ubyte> Mapper009::cpuRead(uword address)
{
    if (address < 0x4020) return {};
    else if (address >= 0x4020 && address < 0x6000)
    {   
        #ifndef NDEBUG
        std::cerr << "Warning: CPU read from unmapped address " << hex(address) << std::endl;
        #endif
        return 0;
    }
    else if (address < 0x8000)  // PRG-RAM
    {
        if (!prg_ram_exists) 
        {
            #ifndef NDEBUG
            std::cerr << "Warning: CPU read from unmapped address " << hex(address) << std::endl;
            #endif
            return 0;
        }
        else
        {
            return prg_ram[address-0x6000];
        }
    }
    else if (address < 0xA000)  // PRG-ROM first bank
    {
        return prg_rom[prg_bank][address-0x8000];
    }
    else                        // PRG-ROM last 3 banks
    {
        address -= 0x8000;
        uint bank = address / 0x2000;
        bank = 4 - bank; // Index from last
        assert(bank > 0);
        address %= 0x2000;
        return prg_rom[prg_rom.size() - bank][address];
    }
}

bool Mapper009::cpuWrite(uword address, ubyte data)
{
    if (address < 0x4020) return false;
    else if (address >= 0x4020 && address < 0x6000)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: CPU write to unmapped address " << hex(address) << std::endl;
        #endif
        return true;
    }
    else if (address >= 0x6000 && address < 0x8000) // PRG-RAM
    {
        if (!prg_ram_exists) 
        {
            #ifndef NDEBUG
            std::cerr << "Warning: CPU write to unmapped address " << hex(address) << std::endl;
            #endif
            return true;
        }
        else prg_ram[address-0x6000] = data;
    }
    else if (address >= 0x8000 && address < 0xA000)
    {
        // PRG-ROM, no registers
    }
    else if (address >= 0xA000 && address < 0xB000)
    {
        // PRG-ROM bank select
        prg_bank = data & 0x0F;
    }
    else if (address >= 0xB000 && address < 0xC000)
    {
        // CHR bank select $FD/$0000
        chr_bank[0][0] = data & 0x1F;
    }
    else if (address >= 0xC000 && address < 0xD000)
    {
        // CHR bank select $FE/$0000
        chr_bank[0][1] = data & 0x1F;
    }
    else if (address >= 0xD000 && address < 0xE000)
    {
        // CHR bank select $FD/$1000
        chr_bank[1][0] = data & 0x1F;
    }
    else if (address >= 0xE000 && address < 0xF000)
    {
        // CHR bank select $FE/$1000
        chr_bank[1][1] = data & 0x1F;
    }
    else // address between $F000 and $FFFF
    {
        // Mirroring
        mirroring = (data & 0x01) ? MirrorType::HORIZONTAL : MirrorType::VERTICAL;
    }
    return true;
}

std::optional<ubyte> Mapper009::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else
    {
        uint chr_table = address / 0x1000;
        uint chr_i = chr_bank[chr_table][(chr_latch[chr_table] == 0xFD) ? 0 : 1];
        ubyte data = chr_mem[chr_i % chr_mem.size()][address % 0x1000];

        if (address == 0x0FD8) 
            chr_latch[0] = 0xFD;
        else if (address == 0x0FE8) 
            chr_latch[0] = 0xFE;
        else if (address >= 0x1FD8 && address <= 0x1FDF)
            chr_latch[1] = 0xFD;
        else if (address >= 0x1FE8 && address <= 0x1FEF)
            chr_latch[1] = 0xFE;

        return data;
    }
}

bool Mapper009::ppuWrite(uword address, ubyte data)
{
    if (address >= 0x2000) return false;
    else
    {
        if (chr_ram)
        {
            uint chr_table = address / 0x1000;
            uint chr_i = chr_bank[chr_table][(chr_latch[chr_table] == 0xFD) ? 0 : 1];
            chr_mem[chr_i % chr_mem.size()][address % 0x1000] = data;
        }
        return true;
    }
}