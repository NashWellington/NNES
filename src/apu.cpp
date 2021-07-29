#include "apu.hpp"

// Channel constants (mostly lookup tables)
/* The number of APU cycles in four PPU frames
* (Note: I would do APU cycles per frame, but that wouldn't be an integer)
*/
const uint MAX_CYCLE = 59561;

// Used by the pulse channels' sequencers
const std::array<std::array<int,8>,4> pulse_waveforms =
{
    0, 1, 0, 0, 0, 0, 0, 0,    // 12.5%
    0, 1, 1, 0, 0, 0, 0, 0,    // 25%
    0, 1, 1, 1, 1, 0, 0, 0,    // 50%
    1, 0, 0, 1, 1, 1, 1, 1     // 25% negated
};

// Used when loading length counters
const std::array<int,32> length_table = 
{//    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
/*00-0F*/  10,254, 20,  2, 40,  4, 80,  6,160,  8, 60, 10, 14, 12, 26, 14,
/*10-1F*/  12, 16, 24, 18, 48, 20, 96, 22,192, 24, 72, 26, 16, 28, 32, 30
};

// TODO these are just NTSC values; figure out a good way to incorporate PAL vals too
// Used to determine the noise channel's period
const std::array<int,16> noise_period_lookup = 
{
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

// Rate that DMC refills buffer (in CPU cycles)
// TODO PAL
const std::array<int,16> rate_lookup = 
{
    428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
};

// TODO let SDL handle resampling???

// Envelopes/linear counter clocked every qurter frame
#define clockQF() {\
pulse_1.env.clock();\
pulse_2.env.clock();\
noise.env.clock();\
triangle.lin.clock();\
}

// Length counters/sweep units clocked every half frame
#define clockHF() {\
pulse_1.len.clock();\
pulse_2.len.clock();\
triangle.len.clock();\
noise.len.clock();\
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
    // Reset reg $4015
    write(0x4015, 0);
    // Rewrite reg $4017 with val previously written
    ubyte data = 0;
    if (interrupt_inhibit) data |= 0x40;
    if (frame_count_mode) data |= 0x80;
    write(0x4017, data);
    frame_interrupt = false;
    // APU should act like 4017 was written to 9-12 cycles before start/reset
    frame_ctr = 2;
    triangle.seq.sequence = 0;
    dmc.out &= 1;
    noise.timer.shift_reg = 1;
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
                if (!interrupt_inhibit) frame_interrupt = true;
                clockQF()
                clockHF() 
                frame_ctr = 0;
            }
            break;
            
        case 18641: // APU cycle 18640.5 -> step 5
            clockQF()
            clockHF()
            frame_ctr = 0;
            break;

        default:
            break;
    }
    assert(frame_ctr < 18641);
    frame_ctr++;
    cycle++;

    // TODO do they not get clocked at all if not enabled?
    pulse_1.clock();
    pulse_2.clock();
    triangle.clock();
    noise.clock();
    dmc.clock(nes);

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
                  + (static_cast<float>(noise.out) / 12241.0f) 
                  + (static_cast<float>(dmc.out) / 22638.0f);
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
    if (pulse_1.len.count > 0) data |= 0x01;
    if (pulse_2.len.count > 0) data |= 0x02;
    if (triangle.len.count > 0) data |= 0x04;
    if (noise.len.count > 0) data |= 0x08;
    if (dmc.bytes_remaining > 0) data |= 0x10;
    if (frame_interrupt) data |= 0x40;
    if (dmc.irq_enable) data |= 0x80;
    frame_interrupt = false;
    // TODO simultaneous interrupt flag set read behavior
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
            if (pulse_1.len.enabled) pulse_1.len.load(data >> 3);
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
            if (pulse_2.len.enabled) pulse_2.len.load(data >> 3);
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

        // $4009 unused

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
            if (triangle.len.enabled) triangle.len.load(data >> 3);
            triangle.timer.period &= 0x00FF;
            triangle.timer.period |= (data & 0x07) << 8;
            triangle.lin.reload = true;
            break;

        /* Noise channel control
        * $400C
        * 
        * 7 6 5 4   3 2 1 0
        * - - L C   V V V V
        * 
        * V - volume/envelope divider period
        * C - constant volume
        * L - length counter halt or loop envelope flag
        */
        case 0x400C:
            noise.env.period = data & 0x0F;
            noise.env.cv = data & 0x10;
            noise.env.loop = data & 0x20;
            noise.len.halt = data & 0x20;
            break;

        /* Noise channel mode and period
        * $400E
        * 
        * 7 6 5 4   3 2 1 0
        * M - - -   P P P P
        * 
        * P - timer period
        * M - noise channel mode
        */
        case 0x400E:
            noise.timer.period = noise_period_lookup[data & 0x0F];
            noise.timer.mode = data & 0x80;
            break;

        /* Noise channel length counter load
        * $400F
        * 
        * 7 6 5 4   3 2 1 0
        * l l l l   l - - -
        * 
        * l - length counter load
        * 
        * Side effects: envelope start flag set
        */
        case 0x400F:
            if (noise.len.enabled) noise.len.load(data >> 3);
            noise.env.start = true;
            break;

        /* DMC flags and rate
        * $4010
        * 
        * 7 6 5 4   3 2 1 0
        * I L - -   R R R R
        * 
        * R - rate: determines number of CPU cycles before changing
        *           output level
        * L - loop
        * I - IRQ enable
        */
        case 0x4010:
            dmc.timer.period = rate_lookup[data & 0x0F]/2; // lookup table is for CPU cycles
            dmc.loop = data & 0x40;
            dmc.irq_enable = data & 0x80;
            if (!dmc.irq_enable) dmc.interrupt = false;
            break;

        // TODO occasional improper level change
        /* DMC direct load
        * $4011
        * 
        * 7 6 5 4   3 2 1 0
        * - D D D   D D D D
        * 
        * D - direct load: sets output level
        */
        case 0x4011:
            dmc.out = data & 0x7F;
            break;

        /* DMC sample address
        * $4012
        */
        case 0x4012:
            dmc.sample_addr = 0xC000 + data * 64;
            break;

        /* DMC sample length
        * $4013
        */
        case 0x4013:
            dmc.sample_len = data * 16 + 1;
            break;

        /* APU control
        * $4015 (write)
        * 
        * 7 6 5 4   3 2 1 0
        * - - - D   N T 2 1
        * 
        * 1 - Pulse 1 enable/clear length counter
        * 2 - Pulse 2   ''
        * T - Triangle  ''
        * N - Noise     ''
        * D - DMC     enable/clear
        * 
        * Side effects: clears DMC interrupt flag
        */
        case 0x4015:
            pulse_1.len.enabled = data & 0x01;
            if (!pulse_1.len.enabled) pulse_1.len.count = 0;
            pulse_2.len.enabled = data & 0x02;
            if (!pulse_2.len.enabled) pulse_2.len.count = 0;
            triangle.len.enabled = data & 0x04;
            if (!triangle.len.enabled) triangle.len.count = 0;
            noise.len.enabled = data & 0x08;
            if (!noise.len.enabled) noise.len.count = 0;
            if (data & 0x10)    // enable DMC
            {
                dmc.start = true;
            }
            else                // clear DMC
            {
                dmc.bytes_remaining = 0;
                // DMC should silence itself once the buffer empties
            }
            // TODO I'm 90% sure irq_enable shouldn't get cleared here
            // dmc.irq_enable = false;
            dmc.interrupt = false;
            break;
        
        /* APU frame counter
        * $4017
        * 
        * 7 6 5 4   3 2 1 0
        * M I - -   - - - -
        * 
        * I = IRQ inhibit flag
        * M = frame counter Mode
        * 
        * Side effects: after 3-4 CPU clock cycles, timer is reset
        *               if mode flag set, quarter+half frame signals generated
        */
        case 0x4017:
            interrupt_inhibit = data & 0x40;
            if (interrupt_inhibit) frame_interrupt = false;
            if (data & 0x80)
            {
                frame_count_mode = FIVE_STEP;
                clockQF()
                clockHF()
            }
            else
            {
                frame_count_mode = FOUR_STEP;
            }
            // TODO buffer
            frame_ctr = 0;
            break;

        default:
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
    out = (!sweep.mute && seq.out && len.count > 0) ? env.out : 0;
}

void Triangle::clock()
{
    if (timer.clock() || timer.clock())
    {
        if (len.count > 0 && lin.count > 0)
        {
            seq.clock();
            // Can't just take the absolute value because 0 occurs twice in a row
            out = seq.sequence < 0 ? -seq.sequence + 1 : seq.sequence;
        }
    }
}

void Noise::clock()
{
    out = !timer.clock() && len.count > 0 ? env.out : 0;
}

void DMC::clock(NES& nes)
{
    // Read memory
    // TODO weird DMC DMA behavior
    if (!sample_buf && bytes_remaining > 0)
    {
        // TODO DMA takes longer than one cycle
        sample_buf = nes.mem->cpuRead(curr_addr);
        if (curr_addr == 0xFFFF) curr_addr = 0x8000;
        else curr_addr++;
        bytes_remaining--;
    }
    if (bytes_remaining == 0)
    {
        if (loop || start)
        {
            curr_addr = sample_addr;
            bytes_remaining = sample_len;
            start = false;
        }
        else if (irq_enable) interrupt = true;
    }

    // Output
    if (timer.clock())
    {
        // 1. Change output level
        if (!silence)
        {
            if (shift_reg & 0x01) // bit 0 set -> add 2
            {
                out = out > 125 ? out : out + 2; // prevent overflow
            }
            else                  // bit 0 clear -> subtract 2
            {
                out = out < 2 ? out : out - 2; // prevent underflow
            }
        }

        // 2. Clock shift register
        shift_reg >>= 1;        

        // 3. Decrement bits-remaining counter, possibly start new cycle
        assert(bits_remaining <= 8);
        if (bits_remaining > 1) bits_remaining--;
        else if (bits_remaining <= 1)
        {
            bits_remaining = 8;
            if (!sample_buf) silence = true;
            else
            {
                shift_reg = sample_buf.value();
                sample_buf = {};
                silence = false;
            }
        }
    }
}

void LengthCounter::load(ubyte length) 
{
    assert(enabled);
    count = length_table[length];
}

void LengthCounter::clock()
{
    if (!halt && count > 0) count--;
}

void LinearCounter::clock()
{
    if (reload) count = reload_val;
    else if (count > 0) count--;
    if (!control) reload = false;
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

void Sweep::clock(Divider& timer)
{
    if (enabled)
    {
        count--;
        if (count < 0)
        {
            int change = timer.period >> shift;
            if (negate)
            {
                // Pulse 1 -> one's comp, 2 -> 2's comp
                change = (ones_comp) ? -change - 1 : -change;
            }
            timer.period += change;
            if (timer.period < 8 || timer.period > 0x7FF) // TODO also mute if target > 0x7FF before?
            {
                mute = true;
                enabled = false;
            }
        }
        if (count < 0 || reload)
        {
            count = period;
            reload = false;
        }
    }
}

bool Divider::clock()
{
    assert(period >= 0);
    if (counter == 0) { counter = period; return true; }
    else { counter--; return false; }
}

bool LFSR::clock()
{
    assert(shift_reg != 0);
    uword feedback;
    if (counter > 0) counter--;
    else
    {
        counter = period;
        if (shift_reg == 0) feedback = 1;
        else feedback = (shift_reg & 1) ^ (mode ? (shift_reg & 0x40) >> 6 : (shift_reg & 0x02) >> 1);
        shift_reg >>= 1;
        shift_reg |= feedback << 14;
    }
    return shift_reg & 1;
}

//  Note: out value is handled externally, so I don't have to 
// create derived classes/overloads or pass around function pointers
void Sequencer::clock() { sequence = (sequence == seq_min) ? seq_max : sequence-1; }