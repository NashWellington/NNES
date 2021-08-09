#pragma once

#include "globals.hpp"
#include <algorithm>
#include <unordered_map>

class Config
{
public:
    Config();
    ~Config();

    void setSettings(std::ofstream& cfg);

    // Parses the config file for NES settings
    void getSettings();

// NES config settings
    // https://wiki.nesdev.com/w/index.php/Overscan#For_emulator_developers
    bool HIDE_OVERSCAN = false;

// Input config
    void findSection(const std::string section);
    void findNext(const std::string next);
    std::vector<int32_t> getKeys();
    std::vector<int32_t> getButtons();

private:
    std::ifstream cfg_in;
};