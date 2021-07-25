#pragma once

#include "globals.hpp"
class Mapper;
#include "mappers/mapper.hpp"

namespace Boot
{
    // Parses a filename and returns its corresponding game name
    std::string getName(std::string filename);
    Header readHeader(std::ifstream& rom, std::string filename);
    std::shared_ptr<Mapper> getMapper(NES& nes, Header& header, std::ifstream& rom);
};