#include "globals.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "bus.h"
#include "boot.h"
#include "display.h"
#include "input.h"
#include "savestate.h"

#ifndef NDEBUG
#define ERROR_LOG_FILENAME "./log/error.log"
#endif

// TODO find better way of handling args
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

    // Check number of arguments
    if (argc < 2 || argc > 3)
    {
        std::cout << "Too few or too many arguments" << std::endl;
        throw std::exception();
    }
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
    // Audio audio = Audio();
    // std::shared_ptr<Audio> audio_ptr = std::make_shared<Audio>(audio);
    // apu.audio = audio_ptr;

    // Main loop
    // TODO time it so 1 frame takes 1/60 seconds
    RunFlags run_flags = {};
    if (argc == 3) run_flags.paused = true;
    uint32_t ppu_cycle = 0;
    uint32_t cycles_per_frame = 262 * 341; // Note: PPU skips a cycle every odd frame

    #ifdef DEBUGGER
    debug_state.filename = rom_filename;
    #endif

    while (!run_flags.finished)
    {
        if (run_flags.paused)
        {
            input.pollInputs(run_flags);
            #ifdef DEBUGGER
            cpu.save(debug_state);
            #endif
            display.displayFrame(run_flags);
        }
        else
        {
            ppu.tick();
            if ((ppu_cycle % 3) == 2) 
            {
                cpu.tick();

                if ((ppu_cycle % 6) == 5)
                    apu.tick();

                #ifdef DEBUGGER
                if (run_flags.tick)
                {
                    run_flags.paused = true;
                    run_flags.tick = false;
                    ppu.addDebug();
                }
                if (run_flags.steps > 0 && cpu.ready())
                {
                    run_flags.steps--;
                    if (run_flags.steps == 0)
                    {
                        run_flags.paused = true;
                        ppu.addDebug();
                    }
                }
                #endif
            }
            ppu_cycle++;
            if ((ppu_cycle == cycles_per_frame) || (ppu_cycle == (cycles_per_frame * 2 - 1)))
            {
                #ifdef DEBUGGER
                cpu.save(debug_state); // Save registers for use by the debugger
                #endif

                display.displayFrame(run_flags);
                ppu_cycle %= cycles_per_frame * 2 - 1;

                #ifdef DEBUGGER
                if (run_flags.frame)
                {
                    run_flags.paused = true;
                    run_flags.frame = false;
                }
                else // Take inputs as normal if not advancing by 1 frame
                {
                    input.pollInputs(run_flags);
                }
                ppu.addDebug();
                #else
                input.pollInputs(run_flags);
                #endif
            }
            #ifdef DEBUGGER
            debug_state.ppu_cycle = ppu_cycle;
            #endif
        }
    }

    #ifndef NDEBUG
    // Reset cerr buffer
    std::cerr.rdbuf(nullptr);
    #endif
}