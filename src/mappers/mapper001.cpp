#include "mapper.h"

// TODO test
Mapper001::Mapper001(Header& header, std::ifstream& rom)
{
    mirroring = header.mirroring;
    assert(!header.trainer);
    assert(header.prg_rom_size == 0x40000 || header.prg_rom_size == 0x80000);
    // TODO testing
    std::cout << header.chr_rom_size << std::endl;
    std::cout << header.chr_ram_size << std::endl;
    assert((header.chr_rom_size == 0x20000) != (header.chr_ram_size == 0x20000));
    assert(header.prg_ram_size == 0 || header.prg_ram_size == 0x2000);

    if (header.prg_ram_size > 0)
    {
        rom.read(reinterpret_cast<char*>(prg_ram.data()), prg_ram.size());
        prg_ram_enabled = true;
    }
    uint banks = header.prg_rom_size / 0x4000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x4000);
    }
    banks = header.chr_rom_size / 0x1000;
    chr_mem.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(chr_mem[i].data()), 0x1000);
    }
}

std::optional<byte> Mapper001::cpuRead(uword address)
{
    if (address < 0x6000) return {};
    else if (address < 0x8000) // PRG-RAM
    {
        if (!prg_ram_enabled) return {};
        else
        {
            // TODO bank switching
            // TODO this probably has something to do w/ submappers...?
            return {};
        }
    }
    else //PRG-ROM
    {
        ubyte low_bank = 0; // bank indices
        ubyte high_bank = 0;
        assert (prg_bank_mode <= 3);
        switch (prg_bank_mode)
        {
            case 0: // 32 KiB mode
            case 1:
                low_bank = prg_bank & 0x0E;
                high_bank = low_bank + 1;
                break;
            case 2: // Fix first bank
                low_bank = 0;
                high_bank = prg_bank;
                break;
            case 3: // Fix last bank
                low_bank = prg_bank;
                high_bank = prg_rom.size() - 1;
                break;
            default:
                break;
        }
        address -= 0x8000;
        if (address < 0x4000) // Low bank
        {
            return prg_rom[low_bank][address];
        }
        else // high bank
        {
            return prg_rom[high_bank][address - 0x4000];
        }
    }
}

bool Mapper001::cpuWrite(uword address, byte data)
{
    if (address < 0x6000) return false;
    else if (address < 0x8000)
    {
        if (!prg_ram_enabled) return false;
        else return true;
    }
    else 
    {
        if (data < 0) // check if byte 7 is set
        {
            reg_shift = 0x10;
            prg_bank_mode = 3;
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
                    prg_bank_mode = (reg_shift & 0x0C) >> 2;
                    chr_bank_mode = (reg_shift & 0x10) >> 4;
                }
                else if (address < 0xC000) // CHR bank 0
                {
                    chr_bank_0 = reg_shift;
                }
                else if (address < 0xE000) // CHR bank 1
                {
                    chr_bank_1 = reg_shift;
                }
                else // PRG bank
                {
                    prg_bank = reg_shift & 0x0F;
                    prg_ram_enabled = static_cast<bool>((reg_shift & 0x10) >> 4);
                }
            }
        }
        return true;
    }
}

std::optional<byte> Mapper001::ppuRead(uword address)
{
    if (address >= 0x2000) return {};
    else
    {
        ubyte low_bank = 0;
        ubyte high_bank = 0;
        if (chr_bank_mode == 0)
        {
            low_bank = chr_bank_0 & 0x1E;
            high_bank = low_bank + 1;
        }
        else
        {
            low_bank = chr_bank_0;
            high_bank = chr_bank_1;
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

bool Mapper001::ppuWrite(uword address, byte data)
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
                low_bank = chr_bank_0 & 0x1E;
                high_bank = low_bank + 1;
            }
            else
            {
                low_bank = chr_bank_0;
                high_bank = chr_bank_1;
            }

            if (address < 0x1000)
            {
                chr_mem[low_bank][address] = data;
            }
            else
            {
                chr_mem[high_bank][address - 0x1000] = data;
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