#pragma once

#include "globals.h"
#include "savestate.h"
#include "util.h"

#ifdef DEBUGGER
#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_sdl.h"
#include "libs/imgui/imgui_impl_opengl3.h"
#endif

#include <GL/glew.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>

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

struct Font
{
public:
    Font() {}
    Font(FT_Library* ft, std::string family, std::string style);
    void renderText(GLuint shader, std::string text, float x, float y, float scale);

private:
    struct Character
    {
        uint texture_id = 0;
        glm::ivec2 size = {};
        glm::ivec2 bearing = {};
        uint advance = 0;
    };

    std::array<Character, 128> characters = {};
};

struct Shader
{
public:
    Shader() {}
    // TODO pass pathnames and load source from glsl file
    Shader(const char* vert_source, const char* frag_source, int total_attr_size);
    void use();
    void addAttribute(const char* name, int attr_size);
    void transform(const char* name, glm::mat4* trans);

private:
    GLuint shader_program = 0;
    int attr_index = 0;
    int total_attr_size = 0;
};

struct Texture
{
public:
    Texture() {}
    Texture(int width, int height, GLint filter);
    void update(int width, int height, void* pixels);
    int width = 0;
    int height = 0;
    GLuint texture = 0;
};

class Display
{
public:
    Display();
    ~Display();

    void displayFrame(RunFlags& run_flags);

#ifdef DEBUGGER
    int palette_selected = 0;
    uword mem_address = 0x8000; // Address in memory to be displayed by the "Memory" window
    enum
    {
        FOLLOW_PC,              // Starts at the address pointed to by the Program Counter
        SELECT_ADDRESS          // Starts at the address selected in the Memory window
    } disassemble_mode = FOLLOW_PC;

// OGL textures
    Texture pt_tex[2];   // pattern table textures
    Texture nt_tex[4];   // nametable textures
    Texture pal_tex[8];  // Palette textures
    Texture spr_tex;     // Sprite textures
#endif
    Texture frame_tex;

private:
// OGL variables
    float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    Shader frame_shader;
    Shader text_shader;

// SDL variables
    SDL_WindowFlags window_flags = {};
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = {};

// FreeType variables
    Font roboto_black;
};

extern Display display;