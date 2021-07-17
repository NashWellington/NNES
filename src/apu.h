#pragma once

class NES;

#include "globals.h"
#include "processor.h"
#include "audio.h"
#include "mem.h"

class APU : public Processor
{
public:
    APU(NES& _nes, Audio& _audio) : nes(_nes), audio(_audio) {}
    void setRegion(Region _region);

    // TODO
    void reset() { return; }
    // void save(Savestate& savestate) { return; }
    // void load(Savestate& savestate) { return; }

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

    NES& nes;
    Audio& audio;
private:
    void mix(); // Mix all channel outputs and push that to audio queue
    void clockPulse(int i);
    void clockTriangle();
    void clockNoise();
    void clockDMC();

    void clockEnvelope(int i);
    void clockLengthCounter(int i);

// APU + channel variables
    uint frame_ctr = 0;
    uint sample_i = 0; // Sample index (goes from 0 to sample_rate/15)

    struct
    {
        int timer = 0;
        int sequence = 7; // 8-step sequencer value
        int length = 0;
        uint out = 0; // The value to be mixed (0-15)
    } pulse[2] = {};

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