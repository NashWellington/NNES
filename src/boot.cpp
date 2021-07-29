#include "boot.hpp"

// TODO move this to globals/util? Return file extension?
std::string Boot::getName(std::string filename)
{
    std::string name = filename.substr(filename.rfind("/")+1);
    int i = name.find(".");
    std::string extension = name.substr(i+1);
    name = name.substr(0, i);
    return name;
}

Header Boot::readHeader(std::ifstream& rom, std::string filename)
{
    Header header = Header();
    header.name = getName(filename);

    ubyte header_data[16] = {0};
    rom.read(reinterpret_cast<char*>(header_data), sizeof(header_data));

    // Detect header type
    if (header_data[0] == 'N' && header_data[1] == 'E' && 
        header_data[2] == 'S' && header_data[3] == 0x1A)
    {
        if ((header_data[7] & 0x0C) == 0x08) 
        {
            header.type = HeaderType::NES20;
            #ifndef NDEBUG
            std::cerr << "Header format: iNES 2.0" << std::endl;
            #endif
        }
        else 
        {
            header.type = HeaderType::INES;
            #ifndef NDEBUG
            std::cerr << "Header format: iNES" << std::endl;
            #endif
        }
    }
    else header.type = HeaderType::NO_HEADER; // TODO UNIF headers

    assert(header.type == HeaderType::INES || header.type == HeaderType::NES20);

    // Detect mirroring type
    // Note: setting to horizontal here could also mean mapper-controlled
    header.mirroring = (header_data[6] & 0x01) ? MirrorType::VERTICAL : MirrorType::HORIZONTAL;

    // Detect battery
    header.battery = (header_data[6] & 0x02) >> 1;

    // Detect trainer
    header.trainer = (header_data[6] & 0x04) >> 2;

    // TODO hard-wired four-screen mode

    // TODO console type (NES, Vs. System, etc.)

    // Detect mapper type
    if (header.type == HeaderType::INES || header.type == HeaderType::NES20) 
        header.mapper = static_cast<uint>(header_data[7] & 0xF0) + static_cast<uint>((header_data[6] & 0xF0) >> 4);
    if (header.type == HeaderType::NES20)
        header.mapper += uword(header_data[8] & 0x0F) << 8;

    // Detect submapper type
    if (header.type == HeaderType::NES20) 
        header.submapper = (header_data[8] & 0xF0) >> 4;

    // Detect PRG-ROM size
    uword prg_rom_ctrl = header_data[4];
    if (header.type == HeaderType::NES20) 
        prg_rom_ctrl += static_cast<uword>((header_data[9] & 0x0F) << 8);

    if ((prg_rom_ctrl & 0x0F00) != 0x0F00) // prg_rom_size = prg_rom_ctrl * 16 KiB if MSB is not 0x0F
    {
        header.prg_rom_size = static_cast<uint>(prg_rom_ctrl * 0x4000);
    }
    else
    {
        std::cerr << "PRG-ROM size not supported yet" << std::endl;
        throw std::exception();
    }

    // Detect CHR-ROM size
    uword chr_rom_ctrl = header_data[5];
    if (header.type == HeaderType::NES20)
        chr_rom_ctrl += static_cast<uword>((header_data[9] & 0xF0) << 4);
    if ((chr_rom_ctrl & 0x0F00) != 0x0F00) // chr_rom_size = chr_rom_ctrl * 8 KiB if MSB is not 0x0F
    {
        header.chr_rom_size = static_cast<uint>(chr_rom_ctrl * 0x2000);
    }
    else
    {
        std::cerr << "CHR-ROM size not supported yet" << std::endl;
        throw std::exception();
    }

    // Detect PRG-RAM size
    if (header.type == HeaderType::INES)
    {
        header.prg_ram_size = static_cast<uint>(header_data[8] * 0x2000);
        // NOTE: if prg_ram_size is 0, mapper will set it to some default
    }
    else if (header.type == HeaderType::NES20)
    {
        ubyte prg_ram_ctrl = header_data[10] & 0x0F;
        if (prg_ram_ctrl != 0) 
            header.prg_ram_size = static_cast<uint>(64) << prg_ram_ctrl;
    }

    // Detect PRG-NV-RAM/EEPROM size
    if (header.type == HeaderType::NES20)
    {
        ubyte prg_nv_ram_ctrl = (header_data[10] & 0xF0) >> 4;
        if (prg_nv_ram_ctrl) header.prg_nv_ram_size = 64 << prg_nv_ram_ctrl;
    }

    // Detect CHR-RAM size
    if (header.type == HeaderType::INES)
    {
        if (header.chr_rom_size == 0) header.chr_ram_size = 0x2000;
    }
    else if (header.type == HeaderType::NES20)
    {
        uint chr_ram_ctrl = header_data[11] & 0x0F;
        if (chr_ram_ctrl != 0) header.chr_ram_size = 64 << chr_ram_ctrl;
    }

    // Detect CHR-NV-RAM size
    if (header.type == HeaderType::NES20)
    {
        ubyte chr_nv_ram_ctrl = (header_data[11] & 0xF0) >> 4;
        if (chr_nv_ram_ctrl) header.chr_nv_ram_size = 64 << chr_nv_ram_ctrl;
    }

    // Detect Region
    switch (header_data[12])
    {
        case 0:
            header.region = Region::NTSC;
            break;
        case 1:
            header.region = Region::PAL;
            break;
        case 2:
            header.region = Region::OTHER; // Multi-region
            break;
        case 3:
            header.region = Region::Dendy;
            break;
    }
    // TODO non-NTSC region support
    if (header.region != Region::NTSC)
    {
        std::cerr << "Error: unsupported region" << std::endl;
        std::cerr << "Forcing region to NTSC" << std::endl;
        header.region = Region::NTSC;
    }

    // TODO Vs. System/Extended Console type

    // Miscellaneous ROM presence
    header.misc_rom_num = header_data[14];

    // TODO default expansion device

    return header;
}

std::shared_ptr<Mapper> Boot::getMapper(NES& nes, Header& header, std::ifstream& rom)
{
    std::shared_ptr<Mapper> mapper;
    // TODO function pointer array?
    switch (header.mapper)
    {
        case 0:
            mapper = std::make_shared<Mapper000>(header, rom);
            break;
        case 1:
            mapper = std::make_shared<Mapper001>(header, rom);
            break;
        case 2:
            mapper = std::make_shared<Mapper002>(header, rom);
            break;
        case 3:
            mapper = std::make_shared<Mapper003>(header, rom);
            break;

        case 4:
            mapper = std::make_shared<Mapper004>(nes, header, rom);
            break;
        
        /* This isn't quite done yet :)
        case 5:
            mapper = std::make_shared<Mapper005>(header);
            break;
        */

        case 7:
            mapper = std::make_shared<Mapper007>(header, rom);
            break;

        case 9:
            mapper = std::make_shared<Mapper009>(header, rom);
            break;

        default:
            std::cerr << "Error: unsupported mapper type: " << header.mapper << std::endl;
            throw std::exception();
    }
    return mapper;
}