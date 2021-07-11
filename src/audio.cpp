#include "audio.h"

Audio::Audio()
{
    // Do all SDL audio init stuff
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "Error: failed to initialize SDL:" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    audio_buffer = {};
    
    SDL_AudioSpec desired;
    desired.freq = sample_rate;
    // TODO switch to byte or ubyte or something
    desired.format = AUDIO_F32SYS; // We want a float so we don't have to do type conversions
    desired.channels = 1;
    desired.samples = 1024; // buffer size
    desired.callback = callback;
    desired.userdata = this;

    id = SDL_OpenAudioDevice(NULL, 0, &desired, &spec, 0);
    if (!id)
    {
        std::cerr << "Error: failed to open audio:" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    if (desired.format != spec.format)
    {
        // TODO save the used format so we can do type conversions
        std::cerr << "Error: format isn't 32-bit float" << std::endl;
        exit(EXIT_FAILURE);
    }
    sample_rate = spec.freq;
    SDL_PauseAudioDevice(id, 0);
}

Audio::~Audio()
{
    SDL_CloseAudioDevice(id);
    SDL_Quit();
}

void Audio::callback(void* data, uint8_t* stream, int bytes)
{
    uint samples = bytes / sizeof(float);
    float* buffer = reinterpret_cast<float*>(stream);
    Audio* audio = reinterpret_cast<Audio*>(data);

    for (uint i = 0; i < samples; i++)
    {
        buffer[i] = audio->audio_buffer.pull();
    }
}

void Audio::pushSample(float sample)
{
    // TODO type conversions if needed
    audio_buffer.push(sample);

    SDL_AudioStatus status = SDL_GetAudioDeviceStatus(id);
    if (status == SDL_AUDIO_STOPPED)
    {
        std::cerr << "SDL Audio stopped" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void AudioBuffer::push(float sample)
{
    buffer[apu_index] = sample;
    apu_index++;
    apu_index %= 4096;
    while (apu_index == sdl_index)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}

float AudioBuffer::pull()
{
    while (sdl_index == apu_index)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
    float sample = buffer[sdl_index];
    sdl_index++;
    sdl_index %= 4096;
    return sample;
}