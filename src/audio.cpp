#include "audio.h"

const uint AUDIO_DELAY_MS = 40;

Audio::Audio()
{
    // Do all SDL audio init stuff
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "Error: failed to initialize SDL:" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    
    SDL_AudioSpec desired;
    desired.freq = sample_rate;
    desired.format = AUDIO_F32SYS; // We want a float so we don't have to do type conversions
    desired.channels = 1;
    desired.samples = 2048; // buffer size
    desired.callback = NULL;

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
}

Audio::~Audio()
{
    SDL_CloseAudioDevice(id);
    SDL_Quit();
}

void Audio::pushSample(float sample)
{
    // TODO type conversions if needed
    SDL_QueueAudio(id, &sample, sizeof(spec.format));

    SDL_AudioStatus status = SDL_GetAudioDeviceStatus(id);
    if (status == SDL_AUDIO_STOPPED)
    {
        std::cerr << "SDL Audio stopped" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (status == SDL_AUDIO_PAUSED 
        && (SDL_GetQueuedAudioSize(id) >= (spec.samples * AUDIO_DELAY_MS / 1000)))
    {
        // start playing audio once buffer has 1024 samples
        SDL_PauseAudioDevice(id, 0);
    }
}