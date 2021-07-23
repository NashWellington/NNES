#pragma once

#include "globals.hpp"
#include <unordered_map>

class Config
{
public:
    Config();
    Config(std::string cfg_file);

    // https://wiki.nesdev.com/w/index.php/Overscan#For_emulator_developers
    // TODO double check to see if I'm implementing this right
    bool HIDE_OVERSCAN = false;
};