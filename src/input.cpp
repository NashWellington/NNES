#include "input.h"

#define BUT_A       0x80
#define BUT_B       0x40
#define BUT_SELECT  0x20
#define BUT_START   0x10
#define BUT_UP      0x08
#define BUT_DOWN    0x04
#define BUT_LEFT    0x02
#define BUT_RIGHT   0x01

Input input = {};

Input::Input()
{
    controllers.fill(nullptr);
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            controllers[i] = SDL_GameControllerOpen(i);
            if (!controllers[i])
                std::cerr << "Could not open game controller " << i << std::endl;
        }
    }
}

Input::~Input()
{
    for (int i = 0; i < static_cast<int>(controllers.size()); i++)
    {
        if (controllers[i])
            SDL_GameControllerClose(controllers[i]);
    }
}

void Input::pollControllers()
{
    for (int i = 0; i < 2; i++)
    {
        if (controllers[i])
        {
            // Xinput B button = right button (NES A button)
            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_B))
                joypads[i] |= BUT_A;
            else
                joypads[i] &= ~BUT_A;
            
            // Xinput A button = bottom button (NES B button)
            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_A))
                joypads[i] |= BUT_B;
            else
                joypads[i] &= ~BUT_B;
            
            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_START))
                joypads[i] |= BUT_START;
            else
                joypads[i] &= ~BUT_START;
            
            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_BACK))
                joypads[i] |= BUT_SELECT;
            else
                joypads[i] &= ~BUT_SELECT;

            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_UP))
                joypads[i] |= BUT_UP;
            else
                joypads[i] &= ~BUT_UP;

            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_DOWN))
                joypads[i] |= BUT_DOWN;
            else
                joypads[i] &= ~BUT_DOWN;

            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_LEFT))
                joypads[i] |= BUT_LEFT;
            else
                joypads[i] &= ~BUT_LEFT;

            if (SDL_GameControllerGetButton(controllers[i], SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
                joypads[i] |= BUT_RIGHT;
            else
                joypads[i] &= ~BUT_RIGHT;
            
            bus.joypad_data_buffer[i] = joypads[i];
        }
    }
}

void Input::pollKeyboard(RunFlags& run_flags, SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                run_flags.finished = true;
                break;
            case SDLK_SPACE:
                run_flags.paused = !run_flags.paused;
                break;

            // Keyboard input if no controller
            if (!controllers[0])
            {
                case SDLK_w:
                    joypads[0] |= BUT_UP;
                    break;
                case SDLK_s:
                    joypads[0] |= BUT_DOWN;
                    break;
                case SDLK_a:
                    joypads[0] |= BUT_LEFT;
                    break;
                case SDLK_d:
                    joypads[0] |= BUT_RIGHT;
                    break;
                case SDLK_v:
                    joypads[0] |= BUT_A;
                    break;
                case SDLK_c:
                    joypads[0] |= BUT_B;
                    break;
                case SDLK_x:
                    joypads[0] |= BUT_START;
                    break;
                case SDLK_z:
                    joypads[0] |= BUT_SELECT;
                    break;
            }
        }
    }
    else if (event.type == SDL_KEYUP && !controllers[0])
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_w:
                joypads[0] &= ~BUT_UP;
                break;
            case SDLK_s:
                joypads[0] &= ~BUT_DOWN;
                break;
            case SDLK_a:
                joypads[0] &= ~BUT_LEFT;
                break;
            case SDLK_d:
                joypads[0] &= ~BUT_RIGHT;
                break;
            case SDLK_v:
                joypads[0] &= ~BUT_A;
                break;
            case SDLK_c:
                joypads[0] &= ~BUT_B;
                break;
            case SDLK_x:
                joypads[0] &= ~BUT_START;
                break;
            case SDLK_z:
                joypads[0] &= ~BUT_SELECT;
                break;
            default:
                break;
        }
    }
    else if (event.type == SDL_QUIT)
        run_flags.finished = true;
}

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

void Input::pollInputs(RunFlags& run_flags)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        #ifdef DEBUGGER
        ImGui_ImplSDL2_ProcessEvent(&event);
        pollDebug(run_flags, event);
        #endif
        pollKeyboard(run_flags, event);
        pollControllers();
    }
}