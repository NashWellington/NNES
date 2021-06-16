#include "display.h"

Display display;

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = 
        {
            texture,
            glm::fvec2(static_cast<float>(face->glyph->bitmap.width),   static_cast<float>(face->glyph->bitmap.rows)),
            glm::fvec2(static_cast<float>(face->glyph->bitmap_left),    static_cast<float>(face->glyph->bitmap_top)),
            glm::fvec2(static_cast<float>(face->glyph->advance.x >> 6), static_cast<float>(face->glyph->advance.y >> 6))
        };
        characters[c] = character;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    FT_Done_Face(face);
    //glGenBuffers(1, &font_vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, font_vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Font::renderText(Shader& shader, std::string text, float window_w, float window_h, float x, float y, float scale, bool align_left)
{
    shader.use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    y -= characters[text[0]].advance.y / 2.0f;
    if (align_left)
    {
        for (auto iter = text.begin(); iter != text.end(); iter++)
        {
            Character c = characters[*iter];

            float c_x = x + (c.size.x/2.0f + c.bearing.x) * scale;
            float c_y = y - (c.size.y/2.0f + (c.size.y - c.bearing.y)) * scale;

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
            float c_y = y - (c.size.y/2.0f + (c.size.y - c.bearing.y)) * scale;

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

Shader::Shader(const char* vert_source, const char* frag_source, int total_attr_size)
: total_attr_size(total_attr_size)
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

Texture::Texture(int width, int height, GLint filter)
: width(width), height(height)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::update(int width, int height, void* pixels)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    this->width = width;
    this->height = height;
}

Display::Display()
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cerr << "Failed to init SDL" << std::endl;
        throw std::exception();
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); // TODO research
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // TODO research
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // TODO research
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // TODO research
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // TODO research
    window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI); // TODO research DPI
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
    
    #ifdef DEBUGGER
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.IniFilename = "./cfg/imgui.ini";
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();                                    // Dark theme

    // Setup backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");
    #endif

    // TODO fonts?

    // TODO Stats?

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
#ifdef DEBUGGER
    for (int i = 0; i < 2; i++) pt_tex[i]  = Texture(128, 128, GL_NEAREST);
    for (int i = 0; i < 4; i++) nt_tex[i]  = Texture(256, 240, GL_NEAREST);
    for (int i = 0; i < 8; i++) pal_tex[i] = Texture(4, 1, GL_NEAREST);
    spr_tex = Texture(64, 64, GL_NEAREST);
#endif
}

Display::~Display()
{
    #ifdef DEBUGGER
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    #endif

    // TODO destroy textures?

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::displayFrame(RunFlags& run_flags)
{
#ifdef DEBUGGER
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();

    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context");

    //ImGuiWindowFlags wf = 0;
    //wf |= ImGuiWindowFlags_NoScrollbar;
    //wf |= ImGuiWindowFlags_NoMove;
    //wf |= ImGuiWindowFlags_NoResize;

    // Palettes
    {
        ImGui::Begin("Palettes");
        ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Image vals
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);
        ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border_unselected = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
        ImVec4 border_selected = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec2 size = ImVec2(58.0f, 14.5f);
        for (int i = 0; i < 8; i++)
        {
            if (i == palette_selected)
                ImGui::Image((ImTextureID)(intptr_t)pal_tex[i].texture, size, uv_min, uv_max, tint, border_selected);
            else
                ImGui::Image((ImTextureID)(intptr_t)pal_tex[i].texture, size, uv_min, uv_max, tint, border_unselected);
            ImGui::SameLine();
        }
        ImGui::End();
    }
    // Pattern Tables
    {
        ImGui::Begin("Pattern Tables");
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Image vals
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);
        ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
        for (int i = 0; i < 2; i++)
        {
            float width = static_cast<float>(2 * pt_tex[i].width);
            float height = static_cast<float>(2 * pt_tex[i].height);
            ImVec2 size = ImVec2(width, height);
            ImGui::Image((ImTextureID)(intptr_t)pt_tex[i].texture, size, uv_min, uv_max, tint, border);
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                float region_sz = 64.0f;
                float region_x = io.MousePos.x - pos.x - region_sz * 0.5f - width*i - 10.0f * i; // TODO find better val than 10
                float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                float zoom = 2.0f;
                if (region_x < 0.0f) region_x = 0.0f;
                else if (region_x > width - region_sz) region_x = width - region_sz;
                if (region_y < 0.0f) region_y = 0.0f;
                else if (region_y > height - region_sz) region_y = height - region_sz;
                ImVec2 uv0 = ImVec2((region_x) / width, (region_y) / height);
                ImVec2 uv1 = ImVec2((region_x + region_sz) / width, (region_y + region_sz) / height);
                ImGui::Image((ImTextureID)(intptr_t)pt_tex[i].texture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint, border);
                ImGui::EndTooltip();
            }
            ImGui::SameLine();
        }
        ImGui::End();
    }
    // Nametables
    {
        ImGui::Begin("Nametables");
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Image vals
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);
        ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
        for (int i = 0; i < 4; i++)
        {
            float width = static_cast<float>(nt_tex[i].width);
            float height = static_cast<float>(nt_tex[i].height);
            ImVec2 size = ImVec2(width, height);
            ImGui::Image((ImTextureID)(intptr_t)nt_tex[i].texture, size, uv_min, uv_max, tint, border);
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                float region_sz = 64.0f;
                float region_x = io.MousePos.x - pos.x - region_sz * 0.5f - width*(i%2) - 10.0f * (i%2);
                float region_y = io.MousePos.y - pos.y - region_sz * 0.5f - height*(i/2) - 10.0f * (i/2);
                float zoom = 2.0f;
                if (region_x < 0.0f) region_x = 0.0f;
                else if (region_x > width - region_sz) region_x = width - region_sz;
                if (region_y < 0.0f) region_y = 0.0f;
                else if (region_y > height - region_sz) region_y = height - region_sz;
                ImVec2 uv0 = ImVec2((region_x) / width, (region_y) / height);
                ImVec2 uv1 = ImVec2((region_x + region_sz) / width, (region_y + region_sz) / height);
                ImGui::Image((ImTextureID)(intptr_t)nt_tex[i].texture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint, border);
                ImGui::EndTooltip();
            }
            if (i%2 == 0) ImGui::SameLine();
        }
        ImGui::End();
    }
    // Sprites
    {
        ImGui::Begin("Sprites");
        ImVec2 uv_min = ImVec2(0.0f, 0.0f);
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);
        ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
        ImVec2 size = ImVec2(static_cast<float>(4 * spr_tex.width), static_cast<float>(4 * spr_tex.height)); // TODO
        ImGui::Image((ImTextureID)(intptr_t)spr_tex.texture, size, uv_min, uv_max, tint, border);
        ImGui::End();
    }
    // Registers
    {
        ImGui::Begin("Registers");
        ImGui::Text("Program Counter: %s", hex(debug_state.registers.reg_pc).c_str());
        ImGui::Text("Stack Pointer:     %s", hex(debug_state.registers.reg_sp).c_str());
        ImGui::Text("Accumulator:       %s", hex(debug_state.registers.reg_a).c_str());
        ImGui::Text("Register X:        %s", hex(debug_state.registers.reg_x).c_str());
        ImGui::Text("Register Y:        %s", hex(debug_state.registers.reg_y).c_str());
        ImGui::Text("Status Register:   %s", hex(debug_state.registers.reg_sr).c_str());
        ImGui::Text("NV BDIZC");
        ImGui::Text("%s", binary(debug_state.registers.reg_sr).c_str());
        ImGui::End();
    }
    // Disassembler
    {
        ImGui::Begin("Disassembler");
        if (ImGui::Button("Program Counter")) disassemble_mode = FOLLOW_PC;
        ImGui::SameLine();
        if (ImGui::Button("Select Address")) disassemble_mode = SELECT_ADDRESS;
        uword addr = 0; 
        if (disassemble_mode == FOLLOW_PC) addr = debug_state.registers.reg_pc;
        else if (disassemble_mode == SELECT_ADDRESS) addr = mem_address;
        int lines = 10;
        std::optional<std::string> line;
        for (int i = 0; i < lines; i++)
        {
            line = disassemble(addr);
            if (line)
            {
                if (i == 0) ImGui::Text("%-20s %s", line.value().c_str(), "<---");
                else ImGui::Text("%-20s", line.value().c_str());
            }
            else i = lines;
        }
        ImGui::Text("PPU cycle: %6i", debug_state.ppu_cycle);
        ImGui::Text("CPU cycle: %6i", debug_state.ppu_cycle/3);
    ImGui::End();
    }
    // Memory
    // TODO add tabs or buttons to switch between memory fields
    {
        ImGui::Begin("Memory");
        ImGui::Text("PRG-ROM:");
        uword addr = (mem_address & 0xFFF0) - 8 * 16;
        for (int i = 0; i < 16; i++) // Display 9 lines
        {
            if (addr < 0x8000) ImGui::Text(" ");
            else ImGui::Text("%s", peekMem(addr).c_str());
            addr += 16;
        }
        ImGui::End();
    }
    // ROM Info
    {
        ImGui::Begin("ROM Info");
        ImGui::Text("Filename: %s", debug_state.filename.c_str());
        ImGui::Text("Header: %8s", debug_state.header_type.c_str());
        ImGui::Text("Mapper:      %3i", debug_state.mapper);
        ImGui::Text("Submapper:   %3i", debug_state.submapper);
        ImGui::Text("PRG-ROM:     %3li KiB", debug_state.prg_rom_size/1024);
        ImGui::Text("PRG-RAM:     %3li KiB", debug_state.prg_ram_size/1024);
        if (debug_state.chr_rom_size != 0)
            ImGui::Text("CHR-ROM:     %3li KiB", debug_state.chr_rom_size/1024);
        if (debug_state.chr_ram_size != 0)
            ImGui::Text("CHR-RAM:     %3li KiB", debug_state.chr_ram_size/1024);
        ImGui::End();
    }

    // Render
    ImGui::Render();
#endif
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
#ifdef DEBUGGER
    float x_trans = (frame_w - static_cast<float>(window_w)) / (static_cast<float>(window_w));
    trans = glm::translate(trans, glm::vec3(x_trans, 0.0f, 0.0f));
#endif
    trans = glm::scale(trans, glm::vec3(x_scale, 1.0f, 0.0f));
    frame_shader.transform("transformation", &trans);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindTexture(GL_TEXTURE_2D, frame_tex.texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (run_flags.paused)
    {
        roboto_black.renderText(text_shader, {"PAUSED"}, (float)window_w, (float)window_h, (float)window_w, (float)window_h, (float)window_h/1000.0f, false);
        frame_shader.use();
    }

    // Draw ImGui elements
#ifdef DEBUGGER
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    SDL_GL_SwapWindow(window);
}