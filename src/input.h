#pragma once

#include "globals.h"
#include "console.h"
#include "audio.h"
#include "video.h"
#include "peripheral.h"

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
    Input(Console& _console, Audio&_audio, Video& _video);
    ~Input();
    bool poll();

// Emulator/Debug control methods
    void quit();
    void pause(); // Toggles
    void mute();  // Toggles volume
private:
    void loadBinds(std::string config);
// Input polling methods
    void pollKeyboard(SDL_Event& event);
    void pollControllers();

    bool running = true;

    Console& console;
    // TODO shared_ptr to debugger object
    Audio& audio;
    Video& video;

    // ifdef SDL
    std::map<SDL_Keycode,std::function<void()>> emu_binds = {};
    #ifdef DEBUGGER
    std::multimap<SDL_Keycode,std::function<void()>> debug_binds = {};
    #endif
    std::map<SDL_Keycode,Bind> key_binds = {};
    std::map<std::pair<SDL_GameControllerButton,uint>,Bind> joypad_binds = {};
    // endif

    std::vector<SDL_GameController*> joypads;
};