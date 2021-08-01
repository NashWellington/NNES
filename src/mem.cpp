#include "mem.hpp"

ubyte Memory::cpuRead(uword address)
{
    std::optional<ubyte> data = nes.cart->cpuRead(address); // Cartridge mem
    if (!data)
    {
        if (address < 0x2000) // zpg, stack, RAM, or their mirrors
        {
            address %= 0x0800;
            if      (address >= 0x0200) data = ram[address - 0x0200];
            else if (address >= 0x0100) data = stack[address - 0x0100];
            else                        data = zero_page[address];
        }
        else if (address >= 0x2000 && address < 0x4018) 
        {
            data = cpuReadReg(address);
        }
        else if (address >= 0x4018 && address < 0x8000)
        {
            data = cpu_open_bus;
        }
        else
        {
            std::cerr << "Error: no PRG-ROM found" << std::endl;
            throw std::exception();
        }
    }
    cpu_open_bus = data.value();
    return data.value();
}

void Memory::cpuWrite(uword address, ubyte data)
{
    if (nes.cart->cpuWrite(address, data)) return; // Cartridge mem
    else if (address < 0x2000) // zpg, stack, RAM, or their mirrors
    {
        address %= 0x0800;
        if      (address >= 0x0200) ram[address - 0x0200] = data;
        else if (address >= 0x0100) stack[address - 0x0100] = data;
        else                        zero_page[address] = data;
    }
    else if (address >= 0x2000 && address < 0x4018) 
    {
        cpuWriteReg(address, data);
    }
    else if (address >= 0x4018 && address < 0x8000)
    {
        // Writes to CPU open bus don't affect it
    }
    else 
    {
        std::cerr << "Error: no PRG-ROM found" << std::endl;
        throw std::exception();
    }
}

ubyte Memory::ppuRead(uword address)
{
    address %= 0x4000;
    std::optional<ubyte> data = nes.cart->ppuRead(address); // Pattern tables
    if (data) return data.value();
    else if (address < 0x2000)
    {
        std::cerr << "Error: no CHR memory found" << hex(address) << std::endl;
        throw std::exception();
    }
    else if (address < 0x3F00) // Nametables + mirrors
    {
        // cart mirroring should have taken care of this
        assert(address < 0x2800);
        address -= 0x2000;
        address %= 0x1000;
        return name_tables[address / 0x0400][address % 0x0400];
    }
    else
    {
        address -= 0x3F00;
        address %= 0x0020;
        if ((address/16 > 0) && (address%4 == 0)) address -= 0x0010;
        return palette_ram[address];
    }
}

void Memory::ppuWrite(uword address, ubyte data)
{
    address %= 0x4000;
    if (nes.cart->ppuWrite(address, data)) return;
    else if (address < 0x2000)
    {
        std::cerr << "Error: no CHR memory found" << hex(address) << std::endl;
        throw std::exception();
    }
    else if (address < 0x3F00) // Nametables + mirrors
    {
        // cart mirroring should have taken care of this
        assert(address < 0x2800);
        address -= 0x2000;
        address %= 0x1000;

        name_tables[address / 0x0400][address % 0x0400] = data;
    }
    else
    {
        address -= 0x3F00;
        address %= 0x0020;
        if ((address/16 > 0) && (address%4 == 0)) address -= 0x0010;
        palette_ram[address] = data;
    }
}

ubyte Memory::cpuReadReg(uword address)
{
    if (address < 0x4000) address = 0x2000 + (address - 0x2000) % 0x0008;
    if ((address >= 0x2000 && address < 0x2008) || address == 0x4014)
    {
        return nes.ppu->read(address);
    }
    else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015)
    {
        if (address == 0x4015) return nes.apu->read(address);
        else return cpu_open_bus;
    }
    else if (address == 0x4016) // Read input port 1
    {
        // TODO expansion etc.
        ubyte data = cpu_open_bus & 0xE0;
        data |= nes.controllers[0]->read();
        cpu_open_bus = data;
        return data;
    }
    else if (address == 0x4017) // Read input port 2
    {
        // TODO DMC conflicts
        // TODO expansion etc.
        ubyte data = cpu_open_bus & 0xE0;
        data |= nes.controllers[1]->read();
        cpu_open_bus = data;
        return data;
    }
    else
    {
        #ifndef NDEBUG
        std::cerr << "Warning: unsupported CPU reg read from " << hex(address) << std::endl;
        #endif
        return 0;
    }
}

void Memory::cpuWriteReg(uword address, ubyte data)
{
    if (address < 0x4000) address = 0x2000 + (address - 0x2000) % 0x0008;
    if ((address >= 0x2000 && address < 0x2008) || address == 0x4014)
    {
        nes.ppu->write(address, data);
    }
    else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017)
    {
        nes.apu->write(address, data);
    }
    else if (address == 0x4016) // Poll input
    {
        nes.controllers[0]->poll(data & 0x01);
        nes.controllers[1]->poll(data & 0x01);
        // TODO expansion ports (if I ever get there)
    }
    else
    {
        #ifndef NDEBUG
        std::cerr << "Warning: unsupported CPU reg write to " << hex(address) << std::endl;
        #endif
    }
}

/*TODO Implement these
void Memory::save(Savestate& savestate)
{

}

void Memory::load(Savestate& savestate)
{

}
*/

// Some games rely on random RAM contents to seed RNG
// TODO option to set all to $00, $FF, or random
void Memory::start()
{
    // TODO I/O regs
    std::srand(std::time(nullptr));
    std::for_each(zero_page.begin(), zero_page.end(), [](ubyte& n){n = std::rand() % 256;});
    std::for_each(stack.begin(), stack.end(), [](ubyte& n){n = std::rand() % 256;});
    std::for_each(ram.begin(), ram.end(), [](ubyte& n){n = std::rand() % 256;});
}