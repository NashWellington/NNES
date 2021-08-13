#pragma once

#include "globals.hpp"
#include "console.hpp"
#include "audio.hpp"
#include "video.hpp"
#include "peripheral.hpp"

#include <map>
#include <algorithm>

// For multithreading w/ the cin thread
#include <mutex>
#include <condition_variable>

// Used to bind a key press/joypad button press to a console input
struct Bind
{
    Bind(uint _button_id, std::shared_ptr<Controller> _controller) 
        : button_id(_button_id), controller(_controller) {}
    uint button_id;
    std::shared_ptr<Controller> controller;
    void press(bool pressed, bool keyboard)
    {
        assert(!(!pressed && !keyboard));
        if (keyboard) controller->keyboardActivate(button_id, pressed);
        else controller->joypadActivate(button_id);
    }
};

class Input
{
public:
    Input(Console& _console, Audio&_audio, Video& _video, std::shared_ptr<Config> _config);
    ~Input();
    bool poll();

// Emulator control methods
    void quit();
    bool pause();   // Return true if paused
    bool mute();    // Return true if muted
    void toggle_fps();
    void toggle_fps(bool show);
    void toggle_render_time();
    void toggle_render_time(bool show);
    void toggle_render_time(Video::RenderTimeDisplay opt);
    void reset();

    bool running = true;
    bool ready();
    void ready(bool r);
    bool is_ready = false; // Prevents cmdLoop from doing things when it shouldn't
    std::mutex mtx;     // Used for emulator control methods
    std::mutex mtx_2;   // Used to set/clear/read is_ready
    std::condition_variable cv;
private:
    void loadBinds();
// Input polling methods
    void pollKeyboard(SDL_Event& event);
    void pollControllers();

    Console& console;
    Audio& audio;
    Video& video;
    std::shared_ptr<Config> config;

    // ifdef SDL
    std::map<SDL_Keycode,std::function<void()>> emu_binds = {};
    std::map<SDL_Keycode,Bind> key_binds = {};
    std::map<std::pair<SDL_GameControllerButton,uint>,Bind> joypad_binds = {};
    // endif

    std::vector<SDL_GameController*> joypads;
};