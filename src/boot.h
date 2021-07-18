#pragma once

// Forward declaration
//class Mapper;

#include "globals.h"
#include "mappers/mapper.h"

namespace Boot
{
    // Parses a filename and returns its corresponding game name
    std::string getName(std::string filename);
    Header readHeader(std::ifstream& rom, std::string filename);
    std::shared_ptr<Mapper> getMapper(Header& header, std::ifstream& rom);
};