#pragma once

#include "globals.hpp"

// TODO if I ever make multiple frontends, put an ifdef around this
#include <SDL.h>
#include <SDL_audio.h>
#include <mutex>
#include <condition_variable>

struct AudioBuffer
{
    AudioBuffer();
    void push(float sample);
    float pull();
    bool full();
    bool empty();
    uint size();
    bool finished = false;
    bool muted = false;
    std::unique_ptr<std::mutex> mtx;
    std::unique_ptr<std::condition_variable> cv;
    std::array<float,4096> buffer = {};
    uint head = 0;
    uint tail = 0;
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