#include "display.h"

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
    // TODO look more into this
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    
    // TODO make these depend on something
    int width = 400;
    int height = 400;

    std::string title = "NNES";

    window = new sf::Window(sf::VideoMode(width, height), title, sf::Style::Default, settings);

    // Needed to set up GLEW
    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit())
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        throw std::exception();
    }

    /*
    // Frame Buffer creation
    GLuint fbo;
    glGenBuffers(1, &fbo);
    glBindBuffer(GL_FRAMEBUFFER, fbo);
    */

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
}

Display::~Display()
{
    delete window;
}

// TODO deprecate
void Display::addPalette(std::array<Pixel,4>* palette, uint palette_index)
{
    if (palette_index == palette_selected)
    {
        // TODO highlight
    }
    glWindowPos2i(20 * (palette_index + 1), 5); // TODO figure out where to place
    GLint iViewport[4];
    glGetIntegerv(GL_VIEWPORT, iViewport);
    glPixelZoom(iViewport[2]/128.0, iViewport[3]/128.0);
    glDrawPixels(4, 1, GL_RGB, GL_UNSIGNED_BYTE, palette);
}

void Display::addElement(GLint width, GLint height, GLfloat x, GLfloat y, ubyte* texture)
{
    // TODO draw image at (x,y)
    x = y;
    y = x;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
}

// TODO deprecate
void Display::processFrame(std::array<std::array<Pixel, 256>, 240>* frame)
{   
    // TODO move back?

    // Clear screen
    glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO switch to textured quads
    // Draw pixels
    glWindowPos2i(0,0);
    GLint iViewport[4];
    glGetIntegerv(GL_VIEWPORT, iViewport);
    glPixelZoom(iViewport[2]/256.0, iViewport[3]/240.0);
    glDrawPixels(256, 240, GL_RGB, GL_UNSIGNED_BYTE, frame);
}

void Display::displayFrame()
{    
    glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    window->display();

    #ifndef NDEBUG
    sf::Event event;
    while (window->pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window->close();
                exit(EXIT_SUCCESS);
                break;

            case sf::Event::Resized:
                glViewport(0, 0, event.size.width, event.size.height);
                break;
            
            default:
                return;
        }
    }
    /*
    #else
    sf::Event event;
    while (window->waitEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window->close();
                exit(EXIT_SUCCESS);
                break;

            case sf::Event::Resized:
                glViewport(0, 0, event.size.width, event.size.height);
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                    case sf::Keyboard::Space:
                        return;

                    case sf::Keyboard::Escape:
                        window->close();
                        return;

                    default:
                        break;
                }
            default:
                break;
        }
    }
    */
    #endif
}
