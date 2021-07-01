#pragma once

#include "globals.h"
#include "display.h"
#include "bus.h"

class Input
{
public:
    Input();
    ~Input();
    void pollInputs(RunFlags& run_flags);

private:
#ifdef DEBUGGER
    void pollDebug(RunFlags& run_flags, SDL_Event& event);
#endif
    void pollKeyboard(RunFlags& run_flags, SDL_Event& event);
    void pollControllers();

    std::array<SDL_GameController*, 4> controllers;
    std::array<ubyte, 2> joypads = {};
};

extern Input input;