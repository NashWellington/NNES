#pragma once

#include "globals.hpp"
#include <algorithm>
#include <unordered_map>

class Config
{
public:
    Config();

    void setSettings(std::ofstream& cfg);

    // Parses the config file for NES settings
    void getSettings(std::ifstream& cfg);

// NES config settings
    // https://wiki.nesdev.com/w/index.php/Overscan#For_emulator_developers
    bool HIDE_OVERSCAN = false;

// Input config
    std::string getConfigBinds();
};