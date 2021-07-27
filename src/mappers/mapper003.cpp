#include "mapper.hpp"

// TODO submappers
// TODO emulate bus conflicts to fix Cybernoid

Mapper003::Mapper003(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;

    assert(!header.trainer);
    assert(mirroring == MirrorType::HORIZONTAL || mirroring == MirrorType::VERTICAL);
    assert(header.prg_rom_size == 0x4000 || header.prg_rom_size == 0x8000);
    assert((header.chr_rom_size >= 0x2000 && header.chr_rom_size <= 0x200'000)
            != (header.chr_ram_size >= 0x2000 && header.chr_ram_size <= 0x200'000));
    assert(header.prg_ram_size == 0);

    uint banks = header.prg_rom_size / 0x4000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x4000);
    }

    if (header.chr_rom_size > 0)
    {
        banks = header.chr_rom_size / 0x2000;
    }
    else
    {
        banks = header.chr_ram_size / 0x2000;
        chr_ram = true;
    }
    chr_mem.resize(banks);
    for(uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(chr_mem[i].data()), 0x2000);
    }

    #ifndef NDEBUG
    header.print(std::cerr);
    #endif
}

std::optional<ubyte> Mapper003::cpuRead(uword address)
{
    if (address < 0x4020) return {};
    else if (address >= 0x4020 && address < 0x8000)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: CPU read from unmapped address " << hex(address) << std::endl;
        #endif
        return 0;
    }
    else
    {
        uint i = 0; // bank index
        if (address >= 0xC000)
        {
            address -= 0x4000;
            i = 1;
        }
        if (prg_rom.size() == 1) i = 0;
        return prg_rom[i][address-0x8000];
    }
}

bool Mapper003::cpuWrite(uword address, ubyte data)
{
    if (address < 0x4020) return false;
    else if (address >= 0x4020 && address < 0x8000) 
    {
        #ifndef NDEBUG
        std::cerr << "Warning: CPU write to unmapped address " << hex(address) << std::endl;
        #endif
        return true;
    }
    else
    {
        chr_bank = data % chr_mem.size();
        return true;
    }
}

std::optional<ubyte> Mapper003::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else
    {
        return chr_mem[chr_bank][address];
    }
}

bool Mapper003::ppuWrite(uword address, ubyte data)
{
    if (address >= 0x2000) return false;
    else
    {
        if (chr_ram)
        {
            chr_mem[chr_bank][address] = data;
        }
        else
        {
            #ifndef NDEBUG
            std::cerr << "Warning: invalid PPU write to " << hex(address) << std::endl;
            #endif
        }
        return true;
    }
}