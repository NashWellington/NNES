#include "mapper.hpp"

// TODO support submapper 5

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
        if (header.battery) header.prg_nv_ram_size = 0x2000;
        else header.prg_ram_size = 0x2000;
    }

    uint banks = 0;
    if (header.prg_ram_size > 0)
    {
        banks = header.prg_ram_size / 0x2000;
    }
    if (header.prg_nv_ram_size > 0)
    {
        assert(prg_ram.size() <= 1);
        nv_ram_i = banks;
        banks += header.prg_nv_ram_size / 0x2000;
    }
    prg_ram.resize(banks);

    if (header.battery)
    {
        assert(header.prg_nv_ram_size > 0);
        save_name = "./saves/" + header.name + ".sav";
        loadFromFile(save_name);
        save_file.open(save_name, std::fstream::in | std::fstream::out | std::ios::binary);
    }

    banks = header.prg_rom_size / 0x4000;
    prg_rom.resize(banks);
    for (uint i = 0; i < banks; i++)
    {
        rom.read(reinterpret_cast<char*>(prg_rom[i].data()), 0x4000);
    }
    // 512K ROM startup
    if (prg_rom.size() == 32)
    {
        reg_chr_bank_0 = 0x10;
        reg_chr_bank_1 = 0x10;
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

    #ifndef NDEBUG
    header.print(std::cerr);
    #endif

    assert(prg_rom.size() >= 2 && prg_rom.size() <= 32);
    assert(prg_ram.size() <= 4 && prg_ram.size() != 3);
    assert(chr_mem.size() >= 2 && chr_mem.size() <= 32);
}

Mapper001::~Mapper001()
{
    save_file.close();
}

void Mapper001::saveToFile(uword address, ubyte data)
{
    save_file.seekp(address);
    save_file << static_cast<char>(data);
}

void Mapper001::loadFromFile(std::string name)
{
    std::fstream load_file;
    load_file.open(name, std::fstream::in | std::fstream::out | std::ios::binary);
    if (!load_file)
    {
        #ifndef NDEBUG
        std::cerr << "Warning: " << name << " not found" << std::endl;
        #endif
        load_file.open(name, std::fstream::out | std::fstream::trunc | std::ios::binary);
        if (load_file)
        {
            // Zero out enough bytes
            for (uint i = nv_ram_i; i < prg_ram.size(); i++)
            {
                load_file.write(reinterpret_cast<char*>(prg_ram[i].data()), 0x2000);
            }
        }
        else
        {
            std::cerr << "Error: save file could not be opened" << std::endl;
            throw std::exception();
        }
    }
    else
    {
        for (uint i = nv_ram_i; i < prg_ram.size(); i++)
        {
            load_file.read(reinterpret_cast<char*>(prg_ram[i].data()), 0x2000);
        }
    }
    load_file.close();
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
            return 0; // TODO open bus
        }
        else if ((reg_prg_bank & 0x10) // PRG-RAM disabled
            || ((reg_chr_bank_0 & 0x10) 
            && (prg_rom.size() <= 16 && chr_mem.size() == 2 && chr_ram
            && prg_ram.size() == 1)))  // extra disable check for SNROM
        {
            return 0; // TODO open bus
        }
        else 
        {
            uint bank = 0;
            if (prg_ram.size() == 2 && nv_ram_i == 1 && chr_mem.size() >= 4)
            {
                bank |= (reg_chr_bank_0 & 0x10) >> 4;     // SZROM
            }
            else
            {
                if (prg_ram.size() >= 2) 
                    bank |= (reg_chr_bank_0 & 0x08) >> 3; // SOROM/SXROM
                if (prg_ram.size() == 4) 
                    bank |= (reg_chr_bank_0 & 0x04) >> 1; // SXROM
            }
            assert(bank < prg_ram.size());
            return prg_ram[bank % prg_ram.size()][address-0x6000];
        }
    }
    else //PRG-ROM
    {
        ubyte low_bank = 0; // bank indices
        ubyte high_bank = 0;
        assert(prg_rom_bank_mode <= 3);
        uint bank = reg_prg_bank & 0x0F;
        // CHR bank bit 4 used for 256K prg bank select if PRG-ROM = 512K
        if (prg_rom.size() == 32) bank |= (reg_chr_bank_0 & 0x10);
        switch (prg_rom_bank_mode)
        {
            case 0: // 32 KiB mode
            case 1:
                low_bank = bank & 0x1E;
                high_bank = low_bank + 1;
                break;
            case 2: // Fix first bank
                low_bank = bank & 0x10;
                high_bank = bank & 0x1F;
                break;
            case 3: // Fix last bank
                low_bank = bank & 0x1F;
                high_bank = (bank & 0x10) | ((prg_rom.size() - 1) & 0x0F);
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
            return true; // TODO open bus
        }
        else if ((reg_prg_bank & 0x10) // PRG-RAM disabled
            || ((reg_chr_bank_0 & 0x10) 
            && (prg_rom.size() <= 16 && chr_mem.size() == 2 && chr_ram
            && prg_ram.size() == 1)))  // extra disable check for SNROM
        {
            return true;
        }
        else 
        {
            uint bank = 0;
            if (prg_ram.size() == 2 && nv_ram_i == 1 && chr_mem.size() >= 4)
            {
                bank |= (reg_chr_bank_0 & 0x10) >> 4;     // SZROM
            }
            else
            {
                if (prg_ram.size() >= 2) 
                    bank |= (reg_chr_bank_0 & 0x08) >> 3; // SOROM/SXROM
                if (prg_ram.size() == 4) 
                    bank |= (reg_chr_bank_0 & 0x04) >> 1; // SXROM
            }
            assert(bank < prg_ram.size());
            prg_ram[bank][address-0x6000] = data;
            if (bank >= static_cast<uint>(nv_ram_i)) 
            {
                saveToFile(0x2000 * (bank - nv_ram_i) + (address-0x6000), data);
            }
            return true;
        }
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
            reg_shift |= (new_bit << 5); // set bit 5 before right shifting
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
                    reg_chr_bank_0 = reg_shift;
                }
                else if (address < 0xE000) // CHR bank 1
                {
                    reg_chr_bank_1 = reg_shift;
                }
                else // PRG bank
                {
                    reg_prg_bank = reg_shift;
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
            low_bank = (reg_chr_bank_0 & 0x1E) % chr_mem.size();
            high_bank = low_bank + 1;
        }
        else
        {
            low_bank = reg_chr_bank_0 % chr_mem.size();
            high_bank = reg_chr_bank_1 % chr_mem.size();
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
                low_bank = (reg_chr_bank_0 & 0x1E) % chr_mem.size();
                high_bank = low_bank + 1;
            }
            else
            {
                low_bank = reg_chr_bank_0 % chr_mem.size();
                high_bank = reg_chr_bank_1 % chr_mem.size();
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