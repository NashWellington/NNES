#include "mapper.h"

Mapper000::Mapper000(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;

    assert(!header.trainer);
    assert((header.prg_ram_size == 0) || (header.prg_ram_size == 0x1000));
    assert((header.prg_rom_size == 0x4000) || (header.prg_rom_size == 0x8000));
    assert(header.chr_rom_size == 0x2000);

    if (header.prg_ram_size > 0) // TODO figure out Family Basic mode
    {
        prg_ram.resize(header.prg_ram_size);
        rom.read(reinterpret_cast<char*>(prg_ram.data()), prg_ram.size());
        prg_ram_exists = true;
    }

    prg_rom.resize(header.prg_rom_size);
    rom.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());

    rom.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    assert(header.prg_rom_size == prg_rom.size());
}

std::optional<byte> Mapper000::cpuRead(uword address)
{
    if (address < 0x4020) return {};
    else if (address < 0x8000) // PRG RAM
    {
        if (!prg_ram_exists) return {};
        else 
        {
            address -=0x6000;
            address %= prg_ram.size();
            return prg_ram[address];
        }
    }
    else // PRG ROm
    {
        address -= 0x8000;
        address %= prg_rom.size();
        return prg_rom[address];
    }
}

bool Mapper000::cpuWrite(uword address, byte data)
{
    return false;
}

std::optional<byte> Mapper000::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else return chr_rom[address];
}

bool Mapper000::ppuWrite(uword address, byte data)
{
    // TODO writing to nametables? probably not
    return false;
}

// TODO test
Mapper001::Mapper001(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;

    assert(!header.trainer);
    assert(header.prg_rom_size == 0x40000 || header.prg_rom_size == 0x80000);
    // Allowing size 0 chr_rom_size for test rom compatibility
    assert(header.chr_rom_size == 0 || header.chr_rom_size == 0x2000);
    assert(header.prg_ram_size == 0 || header.prg_ram_size == 0x2000);

    if (header.prg_ram_size > 0)
    {
        rom.read(reinterpret_cast<char*>(prg_ram.data()), prg_ram.size());
        prg_ram_exists = true;
    }

    prg_rom.resize(header.prg_rom_size);
    rom.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());

    chr_rom.resize(header.chr_rom_size);
    rom.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());
}

std::optional<byte> Mapper001::cpuRead(uword address)
{
    if (address < 0x6000) return {};
    else if (address < 0x8000) // PRG-RAM
    {
        if (!prg_ram_exists) return {};
        else
        {
            // TODO bank switching
        }
    }
    else //PRG-ROM
    {
        // TODO bank switching
    }
    // TODO
    return {};
}

bool Mapper001::cpuWrite(uword address, byte data)
{
    // TODO registers
    return false;
}

std::optional<byte> Mapper001::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else
    {
        // TODO bank switching
    }
    return {};
}

bool Mapper001::ppuWrite(uword address, byte data)
{
    // TODO registers
    return false;
}