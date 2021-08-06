#pragma once

#include "globals.hpp"
#include "savestate.hpp"
#include "util.hpp"

#include <numeric>
#include <sstream>

#include <GL/glew.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>

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

enum class Alignment
{
    LEFT,
    RIGHT,
    CENTER,
    TOP,
    BOTTOM,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_RIGHT,
    BOTTOM_LEFT
};

struct Font
{
public:
    Font() {}
    Font(FT_Library* ft, std::string family, std::string style);
    void renderText(Shader& shader, std::string text, 
        float window_w, float window_h, float x, float y, float scale, 
        Alignment alignment);

private:
    struct Character
    {
        uint texture_id = 0;
        glm::fvec2 size = {};
        glm::fvec2 bearing = {};
        glm::fvec2 advance = {};
        float height = 0.0f;
        bool has_vertical = true;
    };

    std::array<Character, 128> characters = {};
    GLuint font_vbo = 0;
};

class Video
{
public:
    Video();
    ~Video();

    void displayFrame();
    void updateFramerate(float time);
    void updateRenderTime(float time);

    Texture frame_tex;

    bool paused = false;
    bool muted = false;
    bool show_framerate = false;
    enum class RenderTimeDisplay
    {
        NO,
        MS,
        PERCENT
    } show_render_time = RenderTimeDisplay::NO;
private:
// OGL variables
    float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    Shader frame_shader;
    Shader text_shader;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

// SDL variables
    SDL_WindowFlags window_flags = {};
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = {};

// FreeType variables
    Font roboto_black;

// Other
    float framerate = 60.0f;

    /* Either:
    * - The time in ms it takes for the emulator to run for a frame
    * or
    * - The percentage of a frame that the emulator is run
    */
    float render_time = 0.0f;
};