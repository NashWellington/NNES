#pragma once

#include "globals.hpp"
#include <unordered_map>

class Config
{
public:
    // Config();

    void   serialize(std::ofstream& cfg);
    void deserialize(std::ifstream& cfg);

// NES config values
    // https://wiki.nesdev.com/w/index.php/Overscan#For_emulator_developers
    bool HIDE_OVERSCAN = false;

// Input config

};