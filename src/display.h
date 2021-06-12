#pragma once

#include "globals.h"
#include "savestate.h"
#include "util.h"

#ifdef DEBUGGER
#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_sdl.h"
#include "libs/imgui/imgui_impl_opengl3.h"
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>

struct RunFlags
{
    bool finished = false;  
    bool paused = false;

    #ifdef DEBUGGER
    bool tick = false;      // If true, execute one cpu cycle (& 3 ppu cycles)
    bool step = false;      // If true, execute one cpu instruction
    bool frame = false;     // Cycles until the end of the frame
    #endif
};

// TODO copy/move constructor?
struct Pixel
{
    Pixel() {}
    Pixel(ubyte red, ubyte green, ubyte blue) : r(red), g(green), b(blue) {}
    ubyte r = 0;
    ubyte g = 0;
    ubyte b = 0;
};

class Display
{
public:
    Display();
    ~Display();

    void displayFrame(RunFlags& run_flags);
    void addFrame(ubyte* frame, int width, int height);

    #ifdef DEBUGGER
    int palette_selected = 0;
    int spr_h = 8;              // sprite height. 8 for 8x8 sprites, 16 for 8x16
    uword mem_address = 0x8000; // Address in memory to be displayed by the "Memory" window
    enum
    {
        FOLLOW_PC,              // Starts at the address pointed to by the Program Counter
        SELECT_ADDRESS          // Starts at the address selected in the Memory window
    } disassemble_mode = FOLLOW_PC;

    void addPatternTable(ubyte* pt, int pt_i);
    void addNametable(ubyte* nt, int nt_i);
    void addPalette(ubyte* pal, int pal_i);
    void addSprites(ubyte* sprites, int spr_height);
    #endif
private:
// OGL textures
#ifdef DEBUGGER
    GLuint pt_tex[2];   // pattern table textures
    GLuint nt_tex[4];   // nametable textures
    GLuint pal_tex[8];  // Palette textures
    GLuint spr_tex;     // Sprite textures
#endif
    GLuint frame_tex;
// OGL variables
    float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};

// SDL variables
    SDL_WindowFlags window_flags;
    SDL_Window* window;
    SDL_GLContext gl_context;
};

extern Display display;