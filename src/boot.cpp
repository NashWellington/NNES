#include "boot.h"

void Boot::loadRom(std::ifstream& rom)
{
    // Read header
    Header header = readHeader(rom);
    bus.setMapper(getMapper(header, rom));
}

Header Boot::readHeader(std::ifstream& rom)
{
    Header header = Header();

    byte header_data[16] = {0};
    rom.read(reinterpret_cast<char*>(header_data), sizeof(header_data));

    // Detect header type
    if (header_data[0] == 'N' && header_data[1] == 'E' && 
        header_data[2] == 'S' && header_data[3] == 0x1A)
    {
        if ((header_data[7] & 0x0C) == 0x08)
        {
            header.type = HeaderType::NES20_HEADER;
        }
        else
        {
            header.type = HeaderType::INES_HEADER;
        }
    }
    else // TODO UNIF headers
    {
        header.type = HeaderType::NO_HEADER;
    }

    // Detect trainer
    header.trainer = (header_data[6] & 0x04) >> 2;

    // Detect mapper type
    if (header.type == HeaderType::INES_HEADER || header.type == HeaderType::NES20_HEADER)
    {
        header.mapper = (header_data[7] & 0xF0) + ((header_data[6] & 0xF0) >> 4);
    }

    if (header.type == HeaderType::NES20_HEADER) 
        header.mapper += uword(header_data[8] & 0x0F) << 8;

    // Detect PRG-ROM size
    uword prg_rom_ctrl = header_data[4];

    if (header.type == HeaderType::NES20_HEADER) 
        prg_rom_ctrl += (uword(header_data[9] & 0x0F) << 8);

    if ((prg_rom_ctrl & 0x0F00) != 0x0F00) // prg_rom_size = prg_rom_ctrl * 16 KiB if MSB is not 0x0F
    {
        header.prg_rom_size = prg_rom_ctrl * (2 << 13);
    }
    else
    {
        std::cerr << "PRG-ROM size not supported yet" << std::endl;
        throw std::exception();
    }

    // Detect CHR-ROM size
    uword chr_rom_ctrl = header_data[5];
    if (header.type == HeaderType::NES20_HEADER)
        chr_rom_ctrl += (uword(header_data[9] & 0xF0) << 4);
    if ((chr_rom_ctrl & 0x0F00) != 0x0F00) // chr_rom_size = chr_rom_ctrl * 8 KiB if MSB is not 0x0F
    {
        header.chr_rom_size = chr_rom_ctrl * (2 << 12);
    }
    else
    {
        std::cerr << "CHR-ROM size not supported yet" << std::endl;
        throw std::exception();
    }

    // Detect PRG-RAM size
    ubyte prg_ram_ctrl = header_data[10] & 0x0F;
    if (prg_ram_ctrl != 0)
    {
        header.prg_ram_size = 64 << prg_ram_ctrl;
    }

    // Detect mirroring type
    if (header_data[6] & 0x01) header.mirroring = MirrorType::VERTICAL;
    else header.mirroring = MirrorType::HORIZONTAL;

    return header;
}

std::shared_ptr<Mapper> Boot::getMapper(Header& header, std::ifstream& rom)
{
    std::shared_ptr<Mapper> mapper;
    switch (header.mapper)
    {
        case 0:
            mapper = std::make_shared<Mapper000>(header, rom);
            break;

        case 1:
            mapper = std::make_shared<Mapper001>(header, rom);
            break;

        default:
            std::cerr << "Error: unsupported mapper type: " << header.mapper << std::endl;
            throw std::exception();
    }
    return mapper;
}