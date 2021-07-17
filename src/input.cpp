#include "input.h"

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
            SDL_GameControllerClose(joypads[i]); // FIXME
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

    #ifdef DEBUGGER
    // TODO debugger controls
    #endif
    
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

}

void Input::mute()
{

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

        #ifdef DEBUGGER
        auto debug_bind_iter = debug_binds.find(key);
        while (debug_bind_iter != debug_binds.end())
        {
            debug_bind_iter->second());
            debug_bind_iter++;
        }
        #endif

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

// TODO this is all old code. Port this to loadBinds()
#ifdef DEBUGGER
void pollDebug(RunFlags& run_flags, SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_p:
                display.palette_selected += 1;
                display.palette_selected %= 8;
                break;
            case SDLK_f:
                run_flags.paused = false;
                run_flags.frame = true;
                break;
            case SDLK_t:
                run_flags.paused = false;
                run_flags.tick = true;
                break;
            case SDLK_g:
                run_flags.paused = false;
                run_flags.steps += 1;
                break;
            case SDLK_8: // Execute 2^8 instructions
                run_flags.paused = false;
                run_flags.steps += 0x0100;
                break;
            // TODO make these only work if Memory window is hovered/selected
            case SDLK_UP:
                display.mem_addrs.addrs[display.mem_addrs.device] -= 16;
                break;
            case SDLK_DOWN:
                display.mem_addrs.addrs[display.mem_addrs.device] += 16;
                break;
            case SDLK_LEFT:
                display.mem_addrs.addrs[display.mem_addrs.device]--;
                break;
            case SDLK_RIGHT:
                display.mem_addrs.addrs[display.mem_addrs.device]++;
                break;
            case SDLK_PAGEUP:
                display.mem_addrs.addrs[display.mem_addrs.device] -= 16 * 16;
                break;
            case SDLK_PAGEDOWN:
                display.mem_addrs.addrs[display.mem_addrs.device] += 16 * 16;
                break;
        }
    }

    // Make sure memory display address isn't out of bounds
    uint device = display.mem_addrs.device;
    assert (device <= 4);
    switch (device)
    {
    case 0: // Zero Page
        if (display.mem_addrs.addrs[device] >= 0x0100)
            display.mem_addrs.addrs[device] %= 0x0100;
        break;
    case 1: // Stack
        if (display.mem_addrs.addrs[device] < 0x0100)
            display.mem_addrs.addrs[device] += 0x0100;
        else if (display.mem_addrs.addrs[device] >= 0x0200)
            display.mem_addrs.addrs[device] %= 0x0100;
        break;
    case 2: // RAM
        if (display.mem_addrs.addrs[device] < 0x0200)
            display.mem_addrs.addrs[device] += 0x0400;
        else if (display.mem_addrs.addrs[device] >= 0x0800)
        {
            display.mem_addrs.addrs[device] -= 0x0200;
            display.mem_addrs.addrs[device] %= 0x0600;
            display.mem_addrs.addrs[device] += 0x0200;
        }
        break;
    case 3: // PRG RAM
        if (display.mem_addrs.addrs[device] < 0x6000 
            || display.mem_addrs.addrs[device] >= 0x8000)
        {
            display.mem_addrs.addrs[device] %= 0x2000;
            display.mem_addrs.addrs[device] += 0x6000;
        }
        break;
    case 4: // PRG ROM
        if (display.mem_addrs.addrs[device] < 0x8000)
            display.mem_addrs.addrs[device] += 0x8000;
        break;
    default:
        break;
    }
}
#endif

bool Input::poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        #ifdef DEBUGGER
        ImGui_ImplSDL2_ProcessEvent(&event);
        #endif
        pollKeyboard(event);
    }
    pollControllers();
    console.processInputs();
    return running;
}