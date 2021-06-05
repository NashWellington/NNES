#pragma once

#include "globals.h"

class Savestate
{
public:
    Savestate();

};

extern std::vector<Savestate> savestates;

#ifndef NDEBUG
extern Savestate debug_state; // Current state of the emulated NNES
#endif