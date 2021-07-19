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
#define clockQF() {\
pulse_1.env.clock();\
pulse_2.env.clock();\
triangle.lin.clock();\
}

// clock these every half frame
// TODO noise length ctr
#define clockHF() {\
pulse_1.len.clock();\
pulse_2.len.clock();\
triangle.len.clock();\
pulse_1.sweep.clock(pulse_1.timer);\
pulse_2.sweep.clock(pulse_1.timer);\
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
    write(4015, 0);
    // TODO reset triangle phase to 0 // TODO phase??
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
            if (frame_count_mode == 0)
            {
                // Generate IRQ if interrupt inhibit is clear
                if (!irq_inhibit) frame_interrupt = true;
                clockQF()
                clockHF() 
                frame_ctr = 0;
            }
            break;
            
        case 18641: // APU cycle 18640.5 -> step 5
            assert(frame_count_mode == 1);
            clockQF()
            clockHF()
            frame_ctr = 0;
            break;

        default:
            break;
    }
    frame_ctr++;
    cycle++;

    pulse_1.clock();
    pulse_2.clock();
    triangle.clock();
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
*       DMC channel outputs 0-127
*/
void APU::mix()
{
    float pulse_sum = static_cast<float>(pulse_1.out + pulse_2.out);
    float pulse_out = !pulse_sum ? 0 : 95.88f / ((8128.0f / pulse_sum) + 100.0f);
    float tnd_sum = (static_cast<float>(triangle.out) / 8227.0f) 
                  + (static_cast<float>(nse_out) / 12241.0f) 
                  + (static_cast<float>(dmc_out) / 22638.0f);
    float tnd_out = !tnd_sum ? 0 : 159.79f / ((1.0f / tnd_sum) + 100.0f);
    float output = pulse_out + tnd_out;
    audio.pushSample(output);
}

ubyte APU::read(uword address)
{
    assert(address == 0x4015);
    ubyte data = 0;
    /* APU Status
    * $4015
    * 
    * 7 6 5 4   3 2 1 0
    * I F - D   N T 2 1
    * 
    * N,T,2,1 - length counter > 0
    * D       - DMC active
    * F       - frame interrupt
    * I       - DMC interrupt
    */
    data |= (pulse_1.len.count > 0) ? 1 : 0;
    data |= (pulse_2.len.count > 0) ? 1 : 0;
    data |= (triangle.len.count > 0) ? 1 : 0;
    // TODO noise
    // TODO DMC active
    // TODO interrupts
    return data;
}

void APU::write(uword address, ubyte data)
{
    switch (address)
    {
        /* Pulse 1 control register
        * $4000
        * 
        * 7 6 5 4   3 2 1 0
        * D D L C   N N N N
        * 
        * N - envelope period or volume (if constant volume)
        * C - constant volume flag
        * L - loop envelope flag or halt length counter
        * D - duty cycle lookup table index
        */
        case 0x4000:
            pulse_1.env.period = data & 0x0F;
            pulse_1.env.cv = data & 0x10;
            pulse_1.env.loop = data & 0x20;
            pulse_1.len.halt = data & 0x20;
            pulse_1.duty = data >> 6;
            break;

        /* Pulse 1 sweep register
        * $4001
        * 
        * 7 6 5 4   3 2 1 0
        * E P P P   N S S S
        * 
        * S - shift count
        * N - negate flag
        * P - the divider's period
        * E - sweep enabled
        * 
        * Side effects: sweep reload flag set
        */
        case 0x4001:
            pulse_1.sweep.shift = data & 0x07;
            pulse_1.sweep.negate = data & 0x08;
            pulse_1.sweep.period = (data & 0x70) >> 4;
            pulse_1.sweep.enabled = data & 0x80;
            pulse_1.sweep.reload = true;
            break;

        /* Pulse 1 timer (low 8 bits)
        * $4002
        */
        case 0x4002:
            pulse_1.timer.period &= 0xFF00;
            pulse_1.timer.period |= data;
            break;

        /* Pulse 1 length counter load & timer (high 3 bits) 
        * $4003
        * 
        * Side effects: sequencer is set to its starting value, envelope start flag set
        */
        case 0x4003:
            pulse_1.len.load(data >> 3);
            pulse_1.timer.period &= 0x00FF;
            pulse_1.timer.period |= (data & 0x07) << 8;
            pulse_1.seq.sequence = pulse_1.seq.seq_max;
            pulse_1.env.start = true;
            break;

        /* Pulse 2 control register
        * $4004
        * 
        * 7 6 5 4   3 2 1 0
        * D D L C   N N N N
        * 
        * N - envelope period or volume (if constant volume)
        * C - constant volume flag
        * L - loop envelope flag or halt length counter
        * D - duty cycle lookup table index
        */
        case 0x4004:
            pulse_2.env.period = data & 0x0F;
            pulse_2.env.cv = data & 0x10;
            pulse_2.env.loop = data & 0x20;
            pulse_2.len.halt = data & 0x20;
            pulse_2.duty = data >> 6;
            break;

        /* Pulse 2 sweep register
        * $4005
        * 
        * 7 6 5 4   3 2 1 0
        * E P P P   N S S S
        * 
        * S - shift count
        * N - negate flag
        * P - the divider's period
        * E - sweep enabled
        * 
        * Side effects: sweep reload flag set
        */
        case 0x4005:
            pulse_2.sweep.shift = data & 0x07;
            pulse_2.sweep.negate = data & 0x08;
            pulse_2.sweep.period = (data & 0x70) >> 4;
            pulse_2.sweep.enabled = data & 0x80;
            pulse_2.sweep.reload = true;
            break;

        /* Pulse 2 timer (low 8 bits)
        * $4006
        */
        case 0x4006:
            pulse_2.timer.period &= 0xFF00;
            pulse_2.timer.period |= data;
            break;


        /* Pulse 2 length counter load & timer (high 3 bits) 
        * $4007
        * 
        * Side effects: sequencer is set to its starting value, envelope start flag set
        */
        case 0x4007:
            pulse_2.len.load(data >> 3);
            pulse_2.timer.period &= 0x00FF;
            pulse_2.timer.period |= (data & 0x07) << 8;
            pulse_2.seq.sequence = pulse_2.seq.seq_max;
            pulse_2.env.start = true;
            break;
        
        /* Triangle linear counter
        * $4008
        * 
        * 7 6 5 4   3 2 1 0
        * C R R R   R R R R
        * 
        * R - counter reload value
        * C - control & length counter halt flag
        */
        case 0x4008:
            triangle.lin.reload_val = data & 0x7F;
            triangle.lin.control = data & 0x80;
            triangle.len.halt = data & 0x80;
            break;

        /* Triangle timer (low 8 bits)
        * $400A
        */
        case 0x400A:
            triangle.timer.period &= 0xFF00;
            triangle.timer.period |= data;
            break;

        /* Triangle length counter load + timer (high 3 bits)
        * $400B
        * 
        * Side effects: sets linear counter reload flag
        */
        case 0x400B:
            triangle.len.load(data >> 3);
            triangle.timer.period &= 0x00FF;
            triangle.timer.period |= (data & 0x07) << 8;
            triangle.lin.reload = true;
            break;

        // TODO noise & DMC

        /* APU control
        * $4015 (write)
        * 
        * 7 6 5 4   3 2 1 0
        * - - - D   N T 2 1
        * 
        * 1 - Pulse 1 enable
        * 2 - Pulse 2 enable
        * T - Triangle enable
        * N - Noise enable
        * D - DMC enable
        * 
        * Side effects: clears DMC interrupt flag
        */
        case 0x4015:
            pulse_1.enable = data & 0x01;
            pulse_2.enable = data & 0x02;
            triangle.enable = data & 0x04;
            // TODO noise channel enable
            // TODO DMC channel enable/complicated DMC behavior
            // TODO clear DMC interrupt flag
            break;
        
        /* APU frame counter
        * $4017
        * 
        * 7 6 5 4   3 2 1 0
        * M I - -   - - - -
        * 
        * I = IRQ inhibit flag
        * M = frame counter Mode
        */
        case 0x4017:
            irq_inhibit = data & 0x40;
            frame_count_mode = (data & 0x80) ? FIVE_STEP : FOUR_STEP;
            break;

        default:
            #ifndef NDEBUG
            std::cerr << "Warning: unsupported CPU write to " << hex(address) << std::endl;
            #endif
            break;
    }
}

void Pulse::clock()
{
    // Envelope gets clocked every quarter frame already
    // Sweep & length ctr get clocked every half frame already
    if (timer.clock()) 
    {
        seq.clock();
        seq.out = pulse_waveforms[duty][seq.sequence];
    }
    out = (!sweep.mute && seq.out && !len.silence) ? env.out : 0;
}

void Triangle::clock()
{
    if (timer.clock() || timer.clock())
    {
        if (!len.silence && !lin.silence)
        {
            seq.clock();
            seq.out = abs(seq.sequence);
        }
    }
    out = seq.out;
}

void LengthCounter::load(ubyte length) 
{ 
    count = length_table[length];
    silence = false;
}
void LengthCounter::clear() { count = 0; }

void LengthCounter::clock()
{
    if (enable && !halt && count > 0) count--;
    if (enable && count == 0) silence = true;
    // TODO not sure about this
    if (!enable) silence = false;
}

void LinearCounter::clock()
{
    if (reload) { count = reload_val; reload = false; silence = false; }
    else if (count > 0) count--;
    if (!control) reload = false;
    // TODO not sure about this
    if (count == 0) silence = true;
}

void Envelope::clock()
{
    if (start)
    {
        start = false;
        decay = 15;
        divider = period;
    }
    else // clock divider
    {
        divider--;
        if (divider < 0)
        {
            divider = period;
            if (decay > 0) decay--;
            else if (loop) decay = 15;
        }
    }
    // bits 0-3 of $4000/$4004 are the volume if cv
    out = cv ? period : decay;
}

void Sweep::clock(Timer& timer)
{
    if (divider <= 0)
    {
        int change = timer.period >> shift;
        if (negate)
        {
            // Pulse 1 -> one's comp, 2 -> 2's comp
            change = (ones_comp) ? -change - 1 : -change;
        }
        int target = timer.period + change;
        mute = (timer.period < 8) || (target > 0x7FF);
        if (enabled) timer.period = target;
    }
    if (divider <= 0 || reload)
    {
        divider = (period) >> 4;
        reload = false;
    }
}

void Timer::reload() { counter = period; }
bool Timer::clock()
{
    if (counter == 0) { counter = period; return true; }
    else { counter--; return false; }
}

//  Note: out value is handled externally, so I don't have to 
// create derived classes/overloads or pass around function pointers
void Sequencer::clock() { sequence = (sequence == seq_min) ? seq_max : sequence-1; }