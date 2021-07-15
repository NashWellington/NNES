#pragma once

// Forward declaration
//class Mapper;

#include "globals.h"
#include "mappers/mapper.h"

namespace Boot
{
    Header readHeader(std::ifstream& rom);
    std::shared_ptr<Mapper> getMapper(Header& header, std::ifstream& rom);
};