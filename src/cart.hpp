#pragma once

#include "globals.hpp"
#include "peripheral.hpp"
#include "boot.hpp"
#include "mappers/mapper.hpp"

class Cartridge : public Peripheral
{
public:
    Cartridge(Header& header, std::ifstream& rom);
    std::optional<byte> cpuRead(uword address) { return mapper->cpuRead(address); }
    bool cpuWrite(uword address, ubyte data) { return mapper->cpuWrite(address, data); }

    std::optional<byte> ppuRead(uword& address) { mapper->mirrorNametables(address); return mapper->ppuRead(address); }
    bool ppuWrite(uword& address, ubyte data)   { mapper->mirrorNametables(address); return mapper->ppuWrite(address, data); }

    void processInputs() { return; }

    void reset() { mapper->reset(); }

private:
    /* Not going to implement these until I get further along in savestate development
    std::vector<ubyte> misc_rom = {};
    std::vector<ubyte> prg_ram = {};
    std::vector<ubyte> prg_rom = {};
    std::vector<ubyte> chr_rom = {};
    std::vector<ubyte> chr_ram = {}; // Probably don't need both of these but we'll see
    std::vector<ubyte> registers = {};
    */

    std::shared_ptr<Mapper> mapper;
};