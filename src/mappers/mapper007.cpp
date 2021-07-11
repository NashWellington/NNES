#include "mapper.h"

// TODO submappers
// TODO bus conflicts

Mapper007::Mapper007(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;

    assert(mirroring == MirrorType::SINGLE_SCREEN_LOWER 
        || mirroring == MirrorType::SINGLE_SCREEN_UPPER);
    assert(!header.trainer);
    assert(header.prg_ram_size == 0);
    assert(header.prg_rom_size >= 0x8'000 && header.prg_rom_size <= 0x80'000);
    assert((header.chr_rom_size == 0x2000) != (header.chr_ram_size == 0x2000));
    assert((header.chr_rom_size == 0) != (header.chr_ram_size == 0));

    uint banks = header.prg_rom_size / 0x8000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
        rom.read(reinterpret_cast<char*>(prg_rom.data()), 0x8000);

    if (header.chr_ram_size > 0) chr_ram = true;
    rom.read(reinterpret_cast<char*>(chr_mem.data()), 0x2000);
}

std::optional<ubyte> Mapper007::cpuRead(uword address)
{
    if (address < 0x8000) return {};
    else return prg_rom[prg_bank][address-0x8000];
}

bool Mapper007::cpuWrite(uword address, ubyte data)
{
    if (address < 0x8000) return false;
    else
    {
        // Bank selection
        prg_bank = (prg_rom.size() >= 16) ? data & 0x0F : data & 0x07;
        mirroring = (data & 0x10) ? MirrorType::SINGLE_SCREEN_UPPER : MirrorType::SINGLE_SCREEN_LOWER;
        return true;
    }
}

std::optional<ubyte> Mapper007::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else return chr_mem[address];
}

bool Mapper007::ppuWrite(uword address, ubyte data)
{
    if (address >= 0x2000) return false;
    else if (chr_ram) { chr_mem[address] = data; return true; }
    else return true;
}