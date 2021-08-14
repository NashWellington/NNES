#include "globals.hpp"
#include "console.hpp"
#include "audio.hpp"
#include "video.hpp"
#include "input.hpp"
#include "savestate.hpp"
#include "cmd.hpp"

#ifndef NDEBUG
#define ERROR_LOG_FILENAME "./log/error.log"
#endif

#define LICENSE_STRING "NNES Copyright (C) 2021 Nash Wellington\n"
#define USAGE_STRING \
"Usage:\n"\
"   -h,--help                   display help/options menu (you are here)\n"\
"   -f,-r <ROM file name>       open <ROM file name> at startup\n"\
"   -p,--pause                  start paused\n"\
"   -m,--mute                   start muted\n"\
"   --showfps                   start with the framerate counter enabled\n"\
"   --rendertime=[ms|percent]   start showing the time in ms or percentage of a frame it takes to render a frame\n"
// TODO -l for dumping state information to log after every instruction
//      (make sure it's possible (necessary?) to pipe to txt file)
// TODO -d for dumping ROM info (hex values, header info, etc.)


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

    // TODO rework this into -l and -d
    // Set up clog
    std::ofstream log_stream("./log/nestest.log");
    if (!log_stream.is_open())
    {
        std::cout << "Error: could not open ./log/nestest.log" << std::endl;
        throw std::exception();
    }
    std::clog.rdbuf(log_stream.rdbuf());
    #endif

    // Argument parsing
    std::vector<std::string_view> args = {};
    args.resize(argc);
    for(int i = 0; i < argc; i++) args[i] = argv[i];

    // Print program info for -h or --help
    if (hasOpt(args, "-h") || hasOpt(args, "--help"))
    {
        std::cout << LICENSE_STRING << USAGE_STRING;
        exit(EXIT_SUCCESS);
    }

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
            getline(std::cin, rom_filename.value());
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
    std::shared_ptr<Config> config = std::make_shared<Config>();
    std::unique_ptr<NES> nes = std::make_unique<NES>(*audio, *video, config);

    nes->insertROM(rom, rom_filename.value());
    // For now, input has to be initialized after a ROM is loaded
    // This is because controllers don't get initialized until after ROM loading
    // TODO handle binds after Input is initialized and/or initialize controllers at console initialization
    std::shared_ptr<Input> input = std::make_shared<Input>(*nes, *audio, *video, config);

    // Command line args for emulation startup behavior
    if (hasOpt(args, "-p") || hasOpt(args, "--pause")) input->pause();
    if (hasOpt(args, "-m") || hasOpt(args, "--mute"))  input->mute();
    if (hasOpt(args, "--showfps")) input->toggle_fps(true);
    if (hasOpt(args, "--rendertime=ms")) input->toggle_render_time(Video::RenderTimeDisplay::MS);
    if (hasOpt(args, "--rendertime=percent")) input->toggle_render_time(Video::RenderTimeDisplay::PERCENT);

    std::thread cmd_in_thread = std::thread(CMD::cmdLoop, input);

    bool running = true;
    while (running)
    {
        using namespace std::chrono;
        time_point start_time = std::chrono::steady_clock::now();

        running = input->poll();
        nes->run(Scheduler::FRAME);
        video->displayFrame();

        // Framerate/timing
        auto now = std::chrono::steady_clock::now();
        auto render_time = duration_cast<nanoseconds>(now - start_time);
        if (now - start_time > frame(1))
        {
            video->updateFramerate(static_cast<float>(render_time.count()));
        }
        else
        {
            std::this_thread::sleep_until(start_time + frame(1));
            video->updateFramerate(duration_cast<nanoseconds>(frame(1)).count());
        }
        video->updateRenderTime(static_cast<float>(render_time.count()));
    }
    cmd_in_thread.detach(); // Thread not joinable if blocked by cin
    std::cout << std::endl;

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