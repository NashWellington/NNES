#include "peripheral.hpp"

void NESStandardController::poll(bool parallel)
{
    if (parallel) poll_i = -1;
    else if (poll_i == -1) poll_i = 0;
}

ubyte NESStandardController::read()
{
    if (poll_i == -1 || poll_i > 7) return 1;
    else
    {
        ubyte data = value & (1 << poll_i) != 0;
        poll_i++;
        return data;
    }
}

void NESStandardController::keyboardActivate(uint button_id, bool pressed)
{
    assert(button_id < 8);
    if (pressed) keyboard_buffer[button_id] = 1;
    else keyboard_buffer[button_id] = 0;
}

void NESStandardController::joypadActivate(uint button_id)
{
    assert(button_id < 8);
    joypad_buffer |= 1 << button_id;
}

void NESStandardController::processInputs()
{
    ubyte keyboard_buffer_value = 0;
    for (int i = 0; i < 8; i++)
    {
        if (keyboard_buffer[i] > 0) keyboard_buffer_value |= 1 << i;
    }
    value = joypad_buffer | keyboard_buffer_value;
    joypad_buffer = 0;
}