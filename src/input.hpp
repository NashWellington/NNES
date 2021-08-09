#pragma once

#include "globals.hpp"
#include "console.hpp"
#include "audio.hpp"
#include "video.hpp"
#include "peripheral.hpp"

#include <map>
#include <algorithm>


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
    void pause();
    void mute();
    void toggle_fps();
    void toggle_render_time();
    void reset();
    
private:
    void loadBinds();
// Input polling methods
    void pollKeyboard(SDL_Event& event);
    void pollControllers();

    bool running = true;

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