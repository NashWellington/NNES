#include "globals.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "bus.h"
#include "boot.h"
//#include "input.h"
#include "display.h"

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

    /* Here's a general layout of how to get things rolling
    * 1. Open ROM file with an ifstream
    * 2. Initialize Bus()
    * 3. call Boot::loadRom
    *   - this reads the header, initializes a mapper and connects it to the bus
    * 4. Initialize Display
    *   - must be done before PPU
    * 5. Initialize CPU()
    * 6. Initialize PPU()
    * 6. Initialize APU() (once implemented)
    * 7. Do other potentially time-wasting initializations
    * 8. Use <chrono> to get a time point and save it as start_point
    * 9. Run main loop (described in design.txt)
    */

    // Open ROM file
    std::string rom_filename = argv[1];
    std::ifstream rom(rom_filename, std::ios::binary);
    if (!rom.is_open())
    {
        std::cerr << "File not found: " << rom_filename << std::endl;
        throw std::exception();
    }

    // Read .nes file and initialize bus, cartridge
    Boot::loadRom(rom);

    // Initialize display
    Display display = Display();

    // Initialize processors
    CPU cpu = CPU();
    PPU ppu = PPU(bus, display);
    //APU apu = APU();           // TODO fully implement APU

    IMGUI_CHECKVERSION(); // TODO is this necessary?

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); // TODO research
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // TODO research
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // TODO research
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // TODO research
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // TODO research
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI); // TODO research DPI
    SDL_Window* window = SDL_CreateWindow("NNES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags); // TODO change title to "NNES - {game title}"
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    #ifndef NDEBUG
    // TODO testing
    bool running = true;
    while (running) 
    {
        ppu.clock();
        ppu.clock();
        ppu.clock();
        cpu.clock();
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