#include "mapper.h"

// TODO submappers
// TODO bus conflicts/open bus behavior

Mapper007::Mapper007(Header& header, std::ifstream& rom)
{
    assert(!header.trainer);
    assert(header.prg_ram_size == 0);
    assert(header.prg_rom_size >= 0x4'000 && header.prg_rom_size <= 0x80'000);
    assert((header.chr_rom_size > 0) != (header.chr_ram_size > 0));

    mirroring = MirrorType::SINGLE_SCREEN_LOWER;

    // As far as I know, oam_test_3 is the only ROM with PRG-ROM size 16K
    if (header.prg_rom_size < 0x8000)
    {
        prg_rom.resize(1);
        prg_rom[0].resize(header.prg_rom_size);
        rom.read(reinterpret_cast<char*>(prg_rom[0].data()), header.prg_rom_size);
    }
    else
    {
        uint banks = header.prg_rom_size / 0x8000;
        prg_rom.resize(banks);
        for (uint i = 0; i < banks; i++)
        {
            prg_rom[i].resize(0x8000);
            rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x8000);
        }
    }

    uint size = header.chr_rom_size;
    if (header.chr_ram_size > 0) 
    {
        size = header.chr_ram_size;
        chr_ram = true;
    }
    chr_mem.resize(size);
    rom.read(reinterpret_cast<char*>(chr_mem.data()), size);
}

std::optional<ubyte> Mapper007::cpuRead(uword address)
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
        return prg_rom[prg_bank][(address-0x8000) % (prg_rom[prg_bank].size())];
    }
}

bool Mapper007::cpuWrite(uword address, ubyte data)
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
        // Bank selection
        prg_bank = (prg_rom.size() >= 16) ? data & 0x0F : data & 0x07;
        if (prg_rom.size() < 16) prg_bank %= prg_rom.size();
        mirroring = (data & 0x10) ? MirrorType::SINGLE_SCREEN_UPPER : MirrorType::SINGLE_SCREEN_LOWER;
        return true;
    }
}

std::optional<ubyte> Mapper007::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else return chr_mem[address % chr_mem.size()];
}

bool Mapper007::ppuWrite(uword address, ubyte data)
{
    if (address >= 0x2000) return false;
    else if (chr_ram) { chr_mem[address % chr_mem.size()] = data; return true; }
    else 
    {
        #ifndef NDEBUG
        std::cerr << "Warning: attempted write to CHR-ROM at " << hex(address) << std::endl;
        #endif
        return true;
    }
}