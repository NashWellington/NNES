#include "mem.h"

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
    ubyte data = 0;
    if (address < 0x4000) address = 0x2000 + (address - 0x2000) % 0x0008;
    switch (address)
    {
// PPU regs
        case 0x2000: // PPU ctrl
            return ppu_latch;

        case 0x2001: // PPU mask
            return ppu_latch;

        case 0x2002: // PPU status
            // TODO race condition
            data = (reg_ppu_status.reg & 0xE0) + (ppu_latch & 0x1F);
            reg_ppu_status.v = 0;
            reg_ppu_addr.i = 0;
            ppu_latch = data;
            return data;

        case 0x2003: // OAM addr
            return ppu_latch;
        
        case 0x2004: // OAM data
            //data = primary_oam[oam_addr/4].data[oam_addr%4];
            ppu_latch = oam_data;
            return oam_data;
        
        case 0x2005: // PPU scroll
            return ppu_latch;

        case 0x2006: // PPU address
            return ppu_latch;

        case 0x2007: // PPU data
            data = reg_ppu_data;
            reg_ppu_data = ppuRead(reg_ppu_addr.address);
            if (reg_ppu_addr.address >= 0x3F00) data = reg_ppu_data;
            if(reg_ppu_ctrl.i) reg_ppu_addr.address += 32;
            else reg_ppu_addr.address += 1;
            return data;

// APU regs

// Another PPU reg
        case 0x4014: // OAM DMA
            return ppu_latch;

// Another APU reg
        case 0x4015: // APU status
            nes.apu->getStatus();
            data = nes.apu->reg_apu_status.reg;
            return data;

// Input/misc regs
        case 0x4016: // Input port 1
            data |= nes.controllers[0]->read();
            // TODO expansion etc.
            return data;

        case 0x4017: // Input port 2
            data |= nes.controllers[1]->read();
            // TODO expansion etc.
            return data;

        default:
            #ifndef NDEBUG
            std::cerr << "Warning: unsupported CPU Reg Read at " << hex(address) << std::endl;
            #endif
            break;
    }
    return data;
}

void Memory::cpuWriteReg(uword address, ubyte data)
{
    if (address < 0x4000) address = 0x2000 + (address - 0x2000) % 0x0008;
    switch (address)
    {
// PPU regs
        // TODO ignore writes for "about 30k cycles" after power/reset
        case 0x2000: // PPU ctrl
            if (reg_ppu_ctrl.v && reg_ppu_status.v && static_cast<byte>(data) < 0)
            {
                addInterrupt(NMI);
            }
            reg_ppu_ctrl.reg = data;
            ppu_latch = data;
            break;

        case 0x2001: // PPU mask
            reg_ppu_mask.reg = data;
            ppu_latch = data;
            break;

        case 0x2002: // PPU status
            ppu_latch = data;
            break;

        case 0x2003: // OAM addr
            // TODO 2C02 OAM corruption
            oam_addr = data;
            ppu_latch = data;
            break;

        // TODO no writing during vblank + more
        case 0x2004: // OAM data
            primary_oam[oam_addr/4].data[oam_addr%4] = data;
            oam_addr++;
            oam_data = data;
            ppu_latch = data;
            break;

        case 0x2005: // PPU scroll
            reg_ppu_scroll.write(data);
            ppu_latch = data;
            break;
        
        case 0x2006: // PPU addr
            reg_ppu_addr.write(data);
            ppu_latch = data;
            break;

        case 0x2007: // PPU data
            // TODO buffer
            ppuWrite(reg_ppu_addr.address, data);
            if(reg_ppu_ctrl.i) reg_ppu_addr.address += 32;
            else reg_ppu_addr.address += 1;
            ppu_latch = data;
            break;
// APU regs
        case 0x4000: // Pulse 1 control
            nes.apu->reg_pulse_ctrl[0].reg = data;
            break;
        
        case 0x4001: // Pulse 1 sweep control
            nes.apu->reg_sweep[0].reg = data;
            break;

        case 0x4002: // Pulse 1 timer (low 8 bits)
            nes.apu->pulse_timer[0] &= 0xFF00;
            nes.apu->pulse_timer[0] |= static_cast<uword>(data);
            break;
        
        case 0x4003: // Pulse 1 length counter load + timer (high 3 bits)
            nes.apu->pulse_length_ctr_load[0] = data >> 3;
            nes.apu->pulse_timer[0] &= 0x00FF;
            nes.apu->pulse_timer[0] |= (static_cast<uword>(data & 0x07) << 8);
            nes.apu->envelope[0].start = true;
            nes.apu->pulse[0].len.load(nes.apu->pulse_length_ctr_load[0]);
            break;
        
        case 0x4004: // Pulse 2 control
            nes.apu->reg_pulse_ctrl[1].reg = data;
            break;
        
        case 0x4005: // Pulse 2 sweep control
            nes.apu->reg_sweep[1].reg = data;
            break;

        case 0x4006: // Pulse 2 timer (low 8 bits)
            nes.apu->pulse_timer[1] &= 0xFF00;
            nes.apu->pulse_timer[1] |= static_cast<uword>(data);
            break;
        
        case 0x4007: // Pulse 2 length counter load + timer (high 3 bits)
            nes.apu->pulse_length_ctr_load[1] = data >> 3;
            nes.apu->pulse_timer[1] &= 0x00FF;
            nes.apu->pulse_timer[1] |= (static_cast<uword>(data & 0x07) << 8);
            nes.apu->envelope[1].start = true;
            nes.apu->pulse[1].len.load(nes.apu->pulse_length_ctr_load[1]);
            break;

        case 0x4008: // Triangle linear counter
            nes.apu->reg_lin_ctr.reg = data;
            break;

        case 0x4009: // Triangle timer (low 8 bits)
            nes.apu->tri_timer &= 0xFF00;
            nes.apu->tri_timer |= static_cast<uword>(data);
            break;

        case 0x4010: // Triangle length counter load + timer (high 3 bits)
            nes.apu->tri_length_ctr_load = data >> 3;
            nes.apu->tri_timer &= 0x00FF;
            nes.apu->tri_timer |= (static_cast<uword>(data & 0x07) << 8);
            nes.apu->triangle.len.load(nes.apu->tri_length_ctr_load);
            nes.apu->triangle.lin.reload = true;
            break;

// Another PPU reg
        case 0x4014: // OAM DMA
            dma_addr = static_cast<uword>(data) << 8;
            cpu_suspend_cycles = 514;
            ppu_latch = data;
            break;

// Another APU reg
        case 0x4015: // APU ctrl
            nes.apu->reg_apu_ctrl.reg = data;
            if (nes.apu->reg_apu_ctrl.lce_p1 == 0) nes.apu->pulse[0].len.clear();
            if (nes.apu->reg_apu_ctrl.lce_p2 == 0) nes.apu->pulse[1].len.clear();
            if (nes.apu->reg_apu_ctrl.lce_tr == 0) nes.apu->triangle.len.clear();
            //if (nes.apu->reg_apu_ctrl.lce_ns == 0) nes.apu->noise.len.clear();
            break;

// Misc regs
        case 0x4016: // Poll input
            nes.controllers[0]->poll(data & 0x01);
            nes.controllers[1]->poll(data & 0x01);
            // TODO expansion ports (if I ever get there)
            ppu_latch &= 0xF8;
            ppu_latch |= (data & 0x07);
            break;

        case 0x4017: // APU frame counter
            // TODO buffer for 3-4 CPU cycles
            nes.apu->reg_frame_ctr.reg = data & 0xC0;
            // TODO interrupts?
            ppu_latch = nes.apu->reg_frame_ctr.reg;
            break;

        default:
            #ifndef NDEBUG
            std::cerr << "Warning: unsupported CPU Reg Write at " << hex(address) << std::endl;
            #endif
            break;
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
            oam_data = cpuRead(dma_addr);
            dma_addr++;
        }
        else
        {
            primary_oam[oam_addr/4].data[oam_addr%4] = oam_data;
            oam_addr++;
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
    // TODO fully and correctly implement audio & uncomment this out
    //if (current_interrupt == InterruptType::NO_INTERRUPT && nes.apu->frame_interrupt)
    //    addInterrupt(IRQ);
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