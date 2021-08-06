#include "config.hpp"

#define CFG_FILE "./cfg/config.ini"

// TODO save default cfg options if cfg file not found

Config::Config()
{
    std::ifstream cfg;
    cfg.open(CFG_FILE);
    if (!cfg.is_open())
    {
        std::cerr << "Error: could not open config file " << CFG_FILE << std::endl;
        throw std::exception();
    }
    getSettings(cfg);
    cfg.close();
}

void Config::getSettings(std::ifstream& cfg)
{
    cfg.seekg(0);
    uint line_num = 0;
    std::string line = "";
    while (line != "/// NES settings") // Skip to correct section
    {
        getline(cfg, line); line_num++;
    }

    getline(cfg, line); line_num++;

    while (!cfg.eof() && line.substr(0,3) != "///") // Exit once we enter the next section
    {
        getline(cfg, line); line_num++;
        
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
                std::cerr << " at line " << line_num << std::endl;
                throw std::exception();
            }
            
            if (setting == "HIDE_OVERSCAN")
            {
                HIDE_OVERSCAN = toggle;
            }
        }
    }
}