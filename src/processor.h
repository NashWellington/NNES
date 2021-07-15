#pragma once

#include "globals.h"
#include "savestate.h"

// TODO move this to Scheduler
/* Master clock cycles per processer cycle
*        PPU | CPU | APU
* NTSC:   4     12    24   
* PAL:    5     16    32
* Dendy:  5     15    30
*/

// TODO experimental abstract class for processors to interact with the scheduler
enum class Region
{
    NTSC,
    PAL,
    Dendy
};

class Processor
{
public:
    virtual ~Processor() {}
    virtual void reset() = 0;
    virtual void  tick() = 0;
    // virtual void save(Savestate& savestate) = 0;
    // virtual void load(Savestate& savestate) = 0;
    virtual void setRegion(Region _region) = 0;
    std::string name = "";
    Region region = Region::NTSC;
    uint64_t time_scale = 0; // Master clock cycles per processor cycle
    uint64_t cycle = 0;
};