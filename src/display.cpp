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

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = std::make_unique<ImGuiIO>(ImGui::GetIO());
    (void)io; // TODO how and why
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();                                   // Dark theme

    // Setup backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // TODO fonts?

    // Stats
    // TODO use
    //ImVec4 clear_color = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);

    /*

    // Vertex Array Object creation
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex Buffer Object creation
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // TODO experiment w/ dynamic

    // Element Buffer Object creation
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex shader compilation
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Vertex shader error handling
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

    // Fragment shader compilation
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Fragment shader error handling
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

    // Combine shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link and use shader programs
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Link vertex data and attributes
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    // Link texture data and attributes
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

    // Texture
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    */
}

// TODO add keyboard input
bool Display::pollEvents()
{
    bool done = false;
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) // TODO figure out the diff btwn these
            done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            done = true;
    }
    return done;
}

void Display::displayFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    // TODO remove later
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context");

    // Nametables
    {
        ImGui::Begin("Nametables");
        ImGui::Text("Test text");
        ImGui::End();
    }

    // Pattern tables

    // Palette info/selection

    // Render
    ImGui::Render();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
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

// // TODO deprecate
// void Display::addPalette(std::array<Pixel,4>* palette, uint palette_index)
// {
//     if (palette_index == palette_selected)
//     {
//         // TODO highlight
//     }
//     glWindowPos2i(20 * (palette_index + 1), 5); // TODO figure out where to place
//     GLint iViewport[4];
//     glGetIntegerv(GL_VIEWPORT, iViewport);
//     glPixelZoom(iViewport[2]/128.0, iViewport[3]/128.0);
//     glDrawPixels(4, 1, GL_RGB, GL_UNSIGNED_BYTE, palette);
// }

// void Display::addElement(GLint width, GLint height, GLfloat x, GLfloat y, ubyte* texture)
// {
//     // TODO draw image at (x,y)
//     x = y;
//     y = x;
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
// }

// // TODO deprecate
// void Display::processFrame(std::array<std::array<Pixel, 256>, 240>* frame)
// {   
//     // TODO move back?

//     // Clear screen
//     glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT);

//     // TODO switch to textured quads
//     // Draw pixels
//     glWindowPos2i(0,0);
//     GLint iViewport[4];
//     glGetIntegerv(GL_VIEWPORT, iViewport);
//     glPixelZoom(iViewport[2]/256.0, iViewport[3]/240.0);
//     glDrawPixels(256, 240, GL_RGB, GL_UNSIGNED_BYTE, frame);
// }