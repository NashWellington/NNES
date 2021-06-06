#include "globals.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "bus.h"
#include "boot.h"
//#include "input.h"
#include "display.h"
#include "savestate.h"

#ifndef NDEBUG
#define ERROR_LOG_FILENAME "../build/log/error.log"
#endif

int main(int argc, char ** argv)
{
    #ifndef NDEBUG
    // Set up error log
    std::ofstream error_stream(ERROR_LOG_FILENAME);
    if (!error_stream.is_open()) 
    {
        std::cout << "Error: could not open " << ERROR_LOG_FILENAME << std::endl;
        throw std::exception();
    }
    std::cerr.rdbuf(error_stream.rdbuf());

    #ifdef DISP_INSTR
    // Set up state log
    std::ofstream state_stream(argv[2]);
    if (!state_stream.is_open()) 
    {
        std::cout << "Error: could not open " << argv[2] << std::endl;
    }
    std::clog.rdbuf(state_stream.rdbuf());

    // Check number of arguments
    if (argc != 3)
    {
        std::cout << "Too few or too many arguments" << std::endl;
        throw std::exception();
    }

    #else
    // Check number of arguments
    if (argc != 2)
    {
        std::cout << "Too few or too many arguments" << std::endl;
        throw std::exception();
    }
    #endif
    #endif

    // Open ROM file
    std::string rom_filename = argv[1];
    std::ifstream rom(rom_filename, std::ios::binary);
    if (!rom.is_open())
    {
        std::cerr << "File not found: " << rom_filename << std::endl;
        throw std::exception();
    }

    // Initialize everything
    Boot::loadRom(rom);
    cpu.start();

    #ifndef NDEBUG
    bool done = false;
    unsigned long long int ppu_cycles = 0;
    unsigned long long int cycles_per_frame = 262 * 341; // Note: PPU skips a cycle every odd frame
    while (!done) 
    {
        ppu.clock();
        if ((ppu_cycles % 3) == 2) cpu.clock();
        ppu_cycles++;
        done = display.pollEvents(); // TODO make sure game input only polls once per frame
        if (ppu_cycles == cycles_per_frame)
        {
            display.displayFrame();
        }
        else if (ppu_cycles == (cycles_per_frame * 2 - 1))
        {
            display.displayFrame();
            ppu_cycles -= (cycles_per_frame * 2 - 1);
        }
    }
    #endif

    // TODO main loop (after I figure out input & GUI)

    // TODO resetting buffers necessary in c++?

    #ifndef NDEBUG
    #ifdef DISP_INSTR
    // Reset clog buffer
    std::clog.rdbuf(nullptr);
    #endif
    // Reset cerr buffer
    std::cerr.rdbuf(nullptr);
    #endif
}