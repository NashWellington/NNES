#include "input.hpp"

Input::Input(Console& _console, Audio&_audio, Video& _video, std::shared_ptr<Config> _config)
    : console(_console), audio(_audio), video(_video), config(_config)
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

    loadBinds();
}

Input::~Input()
{
    for (uint i = 0; i < joypads.size(); i++)
    {
        if (joypads[i])
            SDL_GameControllerClose(joypads[i]);
    }
}

void Input::loadBinds()
{
    // Emulator binds
    // For now, only controller 1 can map to emulator binds
    config->findSection("/// NNES emulator binds");
    config->findNext("quit");
    for (auto keycode : config->getKeys())
    {
        emu_binds.insert({keycode,
            [this]() { quit(); }
        });
    }

    config->findNext("pause");
    for (auto keycode : config->getKeys())
    {
        emu_binds.insert({keycode,
            [this]() { pause(); }
        });
    }

    config->findNext("mute");
    for (auto keycode : config->getKeys())
    {
        emu_binds.insert({keycode,
            [this]() { mute(); }
        });
    }
    
    config->findNext("toggle fps");
    for (auto keycode : config->getKeys())
    {
        emu_binds.insert({keycode,
            [this]() { toggle_fps(); }
        });
    }

    config->findNext("toggle render time");
    for (auto keycode : config->getKeys())
    {
        emu_binds.insert({keycode,
            [this]() { toggle_render_time(); }
        });
    }

    config->findNext("reset");
    for (auto keycode : config->getKeys())
    {
        emu_binds.insert({keycode,
            [this]() { reset(); }
        });
    }

    // Game Input binds
    std::vector<std::shared_ptr<Controller>> controllers = console.controllers;

    config->findSection("/// NES Standard Controller binds");
    // TODO support for more controllers
    for (int i = 0; i < 2; i++)
    {
        std::string port_string = "/// Port ";
        port_string += static_cast<char>(i+1 + 48);
        config->findNext(port_string);

        config->findNext("button-a");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(0, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(0, controllers[i])
            });
        }

        config->findNext("button-b");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(1, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(1, controllers[i])
            });
        }

        config->findNext("button-select");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(2, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(2, controllers[i])
            });
        }

        config->findNext("button-start");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(3, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(3, controllers[i])
            });
        }

        config->findNext("dpad-up");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(4, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(4, controllers[i])
            });
        }

        config->findNext("dpad-down");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(5, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(5, controllers[i])
            });
        }

        config->findNext("dpad-left");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(6, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(6, controllers[i])
            });
        }

        config->findNext("dpad-right");
        for (auto keycode : config->getKeys())
        {
            key_binds.insert({keycode,
                Bind(7, controllers[i])});
        }
        for (auto button_code : config->getButtons())
        {
            joypad_binds.insert(
            {
                {static_cast<SDL_GameControllerButton>(button_code), i},
                Bind(7, controllers[i])
            });
        }
    }
}

void Input::quit()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    running = false;
    lk.unlock();
    cv.notify_one();
}

bool Input::pause()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    console.paused = !console.paused;
    video.paused = !video.paused;
    bool p = video.paused;
    lk.unlock();
    cv.notify_one();
    return p;
}

bool Input::mute()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    audio.audio_buffer.muted = !audio.audio_buffer.muted;
    video.muted = !video.muted;
    bool m = video.muted;
    lk.unlock();
    cv.notify_one();
    return m;
}

bool Input::toggle_fps()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    video.show_framerate = !video.show_framerate;
    bool f = video.show_framerate;
    lk.unlock();
    cv.notify_one();
    return f;
}

void Input::toggle_fps(bool show)
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    video.show_framerate = show;
    lk.unlock();
    cv.notify_one();
}

Video::RenderTimeDisplay Input::toggle_render_time()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    if (video.show_render_time == Video::RenderTimeDisplay::NO)
        video.show_render_time = Video::RenderTimeDisplay::MS;
    else if (video.show_render_time == Video::RenderTimeDisplay::MS)
        video.show_render_time = Video::RenderTimeDisplay::PERCENT;
    else if (video.show_render_time == Video::RenderTimeDisplay::PERCENT)
        video.show_render_time = Video::RenderTimeDisplay::NO;
    auto v = video.show_render_time;
    lk.unlock();
    cv.notify_one();
    return v;
}

Video::RenderTimeDisplay Input::toggle_render_time(bool show)
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    if (!show)
        video.show_render_time = Video::RenderTimeDisplay::NO;
    else
    {
        if (video.show_render_time == Video::RenderTimeDisplay::MS)
            video.show_render_time = Video::RenderTimeDisplay::PERCENT;
        else video.show_render_time = Video::RenderTimeDisplay::MS;
    }
    auto v = video.show_render_time;
    lk.unlock();
    cv.notify_one();
    return v;
}

void Input::toggle_render_time(Video::RenderTimeDisplay opt)
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    video.show_render_time = opt;
    lk.unlock();
    cv.notify_one();
}

void Input::reset()
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, [this]{return ready();});
    console.reset();
    lk.unlock();
    cv.notify_one();
}

bool Input::ready()
{
    std::unique_lock<std::mutex> lk(mtx_2);
    return is_ready;
}

void Input::ready(bool r)
{
    std::unique_lock<std::mutex> lk(mtx_2);
    is_ready = r;
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
    ready(true);
    cv.notify_one();
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        pollKeyboard(event);
    }
    pollControllers();
    console.processInputs();
    ready(false);
    return running;
}