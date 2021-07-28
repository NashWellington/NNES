#pragma once

class NES;

#include "globals.hpp"
#include "processor.hpp"
#include "audio.hpp"
#include "mem.hpp"

struct Timer
{
    virtual ~Timer() {}
    // Returns true if a clock cycle is output
    virtual bool clock() = 0;
    int period = 0;
    int counter = 0;
};

struct Divider : public Timer
{
    bool clock();
};

// Linear Feedback Shift Register (used by noise channel)
struct LFSR : public Timer
{
    // Return true if bit 0 of shift_reg is set
    // If true, blocks envelope data
    bool clock();
    uword shift_reg = 0;
    bool mode = false;
};

struct Sequencer
{
    void clock();
    int seq_min = 0;
    int seq_max = 0;
    int sequence = 0;
    uint out = 0;
};

struct LengthCounter
{
    void load(ubyte length);
    void clock();
    int count = 0;
    bool enabled = false;
    bool halt = false;
};

struct LinearCounter
{
    void clock();
    int reload_val = 0;
    int count = 0;
    bool control = false; // gets buffered into reload every clock cycle
    bool reload = false;
};

struct Envelope
{
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
    void clock(Divider& timer);
    bool ones_comp = false; // Set to true for Pulse 1
    ubyte shift = 0;
    bool negate = false;
    ubyte period = 0;
    bool enabled = false;
    bool reload = false;
    bool mute = false;
    int count = 0;
};

struct Pulse
{
    void clock();
    ubyte duty = 0;
    Sequencer seq = { .seq_min = 0, .seq_max = 7, .sequence = 7 };
    Divider timer = {};
    LengthCounter len = {};
    Envelope env = {};
    Sweep sweep = {};
    uint out = 0; // The value to be mixed (0-15)
};

struct Triangle
{
    void clock();
    Divider timer = {}; // This gets called twice (b/c tri timer runs at CPU clock speed)
    //  The real sequence goes from 15 to 0 then 0 to 15,
    // but it's just easier to do this and take the absolute value
    Sequencer seq = { .seq_min = -16, .seq_max = 15, .sequence = 7 };
    LengthCounter len = {};
    LinearCounter lin = {};
    uint out = 0; // the value to be mixed (0-15)
};

struct Noise
{
    void clock();
    LFSR timer = {};
    Envelope env = {};
    LengthCounter len = {};
    uint out = 0; // 0-15
};

struct DMC
{
    void clock(NES& nes);
    void start(); // start or restart
    bool irq_enable = false;
    bool interrupt = false;
    bool loop = false;
    int rate = 428; // Default NTSC val //TODO PAL
    int cycles_left = 428; // Number of cycles output level next changes
    ubyte out = 0; // 0-127
    bool silence = true;

    // Memory reader
    uword sample_addr = 0x8000; // Minimum address to read from
    uword curr_addr = 0x8000;
    int bytes_remaining = 0;
    int sample_len = 0;

    // Output
    std::optional<ubyte> sample_buf = {}; // returns false if empty
    ubyte shift_reg = 0;
    ubyte bits_remaining = 0;
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

    NES& nes;
    Audio& audio;
    
private:
    void mix(); // Mix all channel outputs and push that to audio queue
// APU + channel variables
    // "After reset or power-up, APU acts as if $4017 were written with $00
    // from 9 to 12 cycles before first instruction begins"
    // - reset_timing (test 9 of 11 in apu_test) by blargg
    // Note: setting this to 2-4 seems to work
    uint frame_ctr = 4;
    uint sample_i = 0; // Sample index (goes from 0 to sample_rate/15)
    enum
    {
        FOUR_STEP,
        FIVE_STEP
    } frame_count_mode = FOUR_STEP;
    bool interrupt_inhibit = false;

public:
    Pulse pulse_1 = { .sweep = { .ones_comp = true }};
    Pulse pulse_2 = {};
    Triangle triangle = {};
    Noise noise = {};
    DMC dmc = {};

    bool frame_interrupt = false;
};