#include "input.hpp"

#define CONTROL_CONFIG "./cfg/controls.cfg"

Input::Input(Console& _console, Audio&_audio, Video& _video)
    : console(_console), audio(_audio), video(_video)
{
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cerr << "Failed to init SDL Game Controller API" << std::endl;
        throw std::exception();
    }
    
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            joypads.push_back(SDL_GameControllerOpen(i));
            if (!joypads[i])
                std::cerr << "Could not open game controller " << i << std::endl;
        }
    }

    loadBinds(CONTROL_CONFIG);
}

Input::~Input()
{
    for (uint i = 0; i < joypads.size(); i++)
    {
        if (joypads[i])
            SDL_GameControllerClose(joypads[i]);
    }
}

void Input::loadBinds(std::string config)
{
    std::ifstream config_file(config);
    //if(!config_file.is_open())
    //{
    //    std::cerr << "Config file not found: " << config << std::endl;
    //    throw std::exception();
    //}
    // TODO actually load these from a file
    // Emulation controls
    emu_binds.insert({SDLK_ESCAPE,
        [this](){ quit(); }
    });
    emu_binds.insert({SDLK_SPACE,
        [this]() { pause(); }
    });
    emu_binds.insert({SDLK_m,
        [this]() { mute(); }
    });

    emu_binds.insert({SDLK_r, 
        [this]() { reset(); }
    });
    
    // Game Input controls
    std::vector<std::shared_ptr<Controller>> controllers = console.controllers;
    assert(controllers.size() > 0);
    key_binds.insert({SDLK_v, Bind(0, controllers[0])}); // V -> A
    key_binds.insert({SDLK_c, Bind(1, controllers[0])}); // C -> B
    key_binds.insert({SDLK_z, Bind(2, controllers[0])}); // Z -> Select
    key_binds.insert({SDLK_x, Bind(3, controllers[0])}); // X -> Start
    key_binds.insert({SDLK_w, Bind(4, controllers[0])}); // W -> Up
    key_binds.insert({SDLK_s, Bind(5, controllers[0])}); // S -> Down
    key_binds.insert({SDLK_a, Bind(6, controllers[0])}); // A -> Left
    key_binds.insert({SDLK_d, Bind(7, controllers[0])}); // D -> Right

    
    for (uint i = 0; i < joypads.size() && i < controllers.size(); i++)
    {
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_B, i},          Bind(0, controllers[i])}); // X-input B = right button  -> A
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_A, i},          Bind(1, controllers[i])}); // X-input A = bottom button -> B
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_BACK, i},       Bind(2, controllers[i])}); // X-input Back -> Select
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_START, i},      Bind(3, controllers[i])});
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_DPAD_UP, i},    Bind(4, controllers[i])});
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_DPAD_DOWN, i},  Bind(5, controllers[i])});
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_DPAD_LEFT, i},  Bind(6, controllers[i])});
        joypad_binds.insert({{SDL_CONTROLLER_BUTTON_DPAD_RIGHT, i}, Bind(7, controllers[i])});
    }
    config_file.close();
}

void Input::quit()
{
    running = false;
}

void Input::pause()
{
    console.paused = !console.paused;
    video.paused = !video.paused;
}

void Input::mute()
{
    audio.audio_buffer.muted = !audio.audio_buffer.muted;
    video.muted = !video.muted;
}

void Input::reset()
{
    console.reset();
}

void Input::pollControllers()
{
    for (int button_i = 0; button_i < static_cast<int>(SDL_CONTROLLER_BUTTON_MAX); button_i++)
    {
        for (uint joypad_i = 0; joypad_i < joypads.size(); joypad_i++)
        {
            if (joypads[joypad_i])
            {
                bool pressed = SDL_GameControllerGetButton(joypads[joypad_i], static_cast<SDL_GameControllerButton>(button_i));
                if (pressed)
                {
                    auto bind_iter = joypad_binds.find({static_cast<SDL_GameControllerButton>(button_i), joypad_i});
                    if (bind_iter != joypad_binds.end()) bind_iter->second.press(true, false);
                }
            }
        }
    }
}

void Input::pollKeyboard(SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event.key.keysym.sym;
        auto emu_bind_iter = emu_binds.find(key);
        if (emu_bind_iter != emu_binds.end()) 
            emu_bind_iter->second();

        auto key_bind_iter = key_binds.find(key);
        if (key_bind_iter != key_binds.end())
            key_bind_iter->second.press(true, true);
    }
    else if (event.type == SDL_KEYUP)
    {
        auto key_bind_iter = key_binds.find(event.key.keysym.sym);
        while (key_bind_iter != key_binds.end())
        {
            key_bind_iter->second.press(false, true);
            key_bind_iter++;
        }
    }
    else if (event.type == SDL_QUIT)
    {
        quit();
    }
}

bool Input::poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        pollKeyboard(event);
    }
    pollControllers();
    console.processInputs();
    return running;
}