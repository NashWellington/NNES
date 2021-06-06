#pragma once

#include "globals.h"

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_sdl.h"
#include "libs/imgui/imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>

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
    bool pollEvents();
    void displayFrame();

    #ifndef NDEBUG
    int palette_selected = 0;
    GLuint pt_tex[2]; // pattern table textures // TODO move to private?
    GLuint nt_tex[4]; // nametable textures
    GLuint pal_tex[8]; // Palette textures

    void addPatternTable(ubyte* pt, int pt_i);
    void addNametable(ubyte* nt, int nt_i);
    void addPalette(ubyte* pal, int pal_i);
    #endif
private:
// SDL variables
    SDL_WindowFlags window_flags;
    SDL_Window* window;
    SDL_GLContext gl_context;
// ImGui variables
    std::unique_ptr<ImGuiIO> io;
    ImVec4 clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
};

extern Display display;