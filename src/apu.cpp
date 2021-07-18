#include "apu.h"

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

// TODO other sound channels
const uint nse_out = 0;
const uint dmc_out = 0;

// TODO let SDL handle resampling???

// clock these every quarter frame
// TODO noise envelope
// TODO triangle linear counter
#define clockQF() {\
clockEnvelope(0);\
clockEnvelope(1);\
triangle.lin.clock(reg_lin_ctr.reload, reg_lin_ctr.control);\
}

// clock these every half frame
// TODO noise length ctr
// TODO sweep units
#define clockHF() {\
pulse[0].len.clock(reg_apu_ctrl.lce_p1, reg_pulse_ctrl[0].loop);\
pulse[1].len.clock(reg_apu_ctrl.lce_p2, reg_pulse_ctrl[1].loop);\
triangle.len.clock(reg_apu_ctrl.lce_tr, reg_lin_ctr.control);\
}

void APU::setRegion(Region _region)
{
    region = _region;
    name = "NES APU"; // It's a pAPU within the CPU so it doesn't really have its own name
    switch (region)
    {
        case Region::NTSC:
            time_scale = 24;
            break;
        case Region::PAL:
            time_scale = 32;
            break;
        case Region::Dendy:
            time_scale = 30;
            break;
        default:
            std::cerr << "Error: unsupported region" << std::endl;
            throw std::exception();
            break;
    }
}

// https://wiki.nesdev.com/w/index.php/CPU_power_up_state
void APU::reset()
{
    reg_apu_ctrl.reg = 0;
    // TODO reset triangle phase to 0
    // TODO AND APU DPCM output with 1
    // TODO reset APU Frame Counter if 2A03G
}

// TODO should I run this at CPU freq and only do other APU stuff every other cycle?
void APU::tick()
{
    switch(frame_ctr)
    {
        case 3729:  // APU cycle 3728.5 -> step 1
            clockQF()
            break;

        case 7457:  // APU cycle 7456.5 -> step 2
            clockQF()
            clockHF()
            break;

        case 11186: // APU cycle 11185.5 -> step 3
            clockQF()
            break;

        case 14915: // APU cycle 14914.5 -> step 4
            // Do step 4 if 4-step sequence
            if (reg_frame_ctr.mode == 0)
            {
                // Generate IRQ if interrupt inhibit is clear
                if (!reg_frame_ctr.irq_disable) frame_interrupt = true;
                clockQF()
                clockHF() 
                frame_ctr = 0;
            }
            break;
            
        case 18641: // APU cycle 18640.5 -> step 5
            assert(reg_frame_ctr.mode == 1);
            clockQF()
            clockHF()
            frame_ctr = 0;
            break;

        default:
            break;
    }
    frame_ctr++;
    cycle++;

    clockPulse(0);
    clockPulse(1);
    clockTriangle();
    /*
    clockNoise();
    clockDMC();*/

    if ((cycle*audio.sample_rate / (15*MAX_CYCLE)) >= (sample_i+1)) // Should make 44.1k samples/sec
    {
        sample_i++;
        mix();
    }

    if (cycle == MAX_CYCLE) 
    {
        cycle = 0;
        sample_i = 0;
    }
}

/* For more info on mixing: https://wiki.nesdev.com/w/index.php/APU_Mixer
* Note: pulse, triangle, and noise channels output values 0-15
        DMC channel outputs 0-127
*/
void APU::mix()
{
    float pulse_sum = static_cast<float>(pulse[0].out + pulse[1].out);
    float pulse_out = !pulse_sum ? 0 : 95.88f / ((8128.0f / pulse_sum) + 100.0f);
    float tnd_sum = (static_cast<float>(triangle.out) / 8227.0f) 
                 + (static_cast<float>(nse_out) / 12241.0f) 
                 + (static_cast<float>(dmc_out) / 22638.0f);
    float tnd_out = !tnd_sum ? 0 : 159.79f / ((1.0f / tnd_sum) + 100.0f);
    float output = pulse_out + tnd_out;
    audio.pushSample(output);
}

void APU::clockPulse(int i)
{
    pulse[i].timer--;
    if (pulse[i].timer < 0) pulse[i].timer = pulse_timer[i]; // Reload timer
    if (pulse[i].timer == pulse_timer[i])
    {
        uint volume;
        if (reg_pulse_ctrl[i].constant_vol)
        {
            volume = reg_pulse_ctrl[i].volume;
        }
        else
        {
            volume = envelope[i].decay;
        }
        if (pulse_timer[i] < 8) volume = 0;
        if (pulse[i].len.count == 0) volume = 0;
        pulse[i].out = volume * pulse_waveforms[reg_pulse_ctrl[i].duty][pulse[i].sequence];
        pulse[i].sequence--;
        if (pulse[i].sequence < 0) pulse[i].sequence = 7;
    }
}

void APU::clockTriangle()
{
    triangle.timer -= 2; // TODO validate
    if (triangle.timer < 0) triangle.timer = tri_timer; // Reload timer
    if (!triangle.lin.count || !triangle.len.count) triangle.out = 0;
    else if (triangle.timer == tri_timer)
    {
        triangle.out = abs(triangle.sequence);
        triangle.sequence--;
        if (triangle.sequence < -15) triangle.sequence = 15;
    }
}

void APU::clockEnvelope(int i)
{
    // TODO change from 0 to noise period val when I do noise channel
    ubyte period = (i == 2) ? 0 : reg_pulse_ctrl[i].volume;
    if (envelope[i].start) 
    {
        envelope[i].start = false;
        envelope[i].decay = 15;
        envelope[i].divider = period;
    }
    else // clock divider
    {
        envelope[i].divider--;
        if (envelope[i].divider <= 0)
        {
            envelope[i].divider = period; // TODO V+1??
            // Clock decay counter
            if (envelope[i].decay > 0)
                envelope[i].decay--;
            // TODO change from false to noise loop flag when I do noise channel
            else if ((i == 2) ? false : reg_pulse_ctrl[i].loop)
            {
                envelope[i].decay = 15;
            }
        }
    }
}

void LengthCounter::load(ubyte length)
{
    count = length_table[length];
}

void LengthCounter::clear()
{
    count = 0;
}

void LengthCounter::clock(bool enable, bool halt)
{
    if (enable && !halt && count > 0)
        count--;
}

void LinearCounter::load(ubyte length)
{
    count = length;
}

void LinearCounter::clear()
{
    count = 0;
}

void LinearCounter::clock(ubyte length, bool control)
{
    if (reload) { count = length; reload = false; }
    if (!control) count = 0;
}

void APU::getStatus()
{
    reg_apu_status.reg = 0;
    reg_apu_status.lc_p1 = (pulse[0].len.count > 0);
    reg_apu_status.lc_p2 = (pulse[1].len.count > 0);
    reg_apu_status.lc_tr = (triangle.len.count > 0);
    // TODO noise length counter
    // TODO DMC activity
    reg_apu_status.frame_interrupt = frame_interrupt;
    // TODO DMC interrupt
}