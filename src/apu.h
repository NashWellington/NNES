#pragma once

// Forward declaration
//class Audio;
//void Audio::pushSample(float sample){};

#include "globals.h"
#include "audio.h"
#include "bus.h" // please please please no circular dependency issues

class APU
{
public:
    APU();
    ~APU();
    void tick();

    /* Envelope data
    * The envelope index corresponds to the channel the envelope affects
    * 0, 1 => pulse 1, pulse 2
    * 2 => noise
    */
    struct
    {
        bool start = true;
        int decay = 15;
        int divider = 0;
    } envelope[3] = {};

    /* Load the length counter of a channel
    * The index parameter corresponds to a channel
    * 0, 1 => pulse 1, pulse 2
    * 2 => triangle
    * 3 => noise
    */
    void loadLengthCounter(int i);

    /* Set the length counter to 0
    * Same indices as loadLengthCounter
    * Should only be called for each of the low 4 bits in $4015 write
    * when they are zeroed out
    */
    void clearLengthCounter(int i);

    /* Fill reg_apu_status with relevant data
    * Should only be called when $4015 is read
    */
    void getStatus();

private:
    void mix(); // Mix all channel outputs and push that to audio queue
    void clockPulse(int i);
    void clockTriangle();
    void clockNoise();
    void clockDMC();

    void clockEnvelope(int i);
    void clockLengthCounter(int i);

// Audio frontend reference //TODO shared_ptr? unique_ptr?
    std::unique_ptr<Audio> audio;

// APU + channel variables
    uint cycle = 0;
    uint frame_ctr = 0;
    uint sample_i = 0; // Sample index (goes from 0 to sample_rate/15)

    struct
    {
        int timer = 0;
        int sequence = 7; // 8-step sequencer value
        int length = 0;
        float output = 0; // The value to be mixed. Read at 44.1kHz
    } pulse[2] = {};

// Channel constants (mostly lookup tables)
    /* The number of APU cycles in four PPU frames
    * (Note: I would do APU cycles per frame, but that wouldn't be an integer)
    */
    const uint MAX_CYCLE = 59561;

    const std::array<std::array<int,8>,4> pulse_waveforms =
    {
        0, 1, 0, 0, 0, 0, 0, 0,    // 12.5%
        0, 1, 1, 0, 0, 0, 0, 0,    // 25%
        0, 1, 1, 1, 1, 0, 0, 0,    // 50%
        1, 0, 0, 1, 1, 1, 1, 1     // 25% negated
    };

    const std::array<int,32> length_table = 
    {//    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
/*00-0F*/  10,254, 20,  2, 40,  4, 80,  6,160,  8, 60, 10, 14, 12, 26, 14,
/*10-1F*/  12, 16, 24, 18, 48, 20, 96, 22,192, 24, 72, 26, 16, 28, 32, 30
    };

public:
// IRQ line
    bool frame_interrupt = false;

// Registers
    /* Pulse control registers
    * $4000 and $4004
    * 
    * 7 6 5 4   3 2 1 0
    * D D L C   N N N N
    * //TODO
    * 
    */
    union
    {
        struct
        {
            unsigned volume         : 4; // envelope period or volume
            unsigned constant_vol   : 1;
            unsigned loop           : 1; // loop envelope flag or halt length counter
            unsigned duty           : 2;
        };
        ubyte reg;
    } reg_pulse_ctrl[2] = {{.reg = 0}, {.reg = 0}};

    /* Pulse sweep registers
    * $4001 and $4005
    * 
    * 7 6 5 4   3 2 1 0
    * E P P P   N S S S
    * //TODO
    * 
    */
    union
    {
        struct
        {
            unsigned shift      : 3; // shift count
            unsigned negative   : 1;
            unsigned period     : 3;
            unsigned enabled    : 1;
        };
        ubyte reg;
    } reg_sweep[2] = {{.reg = 0}, {.reg = 0}};

    /* Pulse timer
    * $4002 and $4006
    * (also bits 0-2 of $4003 and $4007)
    */
    uword pulse_timer[2] = {}; //TODO name change? start vals?

    /* Pulse length counter load
    * bits 4-7 of $4003 and $4007
    */
    ubyte pulse_length_ctr_load[2] = {}; //TODO better name? etc.

    /* APU control register
    * $4015 write
    * 
    * 7 6 5 4   3 2 1 0
    * - - - D   N T 2 1
    * 
    * D - DMC enable
    * N - noise channel length counter enable
    * T - triangle ''
    * 2 - pulse 2 ''
    * 1 - pulse 1 ''
    */
    union
    {
        struct
        {
            unsigned lce_p1     : 1; // Length Counter Enable Pulse 1
            unsigned lce_p2     : 1; // '' Pulse 2
            unsigned lce_tr     : 1; // '' TRiangle
            unsigned lce_ns     : 1; // '' NoiSe
            unsigned dmc_enable : 1;
            unsigned            : 3;
        };
        ubyte reg;
    } reg_apu_ctrl {.reg = 0};

    /* APU status register
    * $4015 read
    * 
    * 7 6 5 4   3 2 1 0
    * I F - D   N T 2 1
    * 
    * 
    */
    union
    {
        struct
        {
            unsigned lc_p1              : 1; // Length Counter Pulse 1
            unsigned lc_p2              : 1;
            unsigned lc_tr              : 1;
            unsigned lc_ns              : 1;
            unsigned dmc_active         : 1;
            unsigned                    : 1;
            unsigned frame_interrupt    : 1;
            unsigned dmc_interrupt      : 1;
        };
        ubyte reg;
    } reg_apu_status {.reg = 0};

    /* APU frame counter register
    * $4017
    * 
    * 7 6 5 4   3 2 1 0
    * M I - -   - - - -
    * 
    * M - Sequence mode
    *     0 = 4-step sequencing
    *     1 = 5-step sequencing
    * I - IRQ disable flag
    */
    union
    {
        struct
        {
            unsigned                : 6;
            unsigned irq_disable    : 1;
            unsigned mode           : 1;
        };
        ubyte reg;
    } reg_frame_ctr {.reg = 0}; // TODO start/reset 
};

extern APU apu;