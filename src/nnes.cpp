#include "globals.h"
#include "console.h"
#include "audio.h"
#include "video.h"
#include "input.h"
#include "savestate.h"

#ifndef NDEBUG
#define ERROR_LOG_FILENAME "./log/error.log"
#endif

#define LICENSE_STRING "NNES Copyright (C) 2021 Nash Wellington"

/* Args:
* -f,-r : ROM filename
* -d    : Dump state log to file specified // TODO
* -p    : start paused                     // TODO
*/
int main(int argc, char ** argv)
{
    std::cout << LICENSE_STRING << std::endl;

    #ifndef NDEBUG
    // Set up error log
    std::ofstream error_stream(ERROR_LOG_FILENAME);
    if (!error_stream.is_open()) 
    {
        std::cout << "Error: could not open " << ERROR_LOG_FILENAME << std::endl;
        throw std::exception();
    }
    std::cerr.rdbuf(error_stream.rdbuf());
    #endif

    // Argument parsing
    std::vector<std::string_view> args = {};
    args.resize(argc);
    for(int i = 0; i < argc; i++) args[i] = argv[i];

    // Open ROM file
    std::string rom_filename;
    std::optional<std::string_view> arg;
    if ((arg = getOpt(args, "-f")))
        rom_filename = arg.value();
    else if ((arg = getOpt(args, "-r")))
        rom_filename = arg.value();
    else
    {
        std::cout << "Enter ROM filename:" << std::endl;
        std::cin >> rom_filename;
    }
    std::ifstream rom(rom_filename, std::ios::binary);
    if (!rom.is_open())
    {
        std::cerr << "File not found: " << rom_filename << std::endl;
        throw std::exception();
    }
    
    // Setup console and frontends
    Audio audio = Audio();
    Video video = Video();
    NES nes = NES(audio, video);

    nes.insertROM(rom);
    // For now, input has to be initialized after a ROM is loaded
    // This is because controllers don't get initialized until after ROM loading
    // TODO handle binds after Input is initialized and/or initialize controllers at console initialization
    Input input = Input(nes, audio, video);

    // Timing
    uint64_t frame_count = 0;
    std::chrono::time_point start_time = std::chrono::steady_clock::now();

    bool running = true;
    while (running)
    {
        using namespace std::chrono;
        running = input.poll();
        nes.run(Scheduler::FRAME);
        video.displayFrame();
        std::this_thread::sleep_until(start_time + ++frame_count * frame(1));
    }
    nes.~NES(); // Might need to destroy PPU or APU before destroying SDL frontend classes
    audio.~Audio();
    video.~Video();
    SDL_Quit();

    #ifndef NDEBUG
    // Reset cerr buffer
    std::cerr.rdbuf(nullptr);
    #endif
}