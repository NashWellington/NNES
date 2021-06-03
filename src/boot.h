#ifndef NNES_BOOT_H
#define NNES_BOOT_H

#include "globals.h"
#include "bus.h"
#include "mapper.h"

class Boot
{
public:
    static void loadRom(std::ifstream& rom, Bus& bus);
private:
    static Header readHeader(std::ifstream& rom);

    static std::shared_ptr<Mapper> getMapper(Header& header, std::ifstream& rom);
};

#endif