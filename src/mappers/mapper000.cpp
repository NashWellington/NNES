#include "mapper.h"

Mapper000::Mapper000(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;
    assert(mirroring == MirrorType::HORIZONTAL || mirroring == MirrorType::VERTICAL);
    assert(!header.trainer);
    assert(header.prg_ram_size <= 0x2000);
    assert(header.prg_rom_size == 0x4000 || header.prg_rom_size == 0x8000);
    assert((header.chr_rom_size > 0) != (header.chr_ram_size > 0));

    if (header.prg_ram_size == 0 && header.type == HeaderType::INES)
    {
        header.prg_ram_size = 0x2000; // Mostly for compatibility w/ test ROMs
    }

    if (header.prg_ram_size > 0)
    {
        prg_ram.resize(header.prg_ram_size);
        prg_ram_enabled = true;
    }

    prg_rom.resize(header.prg_rom_size);
    rom.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());

    uint64_t chr_size = 0;
    if (header.chr_ram_size > 0) 
    {
        chr_size = header.chr_ram_size;
        chr_ram = true;
    }
    else
    {
        chr_size = header.chr_rom_size;
    }
    rom.read(reinterpret_cast<char*>(chr_mem.data()), chr_size);
}

std::optional<ubyte> Mapper000::cpuRead(uword address)
{
    if (address < 0x6000) return {};
    else if (address < 0x8000) // PRG RAM
    {
        if (!prg_ram_enabled) return {};
        else 
        {
            address -=0x6000;
            address %= prg_ram.size();
            return prg_ram[address];
        }
    }
    else // PRG ROM
    {
        address -= 0x8000;
        address %= prg_rom.size();
        return prg_rom[address];
    }
}

bool Mapper000::cpuWrite(uword address, ubyte data)
{
    if (address < 0x6000) return false;
    else if (address < 0x8000)
    {
        if (!prg_ram_enabled) return false;
        else
        {
            address -= 0x6000;
            address %= prg_ram.size();
            prg_ram[address] = data;
            return true;
        }
    }
    else
    {
        #ifndef NDEBUG
        std::cerr << "Warning: unsupported CPU write to " << hex(address) << std::endl;
        #endif
        return true;
    }
}

std::optional<ubyte> Mapper000::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else return chr_mem[address];
}

bool Mapper000::ppuWrite(uword address, ubyte data)
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