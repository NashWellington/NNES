#include "mapper.hpp"

// TODO support for SZROM
// TODO support submapper 5
// TODO battery-backed save

Mapper001::Mapper001(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;
    submapper = header.submapper;

    assert(!header.trainer);
    assert(header.prg_rom_size >= 0x8000 && header.prg_rom_size <= 0x80000);
    assert(header.prg_ram_size <= 0x8000 && header.prg_nv_ram_size <= 0x8000);
    assert((header.chr_rom_size > 0) != (header.chr_ram_size > 0));

    if (header.prg_nv_ram_size == 0 && header.prg_ram_size == 0 && header.type == HeaderType::INES)
    {
        header.prg_ram_size = 0x8000;
    }

    uint banks = 0;
    if (header.prg_ram_size > 0)
    {
        banks = header.prg_ram_size / 0x2000;
        // TODO change default depending on MMC1A,B,C
        prg_ram_enabled = true;
    }
    if (header.prg_nv_ram_size > 0)
    {
        assert(prg_ram.size() <= 1);
        assert(header.battery);
        nv_ram_i = banks;
        banks += header.prg_nv_ram_size / 0x2000;
        prg_ram_enabled = true;
    }
    prg_ram.resize(banks);

    banks = header.prg_rom_size / 0x4000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x4000);
    }

    if (header.chr_rom_size > 0)
    {
        banks = header.chr_rom_size / 0x1000;
    }
    else if (header.chr_ram_size > 0)
    {
        banks = header.chr_ram_size / 0x1000;
        chr_ram = true;
    }
    chr_mem.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(chr_mem[i].data()), 0x1000);
    }

    assert(prg_rom.size() >= 2 && prg_rom.size() <= 32);
    assert(prg_ram.size() <= 4 && prg_ram.size() != 3);
    assert(chr_mem.size() >= 2 && chr_mem.size() <= 32);
}

std::optional<ubyte> Mapper001::cpuRead(uword address)
{
    if (address < 0x4020) return {};
    else if (address >= 0x4020 && address < 0x6000) // unused
    {
        #ifndef NDEBUG
        std::cerr << "Warning: CPU read from unmapped memory address: " << hex(address) << std::endl; 
        #endif
        return 0;
    }
    else if (address >= 0x6000 && address < 0x8000) // PRG-RAM
    {
        if (!prg_ram.size()) // No PRG-RAM
        {
            #ifndef NDEBUG
            std::cerr << "Warning: CPU read from unmapped memory address: " << hex(address) << std::endl; 
            #endif
            return 0; // TODO open bus?
        }
        else if (!prg_ram_enabled) return 0;
        else return prg_ram[prg_ram_bank % prg_ram.size()][address-0x6000];
    }
    else //PRG-ROM
    {
        ubyte low_bank = 0; // bank indices
        ubyte high_bank = 0;
        assert (prg_rom_bank_mode <= 3);
        switch (prg_rom_bank_mode)
        {
            case 0: // 32 KiB mode
            case 1:
                low_bank = prg_rom_bank & 0x1E;
                high_bank = low_bank + 1;
                break;
            case 2: // Fix first bank
                low_bank = 0;
                high_bank = prg_rom_bank & 0x1F;
                break;
            case 3: // Fix last bank
                low_bank = prg_rom_bank & 0x1F;
                high_bank = prg_rom.size() - 1;
                break;
            default:
                break;
        }
        address -= 0x8000;
        if (address < 0x4000) // Low bank
        {
            return prg_rom[low_bank % prg_rom.size()][address];
        }
        else // high bank
        {
            return prg_rom[high_bank % prg_rom.size()][address - 0x4000];
        }
    }
}

bool Mapper001::cpuWrite(uword address, ubyte data)
{
    if (address < 0x4020) return false;
    else if (address >= 0x4020 && address < 0x6000)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: CPU write to unmapped memory address: " << hex(address) << std::endl; 
        #endif
        return true;
    }
    else if (address >= 0x6000 && address < 0x8000)
    {
        if (!prg_ram.size()) // No PRG-RAM
        {
            #ifndef NDEBUG
            std::cerr << "Warning: CPU read from unmapped memory address: " << hex(address) << std::endl; 
            #endif
            return 0; // TODO open bus?
        }
        else if (!prg_ram_enabled) return true;
        else { prg_ram[prg_ram_bank % prg_ram.size()][address-0x6000] = data; return true; }
    }
    else 
    {
        if (data & 0x80) // check if byte 7 is set
        {
            reg_shift = 0x10;
            prg_rom_bank_mode = 3;
        }
        else
        {
            assert ((reg_shift & 0xE0) == 0); // make sure the top 3 bits are 0

            // Shift data into shift register
            ubyte new_bit = data & 0x01;
            reg_shift |= (new_bit << 5); // set bit 6 before right shifting
            bool set_reg = reg_shift & 01; // set a register after bit shifting 5 times
            reg_shift >>= 1;

            // Set register
            if (set_reg)
            {
                if (address < 0xA000) // control register
                {
                    switch (reg_shift & 0x03) // mirroring
                    {
                        case 0:
                            mirroring = MirrorType::SINGLE_SCREEN_LOWER;
                            break;
                        case 1:
                            mirroring = MirrorType::SINGLE_SCREEN_UPPER;
                            break;
                        case 2:
                            mirroring = MirrorType::VERTICAL;
                            break;
                        case 3:
                            mirroring = MirrorType::HORIZONTAL;
                            break;
                        default:
                            break;
                    }
                    prg_rom_bank_mode = (reg_shift & 0x0C) >> 2;
                    chr_bank_mode = (reg_shift & 0x10) >> 4;
                }
                else if (address < 0xC000) // CHR bank 0
                {
                    chr_bank[0] = reg_shift;

                    // Disable/Enable PRG-RAM if SNROM
                    if (prg_rom.size() <= 8 && chr_mem.size() == 2 && prg_ram.size() == 1)
                    {
                        prg_ram_enabled = !(chr_bank[0] & 0x10);
                    }

                    // Set PRG-RAM bank if SOROM/SUROM/SXROM
                    if (prg_ram.size() == 2) 
                        prg_ram_bank = (chr_bank[0] & 0x40) >> 2;
                    else if (prg_ram.size() == 4) 
                        prg_ram_bank = (chr_bank[0] & 0xC0) >> 2;
                    
                    // Set PRG-RAM bank if SZROM
                    if (prg_ram.size() == 2 && nv_ram_i == 1)
                        prg_ram_bank = (chr_bank[0] & 0x10) >> 4;

                    // Set PRG-ROM bank if 512K PRG-ROM
                    if (prg_rom.size() == 32) 
                        prg_rom_bank = (prg_rom_bank & 0x0F) | (chr_bank[0] & 0x10);
                }
                else if (address < 0xE000) // CHR bank 1
                {
                    chr_bank[1] = reg_shift;

                    // Disable/Enable PRG-RAM if SNROM and not 8KiB mode
                    if (prg_rom.size() <= 8 && chr_mem.size() == 2 && prg_ram.size() == 1 && chr_bank_mode == 1)
                    {
                        prg_ram_enabled = !(chr_bank[1] & 0x10);
                    }

                    // Set PRG-RAM bank if SOROM/SUROM/SXROM and not 8KiB mode
                    if (prg_ram.size() == 2 && chr_bank_mode == 1) 
                        prg_ram_bank = (chr_bank[1] & 0x80) >> 2;
                    else if (prg_ram.size() == 4 && chr_bank_mode == 1) 
                        prg_ram_bank = (chr_bank[1] & 0xC0) >> 2;

                    // Set PRG-RAM bank if SZROM
                    if (prg_ram.size() == 2 && nv_ram_i == 1 && chr_bank_mode == 1)
                        prg_ram_bank = (chr_bank[0] & 0x10) >> 4;

                    // Set PRG-ROM bank if 512K PRG-ROM and not 8KiB mode
                    if (prg_rom.size() == 32 && chr_bank_mode == 1) 
                        prg_rom_bank = (prg_rom_bank & 0x0F) | (chr_bank[1] & 0x10);
                }
                else // PRG bank
                {
                    prg_rom_bank = (prg_rom_bank & 0x10) | (reg_shift & 0x0F);
                    prg_ram_enabled = !(reg_shift & 0x10);
                }
            reg_shift = 0x10;
            }
        }
        return true;
    }
}

std::optional<ubyte> Mapper001::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else
    {
        ubyte low_bank = 0;
        ubyte high_bank = 0;
        if (chr_bank_mode == 0)
        {
            low_bank = (chr_bank[0] & 0x1E) % chr_mem.size();
            high_bank = low_bank + 1;
        }
        else
        {
            low_bank = chr_bank[0] % chr_mem.size();
            high_bank = chr_bank[1] % chr_mem.size();
        }

        if (address < 0x1000)
        {
            return chr_mem[low_bank][address];
        }
        else
        {
            return chr_mem[high_bank][address - 0x1000];
        }
    }
}

bool Mapper001::ppuWrite(uword address, ubyte data)
{
    if (address >= 0x2000) return false;
    else
    {
        if (chr_ram)
        {
            ubyte low_bank = 0;
            ubyte high_bank = 0;
            if (chr_bank_mode == 0)
            {
                low_bank = chr_bank[0] & 0x1E;
                high_bank = low_bank + 1;
            }
            else
            {
                low_bank = chr_bank[0];
                high_bank = chr_bank[1];
            }

            if (address < 0x1000)
            {
                chr_mem[low_bank % chr_mem.size()][address] = data;
            }
            else
            {
                chr_mem[high_bank % chr_mem.size()][address - 0x1000] = data;
            }
        }
        else
        {
            #ifndef NDEBUG
            std::cerr << "Warning: unsupported PPU write to " << hex(address) << std::endl;
            #endif
        }
        return true;
    }
}