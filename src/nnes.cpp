#include "globals.h"
#include "console.h"
#include "audio.h"
#include "video.h"
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

    #ifdef DEBUGGER
    debug_state.filename = rom_filename;
    #endif

    // Setup
    std::shared_ptr<Audio> audio = std::make_shared<Audio>();
    std::shared_ptr<Video> video = std::make_shared<Video>();
    std::shared_ptr<Console> nes = std::make_shared<NES>(audio, video);
    nes->insertROM(rom);
    // TODO initialize Input with pointer to Video and Console
    Input input = Input(nes, audio, video);

    // Timing
    uint64_t frame_count = 0;
    std::chrono::time_point start_time = std::chrono::steady_clock::now();

    bool running = true;
    while (running)
    {
        using namespace std::chrono;
        running = input.poll();
        nes->run(Scheduler::FRAME);
        video->displayFrame();
        std::this_thread::sleep_until(start_time + ++frame_count * frame(1));
    }
    nes->~Console(); // Might need to destroy PPU or APU before destroying SDL frontend classes
    audio->~Audio();
    video->~Video();
    SDL_Quit();

    #ifndef NDEBUG
    // Reset cerr buffer
    std::cerr.rdbuf(nullptr);
    #endif
}