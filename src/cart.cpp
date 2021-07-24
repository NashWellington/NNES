#include "cart.hpp"

Cartridge::Cartridge(Header& header, std::ifstream& rom)
{
    mapper = Boot::getMapper(header, rom);
    
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
            std::cerr << "Error: " << size << " bytes not read" << std::endl;
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