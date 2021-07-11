#include "mapper.h"

//TODO submappers

Mapper002::Mapper002(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;

    assert(mirroring == MirrorType::HORIZONTAL || mirroring == MirrorType::VERTICAL);
    assert(!header.trainer);
    std::cerr << "PRG-ROM size: " << header.prg_rom_size << std::endl;
    assert(header.prg_rom_size >= 0x8000 && header.prg_rom_size <= 0x400'000);
    assert((header.chr_rom_size == 0x2000) != (header.chr_ram_size == 0x2000));

    uint banks = header.prg_rom_size / 0x4000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x4000);
    }

    rom.read(reinterpret_cast<char*>(chr_mem.data()), 0x2000);
    if (header.chr_ram_size > 0) chr_ram = true;
}

std::optional<ubyte> Mapper002::cpuRead(uword address)
{
    if (address < 0x8000) return {};
    else if (address < 0xC000) // switchable low bank
    {
        return prg_rom[prg_bank][address-0x8000];
    }
    else // fixed to last bank
    {
        return prg_rom[prg_rom.size()-1][address-0xC000];
    }
}

bool Mapper002::cpuWrite(uword address, ubyte data)
{
    if (address < 0x8000) return false;
    else
    {
        prg_bank = data % prg_rom.size();
        return true;
    }
}

std::optional<ubyte> Mapper002::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else
    {
        return chr_mem[address];
    }
}

bool Mapper002::ppuWrite(uword address, ubyte data)
{
    if (address >= 0x2000) return false;
    else
    {
        if (chr_ram)
        {
            chr_mem[address] = data;
        }
        else
        {
            #ifndef NDEBUG
            std::cerr << "Warning: unsupported PPU write to " << hex(address) << std::endl;
            #endif
        }
        return true;
    }
}