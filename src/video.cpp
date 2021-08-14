#include "video.hpp"

// TODO move these to their own files
const char* frame_vert_source = 
R"glsl(
    #version 330 core

    in vec2 position;
    in vec2 texCoord;
    out vec2 TexCoord;

    uniform mat4 transformation;

    void main()
    {
        gl_Position = transformation * vec4(position, 0.0, 1.0);
        TexCoord = texCoord;
    }
)glsl";

const char* frame_frag_source = 
R"glsl(
    #version 330 core
    
    in vec2 TexCoord;
    out vec4 outColor;

    uniform sampler2D tex;

    void main()
    {
        outColor = texture(tex, TexCoord);
    }
)glsl";

// TODO move text shader to own file
const char* text_vert_source = 
R"glsl(
    #version 330 core

    in vec2 position;
    in vec2 texCoords;
    out vec2 TexCoords;
    
    uniform mat4 transformation;

    void main()
    {
        gl_Position = transformation * vec4(position, 0.0, 1.0);
        TexCoords = texCoords;
    }

)glsl";

const char* text_frag_source =
R"glsl(
    #version 330 core

    in vec2 TexCoords;
    out vec4 color;

    uniform sampler2D text;

    void main()
    {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
        color = vec4(1.0, 1.0, 1.0, 1.0) * sampled;
    }
)glsl";

const GLfloat vertices[] = 
{
    -1.0f,  1.0f,    0.0f, 0.0f,      // Top left
     1.0f,  1.0f,    1.0f, 0.0f,      // Top right
     1.0f, -1.0f,    1.0f, 1.0f,      // Bottom right
    -1.0f, -1.0f,    0.0f, 1.0f       // Bottom left
};

const GLuint indices[] = 
{
    0, 1, 2,
    2, 3, 0
};


Font::Font(FT_Library* ft, std::string family, std::string style)
{
    // Load font from file
    std::string path = "./fonts/";
    path += family + '/';
    path += family + '-' + style;
    path += ".ttf";
    FT_Face face;
    if (FT_New_Face(*ft, path.c_str(), 0, &face))
    {
        std::cerr << "Error loading font: " << family << ' ' << style << std::endl;
        throw std::exception();
    }
    FT_Set_Pixel_Sizes(face, 0, 64);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load all char glyphs as bmps
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "Error loading char " << c <<  " from font " << family << ' ' << style << std::endl;
            throw std::exception();
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(face->glyph->bitmap.width), static_cast<GLsizei>(face->glyph->bitmap.rows), 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = 
        {
            texture,
            glm::fvec2(static_cast<float>(face->glyph->bitmap.width),   static_cast<float>(face->glyph->bitmap.rows)),
            glm::fvec2(static_cast<float>(face->glyph->bitmap_left),    static_cast<float>(face->glyph->bitmap_top)),
            glm::fvec2(static_cast<float>(face->glyph->advance.x >> 6), static_cast<float>(face->glyph->advance.y >> 6)),
            static_cast<float>(face->height >> 6),
            static_cast<bool>(FT_HAS_VERTICAL(face))
        };
        characters[c] = character;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    FT_Done_Face(face);
}

void Font::renderText(
    Shader& shader, 
    std::string text, 
    float window_w, 
    float window_h, 
    float x, 
    float y, 
    float scale, 
    Alignment alignment)
{
    assert(alignment == Alignment::TOP_LEFT || alignment == Alignment::TOP_RIGHT);
    shader.use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    y -= characters[text[0]].advance.y / 2.0f;
    if (alignment == Alignment::TOP_LEFT)
    {
        for (auto iter = text.begin(); iter != text.end(); iter++)
        {
            Character c = characters[*iter];

            float c_x = x + (c.size.x/2.0f + c.bearing.x) * scale;
            float c_y = y - (2.0f*c.height - c.size.y/2.0f) * scale;

            float c_w = c.size.x * scale;
            float c_h = c.size.y * scale;

            glBindTexture(GL_TEXTURE_2D, c.texture_id);

            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(c_x*2.0f / window_w - 1.0f, c_y*2.0f / window_h - 1.0f, 0.0f));
            trans = glm::scale(trans, glm::vec3(c_w / window_w, c_h / window_h, 0.0f));
            shader.transform("transformation", &trans);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            x += c.advance.x * scale;
        }
    }
    else
    {
        for (auto iter = text.rbegin(); iter != text.rend(); iter++)
        {
            Character c = characters[*iter];
            
            x -= c.advance.x * scale;
            float c_x = x + (c.size.x/2.0f + c.bearing.x) * scale;
            float c_y = y - (2.0f*c.height - c.size.y/2.0f) * scale;

            float c_w = c.size.x * scale;
            float c_h = c.size.y * scale;

            glBindTexture(GL_TEXTURE_2D, c.texture_id);

            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(c_x*2.0f / window_w - 1.0f, c_y*2.0f / window_h - 1.0f, 0.0f));
            trans = glm::scale(trans, glm::vec3(c_w / window_w, c_h / window_h, 0.0f));
            shader.transform("transformation", &trans);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

Shader::Shader(const char* vert_source, const char* frag_source, int _total_attr_size)
: total_attr_size(_total_attr_size)
{
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_source, NULL);
    glCompileShader(vert_shader);
    GLenum error;
    if ((error = glGetError()))
    {
        std::cerr << "Error compiling vertex shader" << std::endl;
        throw std::exception();
    }
    GLint status;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &status);
    char buffer[512];
    glGetShaderInfoLog(vert_shader, 512, NULL, buffer);
    if (buffer[0] != '\0')
    {
        std::cerr << "Vertex shader log: " << std::endl;
        std::cerr << buffer << std::endl;
        throw std::exception();
    }

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_source, NULL);
    glCompileShader(frag_shader);
    if ((error = glGetError()))
    {
        std::cerr << "Error compiling vertex shader" << std::endl;
        throw std::exception();
    }
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &status);
    glGetShaderInfoLog(frag_shader, 512, NULL, buffer);
    if (buffer[0] != '\0')
    {
        std::cerr << "Fragment shader Log:" << std::endl;
        std::cerr << buffer << std::endl;
        throw std::exception();
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);

    glLinkProgram(shader_program);
}

void Shader::use()
{
    glUseProgram(shader_program);
}

void Shader::addAttribute(const char* name, int attr_size)
{
    assert(attr_index + attr_size <= total_attr_size);
    GLint attribute = glGetAttribLocation(shader_program, name);
    glVertexAttribPointer(attribute, attr_size, GL_FLOAT, GL_TRUE, total_attr_size * sizeof(float), reinterpret_cast<void*>(attr_index * sizeof(float)));
    glEnableVertexAttribArray(attribute);
    attr_index += attr_size;
}

void Shader::transform(const char* name, glm::mat4* trans)
{
    uint transformLoc = glGetUniformLocation(shader_program, name);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*trans));
}

Texture::Texture(int _width, int _height, GLint filter)
: width(_width), height(_height)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::update(int _width, int _height, void* pixels)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    width = _width;
    height = _height;
}

Video::Video()
{
    // Setup SDL
    // TODO bring back timer?
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to init SDL Video" << std::endl;
        throw std::exception();
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); // TODO research
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // TODO research
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // TODO research
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // TODO research
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // TODO research
    window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI); // TODO research DPI
    window = SDL_CreateWindow("NNES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags); // TODO change title to "NNES - {game title}"
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        throw std::exception();
    }

    // Setup OpenGL
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // TODO experiment w/ dynamic

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "Error: could not init FreeType" << std::endl;
        throw std::exception();
    }
    roboto_black = Font(&ft, {"Roboto"}, {"Black"});
    // Other fonts if used
    FT_Done_FreeType(ft);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    frame_shader = Shader(frame_vert_source, frame_frag_source, 4);
    frame_shader.addAttribute("position", 2);
    frame_shader.addAttribute("texCoord", 2);

    text_shader = Shader(text_vert_source, text_frag_source, 4);
    text_shader.addAttribute("position", 2);
    text_shader.addAttribute("texCoord", 2);

    frame_tex = Texture(256, 240, GL_NEAREST);
}

Video::~Video()
{
    // TODO destroy textures?

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
}

void Video::displayFrame()
{
    int window_w, window_h = 0;
    SDL_GetWindowSize(window, &window_w, &window_h);
    glViewport(0, 0, window_w, window_h);
    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw frame
    frame_shader.use();
    glm::mat4 trans = glm::mat4(1.0f);
    float frame_w = static_cast<float>(window_h) * 256.0f / 240.0f;
    float x_scale = frame_w / static_cast<float>(window_w);
    trans = glm::scale(trans, glm::vec3(x_scale, 1.0f, 0.0f));
    frame_shader.transform("transformation", &trans);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindTexture(GL_TEXTURE_2D, frame_tex.texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Render text
    float y_offset = 0.0f; // Offset from the top (increments for each line of text)
    if (paused)
    {
        roboto_black.renderText(
            text_shader, 
            {"PAUSED"}, 
            static_cast<float>(window_w), 
            static_cast<float>(window_h), 
            static_cast<float>(window_w), 
            static_cast<float>(window_h)-y_offset, 
            static_cast<float>(window_h)/1500.0f, 
            Alignment::TOP_RIGHT);
        frame_shader.use();
        y_offset += static_cast<float>(window_h)/20.0f;
    }
    else
    {
        if (show_framerate)
        {
            roboto_black.renderText(
                text_shader,
                fmt::format("{:.2f} FPS", framerate),
                static_cast<float>(window_w),
                static_cast<float>(window_h),
                static_cast<float>(window_w),
                static_cast<float>(window_h)-y_offset,
                static_cast<float>(window_h)/1500.0f,
                Alignment::TOP_RIGHT);
            frame_shader.use();
            y_offset += static_cast<float>(window_h)/20.0f;
        }
        if (show_render_time != RenderTimeDisplay::NO)
        {
            std::string rt_fmt_str;
            if (show_render_time == RenderTimeDisplay::MS)
            {
                rt_fmt_str = "{:.2f} ms";
                render_time /= 1'000'000.0f;
            }
            else
            {
                rt_fmt_str = "{:.2f}  %";
            }
            roboto_black.renderText(
                text_shader,
                fmt::format(rt_fmt_str, render_time),
                static_cast<float>(window_w),
                static_cast<float>(window_h),
                static_cast<float>(window_w),
                static_cast<float>(window_h)-y_offset,
                static_cast<float>(window_h)/1500.0f,
                Alignment::TOP_RIGHT);
            frame_shader.use();
            y_offset += static_cast<float>(window_h)/20.0f;
        }
    }
    if (muted)
    {
        roboto_black.renderText(
            text_shader, 
            {"MUTED"}, 
            static_cast<float>(window_w), 
            static_cast<float>(window_h), 
            static_cast<float>(window_w), 
            static_cast<float>(window_h)-y_offset, 
            static_cast<float>(window_h)/1500.0f, 
            Alignment::TOP_RIGHT);
        frame_shader.use();
    }

    // Render window
    SDL_GL_SwapWindow(window);
}

void Video::updateFramerate(float time)
{
    framerate = 60.0f * std::chrono::duration_cast<std::chrono::nanoseconds>(frame(1)).count() / time;
}

void Video::updateRenderTime(float time)
{
    using namespace std::chrono;
    if (show_render_time == RenderTimeDisplay::PERCENT)
    {
        render_time = time / static_cast<float>(duration_cast<nanoseconds>(frame(1)).count());
        render_time *= 100.0f;
    }
    else
    {
        render_time = time;
    }
}