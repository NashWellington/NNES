#include "mapper.hpp"

void Mapper::mirrorNametables(uword& address)
{
    if (address >= 0x2000 && address < 0x3F00)
    {
        uint table_i = ((address - 0x2000) % 0x1000) / 0x0400;
        uint tile_i = ((address - 0x2000) % 0x1000) % 0x0400;
        switch (mirroring)
        {
            case MirrorType::HORIZONTAL:
                table_i /= 2;
                break;
            case MirrorType::VERTICAL:
                table_i %= 2;
                break;
            case MirrorType::SINGLE_SCREEN_LOWER:
                table_i = 0;
                break;
            case MirrorType::SINGLE_SCREEN_UPPER:
                table_i = 1;
                break;
            case MirrorType::OTHER:
                std::cerr << "Error: Unsupported mirroring mode" << std::endl;
                throw std::exception();
            default: // currently just 4-screen
                break;
        }
        address = 0x2000 + 0x0400 * table_i + tile_i;
    }
}