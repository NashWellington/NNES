#include "input.h"

#define BUT_A       0x01
#define BUT_B       0x02
#define BUT_SELECT  0x04
#define BUT_START   0x08
#define BUT_UP      0x10
#define BUT_DOWN    0x20
#define BUT_LEFT    0x40
#define BUT_RIGHT   0x80

Input input = {};

void Input::pollInputs(RunFlags& run_flags)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        #ifdef DEBUGGER
        ImGui_ImplSDL2_ProcessEvent(&event);
        #endif
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        run_flags.finished = true;
                        break;
                    case SDLK_SPACE:
                        run_flags.paused = !run_flags.paused;
                        break;

                    #ifdef DEBUGGER
                    // Debugger controls
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
                    #endif

                    // Game inputs
                    case SDLK_w:
                        joypad_1 |= BUT_UP;
                        break;
                    case SDLK_s:
                        joypad_1 |= BUT_DOWN;
                        break;
                    case SDLK_a:
                        joypad_1 |= BUT_LEFT;
                        break;
                    case SDLK_d:
                        joypad_1 |= BUT_RIGHT;
                        break;
                    case SDLK_v:
                        joypad_1 |= BUT_A;
                        break;
                    case SDLK_c:
                        joypad_1 |= BUT_B;
                        break;
                    case SDLK_x:
                        joypad_1 |= BUT_START;
                        break;
                    case SDLK_z:
                        joypad_1 |= BUT_SELECT;
                        break;

                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    // Game inputs
                    case SDLK_w:
                        joypad_1 &= ~BUT_UP;
                        break;
                    case SDLK_s:
                        joypad_1 &= ~BUT_DOWN;
                        break;
                    case SDLK_a:
                        joypad_1 &= ~BUT_LEFT;
                        break;
                    case SDLK_d:
                        joypad_1 &= ~BUT_RIGHT;
                        break;
                    case SDLK_v:
                        joypad_1 &= ~BUT_A;
                        break;
                    case SDLK_c:
                        joypad_1 &= ~BUT_B;
                        break;
                    case SDLK_x:
                        joypad_1 &= ~BUT_START;
                        break;
                    case SDLK_z:
                        joypad_1 &= ~BUT_SELECT;
                        break;
                    default:
                        break;
                }
                break;

            case SDL_QUIT:
                run_flags.finished = true;
                break;

            default:
                break;
        }
    }
    #ifdef DEBUGGER
    uint device = display.mem_addrs.device;
    assert (device <= 3);
    switch (device)
    {
        case 0: // zpg
            if (display.mem_addrs.addrs[device] >= 0x0100)
                display.mem_addrs.addrs[device] %= 0x0100;
            break;
        case 1: // stack
            if (display.mem_addrs.addrs[device] < 0x0100)
                display.mem_addrs.addrs[device] += 0x0100;
            else if (display.mem_addrs.addrs[device] >= 0x0200)
                display.mem_addrs.addrs[device] %= 0x0100;
            break;
        case 2: // ram
            if (display.mem_addrs.addrs[device] < 0x0200)
                display.mem_addrs.addrs[device] += 0x0400;
            else if (display.mem_addrs.addrs[device] >= 0x0800)
            {
                display.mem_addrs.addrs[device] -= 0x0200;
                display.mem_addrs.addrs[device] %= 0x0600;
                display.mem_addrs.addrs[device] += 0x0200;
            }
            break;
        case 3: // PRG_ROM
            if (display.mem_addrs.addrs[device] < 0x8000)
                display.mem_addrs.addrs[device] += 0x8000;
            break;
        default:
            break;
    }
    #endif
    bus.joypad_data[0] = joypad_1;
}