#pragma once

#include "globals.h"

// TODO if I ever make multiple frontends, put an ifdef around this
#include <SDL.h>
#include <SDL_audio.h>

class Audio
{
public:
    Audio();
    ~Audio();
    void pushSample(float sample);

    int sample_rate = 44100;

private:
    // TODO ifdef
    SDL_AudioDeviceID id;
    SDL_AudioSpec spec;
};