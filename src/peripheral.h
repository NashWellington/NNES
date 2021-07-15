#pragma once

#include "globals.h"

class Peripheral
{
public:
    virtual ~Peripheral() {}
    virtual void processInputs() = 0;
    std::string type;
    std::vector<std::shared_ptr<Peripheral>> children;
};

class Controller : public Peripheral
{
public:
    virtual ~Controller() {}
    virtual void poll(bool parallel) = 0;
    virtual ubyte read() = 0;
    virtual void keyboardActivate(uint button_id, bool pressed) = 0;
    virtual void joypadActivate(uint button_id) = 0;
    virtual void processInputs() = 0;
    std::string type = "Controller";
};

/* NES/Famicom Standard Controller
* Button IDs are linked to which bit the button is mapped to
* 0 - A
* 1 - B
* 2 - Select
* 3 - Start
* 4 - Up
* 5 - Down
* 6 - Left
* 7 - Right
*/
class NESStandardController : public Controller
{
public:
    void  poll(bool parallel);
    ubyte read();
    void  keyboardActivate(uint button_id, bool pressed);
    void  joypadActivate(uint button_id);
    void  processInputs();
    std::string type = "NES Standard Controller";
private:
    ubyte joypad_buffer = 0;
    std::array<uint,8> keyboard_buffer = {};
    ubyte value = 0;

    /* Poll index
    * -1  : controller polled & in parallel mode, will return 1 when read?
    * // TODO reread PISO behavior
    * 0-7 : controller polled & in serial mode, will return polled value and increment
    * >=8 : controller not polled, will return 1
    */
    int poll_i = 8;
};

class Expansion : public Peripheral
{
};