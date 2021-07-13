#include "boot.h"

void Boot::loadRom(std::ifstream& rom)
{
    // Read header
    Header header = readHeader(rom);

    // Send header info to debug_state
    #ifdef DEBUGGER
    switch (header.type)
    {
        case HeaderType::NONE:
            debug_state.header_type = "No Header";
            break;
        case HeaderType::INES:
            debug_state.header_type = "iNES";
            break;
        case HeaderType::NES20:
            debug_state.header_type = "NES 2.0";
            break;
        case HeaderType::UNIF:
            debug_state.header_type = "UNIF";
            break;
        default:
            break;
    }
    debug_state.mapper = header.mapper;
    debug_state.submapper = header.submapper;
    debug_state.prg_rom_size = header.prg_rom_size;
    debug_state.prg_ram_size = header.prg_ram_size;
    debug_state.chr_rom_size = header.chr_rom_size;
    debug_state.chr_ram_size = header.chr_ram_size;
    #endif

    bus.setMapper(getMapper(header, rom));
}

Header Boot::readHeader(std::ifstream& rom)
{
    Header header = Header();

    ubyte header_data[16] = {0};
    rom.read(reinterpret_cast<char*>(header_data), sizeof(header_data));

    // Detect header type
    if (header_data[0] == 'N' && header_data[1] == 'E' && 
        header_data[2] == 'S' && header_data[3] == 0x1A)
    {
        if ((header_data[7] & 0x0C) == 0x08) header.type = HeaderType::NES20;
        else header.type = HeaderType::INES;
    }
    else header.type = HeaderType::NONE; // TODO UNIF headers

    assert(header.type == HeaderType::INES || header.type == HeaderType::NES20);

    // Detect trainer
    header.trainer = (header_data[6] & 0x04) >> 2;

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

    // Detect CHR-RAM size
    if (header.type == HeaderType::INES)
    {
        if (header.chr_rom_size == 0) header.chr_ram_size = 0x2000;
    }
    else if (header.type == HeaderType::NES20) 
        header.chr_ram_size = static_cast<uint>(64) << (header_data[11] & 0x0F);

    // TODO Detect CHR-NV-RAM size

    // Detect mirroring type
    if (header_data[6] & 0x01) header.mirroring = MirrorType::VERTICAL;
    else header.mirroring = MirrorType::HORIZONTAL;

    return header;
}

std::shared_ptr<Mapper> Boot::getMapper(Header& header, std::ifstream& rom)
{
    std::shared_ptr<Mapper> mapper;
    // TODO array of pointers?
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
            mapper = std::make_shared<Mapper004>(header, rom);
            break;
        
        /* This isn't quite done yet :)
        case 5:
            mapper = std::make_shared<Mapper005>(header, rom);
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
    // TODO maybe delete later
    #ifndef NDEBUG
    std::cerr << "Debug info:" << std::endl;
    std::cerr << "Mapper " << header.mapper << std::endl;
    #endif
    return mapper;
}