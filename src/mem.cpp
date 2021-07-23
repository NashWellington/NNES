#include "mem.hpp"

// TODO emulate open bus behavior

ubyte Memory::cpuRead(uword address)
{
    std::optional<ubyte> data = nes.cart->cpuRead(address); // Cartridge mem
    if (data) return data.value();
    else if (address < 0x2000) // zpg, stack, RAM, or their mirrors
    {
        address %= 0x0800;
        if      (address >= 0x0200) return ram[address - 0x0200];
        else if (address >= 0x0100) return stack[address - 0x0100];
        else                        return zero_page[address];
    }
    else if (address < 0x4020) return cpuReadReg(address);
    else
    {
        #ifndef NDEBUG
        std::cerr << "Warning: read from dummy cart memory at " << hex(address) << std::endl;
        #endif
        if (address >= 0x4020 && address < 0x6000)
            return dummy_misc_ram[address-0x4020];
        else if (address >= 0x6000 && address < 0x8000)
            return dummy_prg_ram[address-0x6000];
        #ifndef NDEBUG
        else
        {
            std::cerr << "Error: no PRG-ROM found" << std::endl;
            throw std::exception();
        }
        #endif
    }
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
    else if (address < 0x4020) cpuWriteReg(address, data);
    else 
    {
        #ifndef NDEBUG
        std::cerr << "Warning: write to dummy cart memory at " << hex(address) << std::endl;
        #endif
        if (address >= 0x4020 && address < 0x6000)
            dummy_misc_ram[address-0x4020] = data;
        else if (address >= 0x6000 && address < 0x8000)
            dummy_prg_ram[address-0x6000] = data;
        #ifndef NDEBUG
        else
        {
            std::cerr << "Error: no PRG-ROM found" << std::endl;
            throw std::exception();
        }
        #endif
    }
}

ubyte Memory::ppuRead(uword address)
{
    std::optional<ubyte> data = nes.cart->ppuRead(address); // Pattern tables
    if (data) return data.value();
    else if (address < 0x2000)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: read from dummy CHR-ROM at " << hex(address) << std::endl;
        #endif
        return dummy_pattern_tables[address];
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
    if (nes.cart->ppuWrite(address, data)) return;
    else if (address < 0x2000)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: write to dummy CHR-ROM at " << hex(address) << std::endl;
        #endif
        dummy_pattern_tables[address] = data;
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
    else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017)
    {
        // TODO is this correct?
        if (address == 0x4015) return nes.apu->read(address);
        else return cpu_open_bus;
    }
    else if (address == 0x4016) // Read input port 1
    {
        ubyte data = 0;
        data |= nes.controllers[0]->read();
        // TODO expansion etc.
        return data;
    }
    else if (address == 0x4017) // Read input port 2
    {
        ubyte data = 0;
        // TODO DMC conflicts?
        data |= nes.controllers[1]->read();
        // TODO expansion etc.
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
        cpu_open_bus &= 0xF8;
        cpu_open_bus |= (data & 0x07);
    }
    else
    {
        #ifndef NDEBUG
        std::cerr << "Warning: unsupported CPU reg write to " << hex(address) << std::endl;
        #endif
    }
}

bool Memory::oamWrite(bool odd_cycle)
{
    assert(cpu_suspend_cycles <= 514 && cpu_suspend_cycles >= 0);
    if (cpu_suspend_cycles == 0) return false;
    if (cpu_suspend_cycles > 512)
    {
        if (cpu_suspend_cycles == 514 && !odd_cycle) cpu_suspend_cycles -=1; // One less idle cycle if even
    }
    else
    {
        if (!(cpu_suspend_cycles % 2))
        {
            nes.ppu->oam_data = cpuRead(nes.ppu->dma_addr);
            nes.ppu->dma_addr++;
        }
        else
        {
            nes.ppu->primary_oam[nes.ppu->oam_addr/4].data[nes.ppu->oam_addr%4] = nes.ppu->oam_data;
            nes.ppu->oam_addr++;
        }
    }
    cpu_suspend_cycles -= 1;
    return true;
}

/*TODO Implement these
void Memory::save(Savestate& savestate)
{

}

void Memory::load(Savestate& savestate)
{

}
*/

InterruptType Memory::getInterrupt()
{
    // FIXME adding APU interrupts caused major CPU test result regressions
    if (current_interrupt == InterruptType::NO_INTERRUPT 
        && (nes.apu->frame_interrupt || nes.apu->dmc.interrupt))
        addInterrupt(IRQ);
    return current_interrupt;
}

// TODO NMI/Reset priority?
void Memory::addInterrupt(InterruptType interrupt)
{
    current_interrupt = interrupt;
}

void Memory::clearInterrupt()
{
    current_interrupt = NO_INTERRUPT;
}

// Some games rely on random RAM contents to seed RNG
// TODO option to set all to $00, $FF, or random
void Memory::start()
{
    cpu_suspend_cycles = 0;
    // TODO I/O regs
    std::srand(std::time(nullptr));
    std::for_each(zero_page.begin(), zero_page.end(), [](ubyte& n){n = std::rand() % 256;});
    std::for_each(stack.begin(), stack.end(), [](ubyte& n){n = std::rand() % 256;});
    std::for_each(ram.begin(), ram.end(), [](ubyte& n){n = std::rand() % 256;});
}