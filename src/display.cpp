#include "display.h"

Display display;

// TODO move these to their own files

const char* vertexShaderSource = 
R"glsl(
    #version 330 core

    in vec2 position;
    in vec2 texCoord;
    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
        TexCoord = texCoord;
    }
)glsl";

const char* fragmentShaderSource = 
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

const GLfloat vertices[] = 
{
    -1.0f,  1.0f,    0.0f, 0.0f,      // Top left
     1.0f,  1.0f,    1.0f, 0.0f,      // Top right
     1.0f, -1.0f,    1.0f, 1.0f,      // Bottom right
    -1.0f, -1.0f,    0.0f, 1.0f       // Bottom left
};

const GLint indices[] = 
{
    0, 1, 2,
    2, 3, 0
};

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
    window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP); // TODO research DPI
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

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = std::make_unique<ImGuiIO>(ImGui::GetIO());
    (void)io;
    io->IniFilename = NULL;
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();                                   // Dark theme

    // Setup backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // TODO fonts?

    // TODO Stats?

    // Setup OpenGL
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // TODO experiment w/ dynamic

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLenum error;
    if ((error = glGetError()))
    {
        std::cerr << "Error compiling vertex shader" << std::endl;
        throw std::exception();
    }
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    char buffer[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
    if (buffer[0] != '\0')
    {
        std::cout << "Vertex shader log: " << std::endl;
        std::cerr << buffer << std::endl;
        throw std::exception();
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    if ((error = glGetError()))
    {
        std::cerr << "Error compiling vertex shader" << std::endl;
        throw std::exception();
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
    if (buffer[0] != '\0')
    {
        std::cout << "Fragment shader Log:" << std::endl;
        std::cerr << buffer << std::endl;
        throw std::exception();
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

#ifndef NDEBUG
    glGenTextures(1, &pt_tex[0]);
    glBindTexture(GL_TEXTURE_2D, pt_tex[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &pt_tex[1]);
    glBindTexture(GL_TEXTURE_2D, pt_tex[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    for (int i = 0; i < 4; i++)
    {
        glGenTextures(1, &nt_tex[i]);
        glBindTexture(GL_TEXTURE_2D, nt_tex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

// TODO add keyboard input
bool Display::pollEvents()
{
    bool done = false;
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        done = true;
                        break;
                }
                break;
            case SDL_QUIT:
                done = true;
                break;
            default:
                break;
        }
    }
    return done;
}

Display::~Display()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

void Display::displayFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context");

#ifndef NDEBUG
    // TODO look into window resize constraints
    // Pattern Tables
    {
        ImGui::Begin("Pattern Tables");
        ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Image vals
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);
        ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // TODO border thickness
        ImVec2 size = ImGui::GetContentRegionAvail();
        size.x /= 2;
        ImGui::Image((void*)(intptr_t)pt_tex[0], size, uv_min, uv_max, tint, border);
        ImGui::SameLine();
        ImGui::Image((void*)(intptr_t)pt_tex[1], size, uv_min, uv_max, tint, border);
        // TODO palette selection
        ImGui::End();
    }

    // Nametables
    {
        ImGui::Begin("Nametables");
        ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Image vals
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);
        ImVec4 tint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // TODO border thickness
        ImVec2 size = ImGui::GetContentRegionAvail();
        size.x /= 2;
        size.y /= 2;
        //ImVec2 size = ImVec2(ImGui::GetWindowWidth()/2, ImGui::GetWindowHeight()/2); // TODO lower height by (title bar height + whatever else is in window)
        ImGui::Image((void*)(intptr_t)nt_tex[0], size, uv_min, uv_max, tint, border);
        ImGui::SameLine();
        ImGui::Image((void*)(intptr_t)nt_tex[1], size, uv_min, uv_max, tint, border);
        ImGui::Image((void*)(intptr_t)nt_tex[2], size, uv_min, uv_max, tint, border);
        ImGui::SameLine();
        ImGui::Image((void*)(intptr_t)nt_tex[3], size, uv_min, uv_max, tint, border);
        ImGui::End();
    }

    //TODO add (save)state info
#endif

    // Render
    ImGui::Render();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void Display::addPatternTable(ubyte* pt, int pt_i)
{
    assert((pt_i >= 0) && (pt_i < 2));
    glBindTexture(GL_TEXTURE_2D, pt_tex[pt_i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, pt);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Display::addNametable(ubyte* nt, int nt_i)
{
    assert((nt_i >= 0) && (nt_i < 4));
    glBindTexture(GL_TEXTURE_2D, nt_tex[nt_i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 240, 0, GL_RGB, GL_UNSIGNED_BYTE, nt);
    glBindTexture(GL_TEXTURE_2D, 0);
}