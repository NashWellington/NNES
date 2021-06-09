#pragma once

#include "globals.h"
#include "savestate.h"
#include "util.h"

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_sdl.h"
#include "libs/imgui/imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>

struct RunFlags
{
    bool finished = false;  
    bool paused = false;    
    bool tick = false;      // If true, execute one cpu cycle (& 3 ppu cycles)
    bool step = false;      // If true, execute one cpu instruction
    bool frame = false;
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

    // Returns true if window close
    void pollEvents(RunFlags& run_flags);
    void displayFrame(RunFlags& run_flags);
    void renderFrame(ubyte* frame, int width, int height);

    #ifndef NDEBUG
    int palette_selected = 0;
    int spr_h = 8;              // sprite height. 8 for 8x8 sprites, 16 for 8x16
    uword mem_address = 0x8000; // Address in memory to be displayed by the "Memory" window

    void addPatternTable(ubyte* pt, int pt_i);
    void addNametable(ubyte* nt, int nt_i);
    void addPalette(ubyte* pal, int pal_i);
    void addSprites(ubyte* sprites, int spr_height);
    #endif
private:
// OGL textures
#ifndef NDEBUG
    GLuint pt_tex[2];   // pattern table textures
    GLuint nt_tex[4];   // nametable textures
    GLuint pal_tex[8];  // Palette textures
    GLuint spr_tex;     // Sprite textures
#endif
    GLuint frame_tex;
// SDL variables
    SDL_WindowFlags window_flags;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext gl_context;
// ImGui variables
    ImVec4 clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
};

extern Display display;