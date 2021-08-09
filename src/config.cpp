#include "config.hpp"

#define CFG_FILE "./cfg/config.ini"

// TODO save default cfg options if cfg file not found

Config::Config()
{
    cfg_in.open(CFG_FILE);
    if (!cfg_in.is_open())
    {
        std::cerr << "Error: could not open config file " << CFG_FILE << std::endl;
        throw std::exception();
    }
    getSettings();
}

Config::~Config()
{
    cfg_in.close();
}

void Config::getSettings()
{
    cfg_in.seekg(0);
    findSection("/// NES settings");

    std::string line = "";
    getline(cfg_in, line);

    while (!cfg_in.eof() && line.substr(0,3) != "///") // Exit once we enter the next section
    {
        getline(cfg_in, line);
        
        if (line != "" && line.substr(0,2) != "//") // Valid line to use for settings
        {
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end()); // Remove spaces
            std::string setting = line.substr(0, line.find(":"));
            bool toggle;
            std::string toggle_str = line.substr(line.find(":")+1, line.size());
            if (toggle_str == "false")
            {
                toggle = false;
            }
            else if (toggle_str == "true")
            {
                toggle = true;
            }
            else
            {
                std::cerr << "Error: invalid value for " << setting; 
                std::cerr << ": \"" << toggle_str << "\"";
                std::cerr << " at line " << cfg_in.tellg() << std::endl;
                throw std::exception();
            }
            
            if (setting == "HIDE_OVERSCAN")
            {
                HIDE_OVERSCAN = toggle;
            }
        }
    }
}

void Config::findSection(const std::string section)
{
    cfg_in.seekg(0);
    std::string line = "";
    while (line != section) // Skip to correct section
    {
        getline(cfg_in, line);
    }
}

void Config::findNext(const std::string next)
{
    std::string line = "";
    while (line != next)
    {
        getline(cfg_in, line);
        line = line.substr(0,line.find(":"));
    }
}

// ifdef SDL
std::vector<int32_t> Config::getKeys()
{
    std::string keys = "";
    getline(cfg_in, keys);
    keys.erase(std::remove_if(keys.begin(), keys.end(), isspace), keys.end()); // Trim whitespace
    if (keys.substr(0, keys.find(":")) != "keys")
    {
        std::cerr << "Error: incorrect config file formatting" << std::endl;
        throw std::exception();
    }
    keys = keys.substr(keys.find(":")+1, keys.size());
    std::vector<int32_t> keycodes = {};
    int delim_pos = keys.find(","); 
    do
    {
        auto key = keys.substr(0,delim_pos);
        int32_t keycode = 0;
        if (key.size() == 0) break;
        else if (key.size() == 1) // Most keys just use their ASCII code
        {
            keycode = static_cast<int32_t>(key[0]);
        }
        else if (key == "return") keycode = 13;
        else if (key == "escape") keycode = 27;
        else if (key == "backspace") keycode = 8;
        else if (key == "tab") keycode = 9;
        else if (key == "space") keycode = 32;
        else
        {
            std::cerr << "Error: incorrect config file formatting" << std::endl;
            throw std::exception();
        }
        keycodes.push_back(keycode);
        keys = keys.substr(delim_pos+1,keys.size());
    }
    while (delim_pos >= 0);
    return keycodes;
}

std::vector<int32_t> Config::getButtons()
{
    std::string buttons = "";
    getline(cfg_in, buttons);
    buttons.erase(std::remove_if(buttons.begin(), buttons.end(), isspace), buttons.end()); // Trim whitespace
    if (buttons.substr(0, buttons.find(":")) != "ctrl")
    {
        std::cerr << "Error: incorrect config file formatting" << std::endl;
        throw std::exception();
    }
    buttons = buttons.substr(buttons.find(":")+1, buttons.size());
    std::vector<int32_t> button_codes = {};
    int delim_pos = buttons.find(","); 
    do
    {
        auto button = buttons.substr(0,delim_pos);
        int32_t button_code = 0;

        if (button.size() == 0) break;
        else if (button == "button-right")       button_code = 0;
        else if (button == "button-bottom") button_code = 1;
        else if (button == "button-left")   button_code = 2;
        else if (button == "button-top")    button_code = 3;
        else if (button == "button-select") button_code = 4;
        // TODO Xinput guide button
        else if (button == "button-start")  button_code = 6;
        else if (button == "button-l3")     button_code = 7;  // Left stick
        else if (button == "button-r3")     button_code = 8;  // Right stick
        else if (button == "button-l1")     button_code = 9;  // Left bumper/shoulder
        else if (button == "button-r1")     button_code = 10; // Right bumper/shoulder
        else if (button == "dpad-up")       button_code = 11;
        else if (button == "dpad-down")     button_code = 12;
        else if (button == "dpad-left")     button_code = 13;
        else if (button == "dpad-right")    button_code = 14;
        else
        {
            std::cerr << "Error: incorrect config file formatting" << std::endl;
            throw std::exception();
        }

        button_codes.push_back(button_code);
        buttons = buttons.substr(delim_pos+1,buttons.size());
    }
    while (delim_pos >= 0);
    return button_codes;
}
// else etc.