#pragma once

#include "globals.h"
#include "bus.h"
#include "mappers/mapper.h"

class Boot
{
public:
    static void loadRom(std::ifstream& rom);
private:
    static Header readHeader(std::ifstream& rom);
    static std::shared_ptr<Mapper> getMapper(Header& header, std::ifstream& rom); // TODO unique_ptr?
};