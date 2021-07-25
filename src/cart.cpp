#include "cart.hpp"

Cartridge::Cartridge(NES& nes, Header& header, std::ifstream& rom)
{
    mapper = Boot::getMapper(nes, header, rom);
    
    // Check that the whole file has been read
    #ifndef NDEBUG
    uint64_t size = 0;
    while (!rom.eof())
    {
        rom.get();
        size++;
    }
    if (size > 0)
    {
        if (header.misc_rom_num > 0)
        {
            std::cerr << "Error: misc ROM with " << size << " bytes not read" << std::endl;
            throw std::exception();
        }
        else
        {
            std::cerr << "Warning: " << size << " extra bytes in the ROM file" << std::endl;
        }
    }
    #endif
    rom.close();
}

std::optional<byte> Cartridge::cpuRead(uword address) { return mapper->cpuRead(address); }
bool Cartridge::cpuWrite(uword address, ubyte data) { return mapper->cpuWrite(address, data); }

std::optional<byte> Cartridge::ppuRead(uword& address) { mapper->mirrorNametables(address); return mapper->ppuRead(address); }
bool Cartridge::ppuWrite(uword& address, ubyte data)   { mapper->mirrorNametables(address); return mapper->ppuWrite(address, data); }

void Cartridge::processInputs() { return; }

void Cartridge::reset() { mapper->reset(); }