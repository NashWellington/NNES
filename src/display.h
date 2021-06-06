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
    uint palette_selected = 0;
    int pt_selected = 0;
    GLuint pt_tex[2]; // pattern table textures
    GLuint nt_tex[4]; // nametable textures

    void addPatternTable(ubyte* pt, int pt_i);

    void addNametable(ubyte* nt, int nt_i);

    /* Adds an element to the display
    * params:
    *       - width = width of the element (px)
    *       - height = height of the element (px)
    *       - x = relative horizontal position (-1 to 1)
    *       - y = relative vertical position (-1 to 1)
    *       - texture = pixel matrix to be displayed
    */
    //static void addElement(GLint width, GLint height, GLfloat x, GLfloat y, ubyte* texture);

    // Add a palette and highlight if it is selected
    //static void addPalette(std::array<Pixel,4>* palette, uint palette_index);
    #endif

    // TODO deprecate
    //void processFrame(std::array<std::array<Pixel, 256>, 240>* frame);
private:
// SDL variables
    SDL_WindowFlags window_flags;
    SDL_Window* window;
    SDL_GLContext gl_context;
// ImGui variables
    std::unique_ptr<ImGuiIO> io;
    ImVec4 clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
// Helper methods
};

extern Display display;