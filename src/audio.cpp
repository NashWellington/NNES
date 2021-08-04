#include "audio.hpp"

Audio::Audio()
{
    // Do all SDL audio init stuff
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "Error: failed to initialize SDL:" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        throw std::exception();
    }

    audio_buffer = {};

    SDL_AudioSpec desired;
    desired.freq = sample_rate;
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
        throw std::exception();
    }
    if (desired.format != spec.format)
    {
        std::cerr << "Error: format isn't 32-bit float" << std::endl;
        throw std::exception();
    }
    sample_rate = spec.freq;
    SDL_PauseAudioDevice(id, 0);
}

Audio::~Audio()
{
    audio_buffer.finished = true;
    audio_buffer.cv->notify_all();
    SDL_PauseAudioDevice(id, 1);
    SDL_CloseAudioDevice(id);
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
    audio_buffer.push(sample);

    SDL_AudioStatus status = SDL_GetAudioDeviceStatus(id);
    if (status == SDL_AUDIO_STOPPED)
    {
        std::cerr << "SDL Audio stopped" << std::endl;
        std::cerr << SDL_GetError() << std::endl;
        throw std::exception();
    }
}

AudioBuffer::AudioBuffer()
{
    mtx = std::make_unique<std::mutex>();
    cv = std::make_unique<std::condition_variable>();
}

void AudioBuffer::push(float sample)
{
    std::unique_lock<std::mutex> lk(*mtx);
    while (full())
    {
        if (finished) return;
        cv->wait(lk);
    }
    buffer[head] = sample;
    head++;
    head %= 4096;
    if (size() >= 1024) cv->notify_one();
}

float AudioBuffer::pull()
{
    std::unique_lock<std::mutex> lk(*mtx);
    while (empty())
    {
        if (finished) return 0.0;
        cv->wait(lk);
    }
    float sample = muted ? 0.0f : buffer[tail];
    tail++;
    tail %= 4096;
    cv->notify_one();
    return sample;
}

bool AudioBuffer::full()
{
    return ((head+1) % 4096) == tail;
}

bool AudioBuffer::empty()
{
    return head == tail;
}

uint AudioBuffer::size()
{
    uint h = (head < tail) ? head+4096 : head;
    return h - tail;
}