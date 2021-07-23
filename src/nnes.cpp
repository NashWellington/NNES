#include "globals.hpp"
#include "console.hpp"
#include "audio.hpp"
#include "video.hpp"
#include "input.hpp"
#include "savestate.hpp"

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
    std::optional<std::string> rom_filename;
    if ((rom_filename = getOpt(args, "-f")));
    else ((rom_filename = getOpt(args, "-r")));
    
    std::ifstream rom;
    while (!rom.is_open())
    {
        if (!rom_filename)
        {
            std::cout << "Enter ROM filename:" << std::endl;
            rom_filename = "";
            std::cin >> rom_filename.value();
        }
        rom = std::ifstream(rom_filename.value(), std::ios::binary);
        if (!rom.is_open())
        {
            std::cout << "File not found: " << rom_filename.value() << std::endl;
            rom_filename.reset();
        }
    }
    
    // Setup console and frontends
    std::unique_ptr<Audio> audio = std::make_unique<Audio>();
    std::unique_ptr<Video> video = std::make_unique<Video>();
    std::unique_ptr<NES> nes = std::make_unique<NES>(*audio, *video);

    nes->insertROM(rom, rom_filename.value());
    // For now, input has to be initialized after a ROM is loaded
    // This is because controllers don't get initialized until after ROM loading
    // TODO handle binds after Input is initialized and/or initialize controllers at console initialization
    std::unique_ptr<Input> input = std::make_unique<Input>(*nes, *audio, *video);

    // Timing
    uint64_t frame_count = 0;
    std::chrono::time_point start_time = std::chrono::steady_clock::now();

    bool running = true;
    while (running)
    {
        using namespace std::chrono;
        running = input->poll();
        nes->run(Scheduler::FRAME);
        video->displayFrame();
        std::this_thread::sleep_until(start_time + ++frame_count * frame(1));
    }
    // Call destructors so SDL subsystems quit before SDL_Quit()
    nes.reset();
    audio.reset();
    video.reset();
    input.reset();
    SDL_Quit();

    #ifndef NDEBUG
    // Reset cerr buffer
    std::cerr.rdbuf(nullptr);
    #endif
}