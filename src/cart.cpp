#include "cart.h"

Cartridge::Cartridge(Header& header, std::ifstream& rom)
{
    /* TODO uncomment this once I fully flesh out cart
    // TODO One day I'll have to figure out what a trainer is
    assert(!header.trainer);
    prg_ram.resize(header.prg_ram_size);

    prg_rom.resize(header.prg_rom_size);
    rom.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());

    assert((header.chr_rom_size > 0) != (header.chr_ram_size > 0));

    chr_rom.resize(header.chr_rom_size);
    if (chr_rom.size() > 0)
        rom.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    chr_ram.resize(header.chr_ram_size);
    
    assert(sizeof(char) == 1);

    if (rom.eof()) misc_rom.resize(0); // Not sure if necessary

    while (!rom.eof())
    {
        ubyte buf;
        rom.read(reinterpret_cast<char*>(&buf), 1);
        misc_rom.push_back(buf);
    }
    */

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
        std::cerr << "Error: " << size << " bytes not read" << std::endl;
        throw std::exception();
    }
    #endif
    rom.close();
}