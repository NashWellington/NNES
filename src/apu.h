#pragma once

class NES;

#include "globals.h"
#include "processor.h"
#include "audio.h"
#include "mem.h"

// Really this is a timer, but a timer only contains a divider anyway
struct Timer
{
    // Note: not all dividers can reload on command
    void reload();
    // Returns true if a clock cycle is output
    bool clock();
    int period = 0;
    int counter = 0;
};

struct Sequencer
{
    void clock();
    int seq_min = 0;
    int seq_max = 0;
    int sequence = 0;
    bool out = false;
};

struct LengthCounter
{
    void load(ubyte length);
    void clear();
    /* Clocks the length counter
    * enable - $4015 bit 0/1 write (reg_apu_ctrl.lce_p1/lce_p2)
    * halt   - $4000/$4004 bit 5 (reg_pulse_ctrl)
    */
    void clock();
    int count = 0;
    bool enable = false;
    bool halt = false;
    bool silence = true; // Silences channel
};

struct LinearCounter
{
    /* Clocks the linear counter
    * control - $4008 bit 7
    * length  - $4008 bits 0-6
    */
    void clock();
    int reload_val = 0;
    int count = 0;
    bool control = false; // gets buffered into reload every clock cycle
    bool reload = false;
    bool silence = true;
};

struct Envelope
{
    /* Clocks the envelope
    * period - $4000/$4004 bits 0-3 OR //TODO noise chan
    * loop   - $4000/$4004 bit 5 OR //TODO noise chan
    * cv     - $4000/$4004 bit 4 (constant volume)
    */
    void clock();
    ubyte period = 0; // used as period or constant volume control
    bool cv = false;
    bool loop = false;
    bool start = true;
    int decay = 15;
    int divider = 0;
    uint out = 0;
};

struct Sweep
{
    void clock(Timer& timer);
    bool ones_comp = false; // Set to true for Pulse 1
    ubyte shift = 0;
    bool negate = false;
    ubyte period = 0;
    bool enabled = false;
    bool reload = false;
    bool mute = false;
    int divider = 0;
};

struct Pulse
{
    /* Duty cycle index
    * $4000/$4004 bits 6-7 (reg_pulse_ctrl)
    */
    void clock();
    ubyte duty = 0;
    Sequencer seq = { .seq_min = 0, .seq_max = 7 };
    Timer timer = {};
    LengthCounter len = {};
    Envelope env = {};
    Sweep sweep = {};
    uint out = 0; // The value to be mixed (0-15)
    bool enable = false; // Enabled/disabled by $4015 write to bit 0/1
};

struct Triangle
{
    void clock();
    Timer timer = {}; // This gets called twice (b/c tri timer runs at CPU clock speed)
    Sequencer seq = { .seq_min = -15, .seq_max = 15 };
    LengthCounter len = {};
    LinearCounter lin = {};
    uint out = 0; // the value to be mixed (0-15)
    bool enable = false; // Enabled/disabled by $4015 write to bit 2
};

class APU : public Processor
{
public:
    APU(NES& _nes, Audio& _audio) : nes(_nes), audio(_audio) {}
    void setRegion(Region _region);

    void reset();
    // void save(Savestate& savestate) { return; }
    // void load(Savestate& savestate) { return; }

    void tick();
    ubyte read(uword address);
    void write(uword address, ubyte data);

    /* Fill reg_apu_status with relevant data
    * Should only be called when $4015 is read
    */
    void getStatus();

    NES& nes;
    Audio& audio;
    
private:
    void mix(); // Mix all channel outputs and push that to audio queue
    void clockPulse(int i);
    void clockTriangle();
    void clockNoise();
    void clockDMC();

    void clockEnvelope(int i);
public:
// APU + channel variables
    uint frame_ctr = 0;
    uint sample_i = 0; // Sample index (goes from 0 to sample_rate/15)
    enum
    {
        FOUR_STEP,
        FIVE_STEP
    } frame_count_mode = FOUR_STEP;
    bool irq_inhibit = false;

    Pulse pulse_1 = { .sweep = { .ones_comp = true }};
    Pulse pulse_2 = {};
    Triangle triangle = {};

public:
// IRQ line
    bool frame_interrupt = false;

};