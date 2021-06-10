#pragma once

#pragma GCC diagnostic warning "-Wunused-parameter"

#include "../globals.h"
#include "../savestate.h"

// https://wiki.nesdev.com/w/index.php/Mirroring#Nametable_Mirroring
enum class MirrorType
{
    HORIZONTAL,
    VERTICAL,
    SINGLE_SCREEN_LOWER, // lower or upper bank
    SINGLE_SCREEN_UPPER, 
    FOUR_SCREEN,
    OTHER           // TODO
};

enum class HeaderType
{
    NONE,       // Not an iNES, NES 2.0, or UNIF header
    INES,
    NES20,
    UNIF        // Currently unimplemented
};

struct Header
{
    HeaderType type = HeaderType::NONE;
    uint mapper = 0;
    uint submapper = 0;
    bool trainer = false;
    uint64_t prg_rom_size = 0;
    uint64_t chr_rom_size = 0;
    uint64_t prg_ram_size = 0;
    uint64_t chr_ram_size = 0;
    MirrorType mirroring;
};

class Mapper
{
public:
    virtual std::optional<byte> cpuRead(uword address) = 0;
    virtual bool cpuWrite(uword address, byte data) = 0;
    virtual std::optional<byte> ppuRead(uword address) = 0;
    virtual bool ppuWrite(uword address, byte data) = 0;
    bool save(Savestate& savestate) { return false; } // Most mappers won't need these
    bool load(Savestate& savestate) { return false; }
    MirrorType mirroring = MirrorType::HORIZONTAL;
};

// TODO move derived mappers to their own cpp files (but keep this .h file)
// TODO Submappers

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
    bool prg_ram_enabled = false;

    /* Program ROM
    * capacity: $4000 or $8000
    * window:   $8000
    * location: $8000
    */
    std::vector<byte> prg_rom = {};

    /* Character ROM or RAM
    * capacity: $2000
    * window:   $2000
    * location: $0000
    */
    std::array<byte, 0x2000> chr_mem = {};

    /* Determines if chr_mem acts as CHR-RAM
    * Used for compatibility with some test roms and homebrew games
    */
    bool chr_ram = false;
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
    byte reg_shift = 0x10;

    /* Controls PRG-ROM bank switching
    * 0, 1: switch 32KiB at $8000
    * 2: fix first bank at $8000 and switch 16KiB at $C000
    * 3: fix last bank at $C000 and switch 16KiB at $8000
    */
    ubyte prg_rom_bank_mode = 0;

    /* Controls CHR-ROM bank switching
    * 0: switch one 8KiB bank uses prg_bank_0 for indices
    * 1: switch two 4KiB banks uses prg_bank_0 and prg_bank_1 for indices
    */
    ubyte chr_bank_mode = 0;

    ubyte prg_rom_bank = 0; // Used w/ prg_bank_mode to determine PRG-ROM banking

    ubyte prg_ram_bank = 0;

    // https://wiki.nesdev.com/w/index.php/MMC1#Variants
    union
    {
        struct
        {
            union
            {
                struct // default
                {
                    unsigned chr_bank : 5;
                };
                struct // SNROM
                {
                    unsigned : 4;
                    
                    /* Toggles PRG-RAM
                    * Only used if //TODO when?
                    */
                    unsigned        e : 1;
                };
                struct // SOROM, SUROM, SXROM
                {
                    unsigned : 2;

                    union
                    {
                        struct
                        {
                            /* Selects 8KiB PRG-RAM bank
                            * Only used if PRG-RAM size == 32KiB
                            * //TODO should I reverse this?
                            */
                            unsigned       ss : 2;
                        };
                        struct
                        {
                            unsigned : 1;
                            /* Selects 8KiB PRG-RAM bank
                            * Only used if PRG-RAM size == 16KiB
                            */
                            unsigned        s : 1;
                        };
                    };
                    /* Selects 256 PRG-ROM bank
                    * Only if PRG-ROM size == 512KiB
                    */
                    unsigned        p : 1;
                };
            };
            unsigned : 3; // unused
        };
        byte reg = 0;
    } chr_bank[2] {};

// Banks
    /* Program RAM (optional)
    * capacity: $0 to $8000 (0-32 KiB)
    * banks:      0-4
    * window:   $2000
    * location: $6000
    */
    std::vector<std::array<byte,0x2000>> prg_ram = {};
    bool prg_ram_enabled = false;

    /* Program ROM
    * capacity: $8000 to $80000 (32-512 KiB)
    * banks:     2-16
    * window:   $4000 x 2
    * location: $8000
    */
    std::vector<std::array<byte,0x4000>> prg_rom = {};

    /* Character ROM/RAM
    * capacity: $2000 to $20000 (8-128KiB)
    * banks:     2-16
    * window:   $1000 x 2
    * location: $0000
    */
    std::vector<std::array<byte,0x1000>> chr_mem = {};

    bool chr_ram = false;
};