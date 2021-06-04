#pragma once

#pragma GCC diagnostic warning "-Wunused-parameter"

#include "globals.h"

extern Mapper mapper;

enum class MirrorType
{
    HORIZONTAL,
    VERTICAL         // TODO there are more types of mirroring
};

enum class HeaderType
{
    NO_HEADER,       // Not an iNES, NES 2.0, or UNIF header
    INES_HEADER,
    NES20_HEADER,
    UNIF_HEADER      // Currently unimplemented
};

struct Header
{
    // TODO reorder these
    HeaderType type = HeaderType::NO_HEADER;
    int mapper = 0;
    uint64_t prg_rom_size = 0;
    uint64_t chr_rom_size = 0;
    uint64_t prg_ram_size = 0;
    bool trainer = false;
    MirrorType mirroring; // TODO it's probably a little more complicated than this
};

class Mapper
{
public:
    std::optional<byte> cpuRead(uword address);
    bool cpuWrite(uword address, byte data);
    std::optional<byte> ppuRead(uword address);
    bool ppuWrite(uword address, byte data);
    MirrorType mirroring = MirrorType::HORIZONTAL;
};

// TODO move derived mappers to their own cpp files (but keep this .h file)

/* NROM
* http://wiki.nesdev.com/w/index.php/NROM
*/
class Mapper000 : public Mapper
{
public:
    Mapper000(Header& header, std::ifstream& rom);
    std::optional<byte> cpuRead(uword address);
    bool cpuWrite(uword address, byte data);
    std::optional<byte> ppuRead(uword address);
    bool ppuWrite(uword address, byte data);
private:
    /* Program RAM
    * NOTE: only available in Family Basic mode
    * // TODO what in the fuck is Family Basic mode
    * capacity: $0800 or $1000
    * window:   $2000
    * location: $6000
    */
    std::vector<byte> prg_ram = {};
    bool prg_ram_exists = false;

    /* Program ROM
    * capacity: $4000 or $8000
    * window:   $8000
    * location: $8000
    */
    std::vector<byte> prg_rom = {};

    /* Character ROM
    * capacity: $2000
    * window:   $2000
    * location: $0000
    */
    std::array<byte, 0x2000> chr_rom = {};
};

/* MMC1
* http://wiki.nesdev.com/w/index.php/MMC1
*/
class Mapper001 : public Mapper
{
public:
    Mapper001(Header& header, std::ifstream& rom);
    std::optional<byte> cpuRead(uword address);
    bool cpuWrite(uword address, byte data);
    std::optional<byte> ppuRead(uword address);
    bool ppuWrite(uword address, byte data);
private:
// Registers
    //TODO

// Banks
    /* Program RAM (optional)
    * capacity: $8000
    * window:   $2000
    * location: $6000
    */
    std::array<byte, 0x8000> prg_ram = {};
    bool prg_ram_exists = false;

    /* Program ROM
    * capacity: $40000 or $80000 (256K or 512K)
    * window:   $4000 x 2
    * location: $8000
    */
    std::vector<byte> prg_rom = {};

    /* Character ROM
    * capacity: $20000 (128K)
    * window:   $1000 x 2
    * location: $0000
    */
    std::vector<byte> chr_rom = {};
};