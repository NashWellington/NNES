#include "mapper.hpp"

// TODO registers, audio, and a lot more. This mapper is nowhere near finished.
// TODO submappers
// TODO open bus behavior???

Mapper005::Mapper005(Header& header, std::ifstream& rom)
{
    assert(!header.trainer);
    // Mirroring should be set to 0 (mapper controlled)
    assert(header.mirroring == MirrorType::HORIZONTAL);
    assert(header.prg_ram_size <= 0x20'000);
    assert(header.prg_rom_size >= 0x20'000 && header.prg_rom_size <= 0x100'000);
    assert(header.chr_rom_size >= 0x20'000 && header.chr_rom_size <= 0x100'000);
    assert(header.chr_ram_size == 0);

    mirroring = MirrorType::OTHER;

    uint banks = 0;
    if (header.prg_ram_size > 0)
    {
        banks = header.prg_ram_size / 0x2000;
        prg_ram.resize(banks);
        for (uint i = 0; i < banks; i++)
        {
            rom.read(reinterpret_cast<char*>(prg_ram[i].data()), 0x2000);
        }
        prg_ram_exists = true;
    }

    banks = header.prg_rom_size / 0x2000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x2000);
    }

    banks = header.chr_rom_size / 0x0400;
    chr_mem.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(chr_mem[i].data()), 0x0400);
    }
}

std::optional<ubyte> Mapper005::cpuRead(uword address)
{
    if (address < 0x5000) return {};
    // TODO banking
}

bool Mapper005::cpuWrite(uword address, ubyte data)
{
    if (address < 0x2000) return false;
    
    // Overlay of NES internal regs
    if (address >= 0x2000 && address < 0x5000)
    {
        // Mirror PPU regs
        if (address >= 0x2000 && address < 0x4000)
            address = 0x2000 + (address - 0x2000) % 0x0008;
            
        switch (address)
        {
            // TODO overlay regs
            default:
                #ifndef NDEBUG
                std::cerr << "Warning: unsupported reg write to " << hex(address) << std::endl;
                #endif
                break;
        }
        return false;
    }
    // MMC5 regs
    else if (address >= 0x5000 && address < 0x6000)
    {
        switch (address)
        {
            // TODO audio & bank select regs (& possibly more)
            default:
                #ifndef NDEBUG
                std::cerr << "Warning: unsupported reg write to " << hex(address) << std::endl;
                #endif
                break;
        }
    }
    // PRG-RAM
    else if (address >= 0x6000 && address < 0x8000)
    {
        // TODO bank switching
    }
    else
    {
        // TODO bank switching
    }
    return true;
}

std::optional<ubyte> Mapper005::ppuRead(uword address)
{
    // TODO
}

bool Mapper005::ppuWrite(uword address, ubyte data)
{
    // TODO
}