#include "config.hpp"

#define CFG_FILENAME "./cfg/config.ini"

// TODO save default cfg options if cfg file not found

// Config::Config()
// {
//     std::ifstream cfg;
//     cfg.open(CFG_FILENAME, std::fstream::in);
//     if (!cfg.is_open())
//     {
//         std::cerr << "Error: could not open config file " << CFG_FILENAME << std::endl;
//         throw std::exception();
//     }
//     // TODO
//     //deserialize(cfg);
//     cfg.close();
// }