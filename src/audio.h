#pragma once

#include "globals.h"

// TODO if I ever make multiple frontends, put an ifdef around this
#include <SDL.h>
#include <SDL_audio.h>

struct AudioBuffer
{
    std::array<float,4096> buffer = {};
    uint apu_index = 0;
    uint sdl_index = 0;
    void push(float sample);
    float pull();
};

class Audio
{
public:
    Audio();
    ~Audio();
    void pushSample(float sample);

    int sample_rate = 44100;
    AudioBuffer audio_buffer;

private:
    // TODO ifdef
    SDL_AudioDeviceID id;
    SDL_AudioSpec spec;

    static void callback(void* data, uint8_t* stream, int bytes);
};