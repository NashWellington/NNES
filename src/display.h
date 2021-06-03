#ifndef NNES_DISPLAY_H
#define NNES_DISPLAY_H

#include "globals.h"
#include <GL/glew.h>
#include <SFML/Window.hpp>

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

    #ifndef NDEBUG
    uint palette_selected = 0;

    /* Adds an element to the display
    * params:
    *       - width = width of the element (px)
    *       - height = height of the element (px)
    *       - x = relative horizontal position (-1 to 1)
    *       - y = relative vertical position (-1 to 1)
    *       - texture = pixel matrix to be displayed
    */
    void addElement(GLint width, GLint height, GLfloat x, GLfloat y, ubyte* texture);

    // Add a palette and highlight if it is selected
    void addPalette(std::array<Pixel,4>* palette, uint palette_index);

    #endif

    // TODO deprecate
    void processFrame(std::array<std::array<Pixel, 256>, 240>* frame);
    void displayFrame();

private:
    sf::Window* window;
};

#endif