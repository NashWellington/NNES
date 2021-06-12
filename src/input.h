#pragma once

#include "globals.h"
#include "display.h"
#include "bus.h"

class Input
{
public:
    void pollInputs(RunFlags& run_flags);

    ubyte joypad_1 = 0;
};

extern Input input;