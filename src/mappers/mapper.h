#pragma once

// TODO clang, MSVC, etc.
// #pragma GCC diagnostic warning "-Wunused-parameter"

#include "../globals.h"
#include "../savestate.h"

// TODO check nes20db for min/max RAM/ROM sizes

// https://wiki.nesdev.com/w/index.php/Mirroring#Nametable_Mirroring
enum class MirrorType
{
    HORIZONTAL,             // Horizontal or mapper-controlled (set by ROM header)
    VERTICAL,
    SINGLE_SCREEN_LOWER,    // lower or upper bank
    SINGLE_SCREEN_UPPER, 
    FOUR_SCREEN,
    OTHER                   // Mapper-controlled (set by mapper)
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
    virtual ~Mapper() {}
    virtual std::optional<ubyte> cpuRead(uword address) = 0;
    virtual bool cpuWrite(uword address, ubyte data) = 0;
    virtual std::optional<ubyte> ppuRead(uword address) = 0;
    virtual bool ppuWrite(uword address, ubyte data) = 0;
    // virtual bool save(Savestate& savestate) = 0; // Most mappers won't need these
    // virtual bool load(Savestate& savestate) = 0;
    // inline void tick() {return;} // Probably won't use this
    MirrorType mirroring = MirrorType::HORIZONTAL;
};

/* NROM
* http://wiki.nesdev.com/w/index.php/NROM
* Games: 248
* Notable games: 1942, Dig Dug (I & II), Donkey Kong (1, 3, and Jr.)
*   Duck Hunt, ExciteBike, Galaga, Ice Climber, Mario Bros., 
*   Ms. Pac-Man, Pac-Man, Super Mario Bros.
*/
class Mapper000 : public Mapper
{
public:
    Mapper000(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
    /* Program RAM
    * NOTE: only available in Family Basic mode
    * // TODO what in the fuck is Family Basic mode
    * capacity: $0800 or $1000
    * window:   $2000
    */
    std::vector<ubyte> prg_ram = {};
    bool prg_ram_enabled = false;

    /* Program ROM
    * capacity: $4000 or $8000
    * window:   $8000
    */
    std::vector<ubyte> prg_rom = {};

    /* Character ROM or RAM
    * capacity: $2000
    * window:   $2000
    */
    std::array<ubyte, 0x2000> chr_mem = {};

    /* Determines if chr_mem acts as CHR-RAM
    * Used for compatibility with some test roms and homebrew games
    */
    bool chr_ram = false;
};

/* MMC1
* http://wiki.nesdev.com/w/index.php/MMC1
* Games: 681
* Notable games: Barbie, Bomberman II, Bubble Bobble, Castlevania II, 
*   Double Dragon, Dr. Mario, Dragon Warrior (I-IV), Final Fantasy (I & II)
*   Kid Icarus, The Legend of Zelda, Mega Man 2, Metroid, Ninja Gaiden, 
*   TMNT, Tetris (Licensed), Zelda II
*/
class Mapper001 : public Mapper
{
public:
    Mapper001(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
    uint submapper = 0;

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
        ubyte reg = 0;
    } chr_bank[2] {};

// Banks
    /* Program RAM (optional)
    * capacity: $0 to $8000 (0-32 KiB)
    * banks:      0-4
    * window:   $2000
    */
    std::vector<std::array<ubyte,0x2000>> prg_ram = {};
    bool prg_ram_enabled = false;

    /* Program ROM
    * capacity: $8,000 to $80,000 (32-512 KiB)
    * banks:     2-32
    * window:   $4000 x 2
    */
    std::vector<std::array<ubyte,0x4000>> prg_rom = {};

    /* Character ROM/RAM
    * capacity: $2,000 to $20,000 (8-128KiB)
    * banks:     2-32
    * window:   $1000 x 2
    */
    std::vector<std::array<ubyte,0x1000>> chr_mem = {};

    bool chr_ram = false;
};

/* UxROM (except for 2 games)
* https://wiki.nesdev.com/w/index.php/UxROM
* Games: 271
* Notable Games: Archin, Castlevania, Contra, Ducktales (I & II), 
*   Ghosts 'n Goblins, Mega Man, Metal Gear, Paperboy 2, Prince of Persia, 
*   Super Pitfall
*/
class Mapper002 : public Mapper
{
public:
    Mapper002(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
// Banks
    /* Program ROM
    * capacity: $40,000 to $400,000 (256 to 4096 KiB)
    * banks:     10-100
    * window:   $4,000 x 2
    */
    std::vector<std::array<ubyte,0x4000>> prg_rom = {};
    uint prg_bank = 0; // location of the first PRG-ROM bank

    /* Character ROM/RAM
    * capacity: $2000
    * banks:     N/A
    * window:   $2000
    */
    std::array<ubyte,0x2000> chr_mem = {};
    bool chr_ram = false;
};

/* CNROM & similar boards
* https://wiki.nesdev.com/w/index.php/INES_Mapper_003
* Games: 155
* Notable Games: Adventure Island, Arkanoid, Donkey Kong Classics, 
*   Gradius, Joust, Paperboy, Tetris (Unlicensed)
*/
class Mapper003 : public Mapper
{
public:
    Mapper003(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
// Banks
    /* Program ROM
    * capacity: $4000 or $8000 (16 or 32 KiB)
    * banks:     1 (with mirroring) or 2 fixed
    * window:   $4000 x 2
    */
    std::vector<std::array<ubyte,0x4000>> prg_rom = {};

    /* Character ROM/RAM
    * capacity: $2,000 to $200,000 (8 KiB to 2MiB)
    * banks:     1 to 256
    * window:   $2000
    */
    std::vector<std::array<ubyte,0x2000>> chr_mem = {};
    bool chr_ram = false;
    uint chr_bank = 0;
};

/* MMC3 (TxROM), MMC6 (HKROM)
* https://wiki.nesdev.com/w/index.php/MMC3
* Games: 600
* Notable games: Adventure Island II, Bubble Bobble 2, Contra Force, 
*   Double Dragon (II & III), Jurassic Park, Kirby's Adventure, 
*   Mega Man (3-6), Mother (JP), Ninja Gaiden (II & III), Smash T.V.,
*   Star Wars, Star Wars: The Empire Strikes Back, Street Fighter 2010, 
*   Super Mario Bros. (2 & 3), TMNT (II, III, Tournament Fighters), Tetris 2
*   Wario's Woods, Yoshi's Cookie
*/
class Mapper004 : public Mapper
{
public:
    Mapper004(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
// Banks
    /* Program RAM
    * capacity: $0 or $2000 (0 or 8KiB)
    * banks:     0 or 1
    * window:   $2000
    */
    std::array<ubyte,0x2000> prg_ram = {};
    bool prg_ram_exists = false;
    bool prg_ram_enabled = true;
    bool prg_ram_write_protect = false; // TODO defaults?

    /* Program ROM
    * capacity: $8,000 to $80,000 (32KiB to 2048 KiB)
    * banks:     4 to 256
    * window:   $2000 x 4
    */
    std::vector<std::array<ubyte,0x2000>> prg_rom = {};
    std::array<uint,4> prg_bank = {};

    /* Character ROM/RAM
    * capacity: $8,000 to $40,000 (32KiB to 256 Kib)
    * banks:     8* to 64*
    * window:   $2000 x2 + $1000 x 4
    */
    std::vector<std::array<ubyte,0x1000>> chr_mem = {};
    bool chr_ram = false;
    std::array<uint,8> chr_bank = {};

// Registers
    /* Bank select ($8000-$9FFE, even)
    * 
    * 7 6 5 4   3 2 1 0
    * C P M -   - R R R
    * 
    * R - Specifies which bank register to update on next write to Bank Data reg
    *     R = 0 - Select 2 KiB CHR bank at $0000 (or $1000)
    *     R = 1 - Select 2 KiB CHR bank at $0800 (or $1800)
    *     R = 2 - Select 1 KiB CHR bank at $1000 (or $0000)
    *     R = 3 - Select 1 KiB CHR bank at $1400 (or $0400)
    *     R = 4 - Select 1 KiB CHR bank at $1800 (or $0800)
    *     R = 5 - Select 1 KiB CHR bank at $1C00 (or $0C00)
    *     R = 6 - Select 8 KiB PRG bank at $8000 (or $C000)
    *     R = 7 - Select 8 KiB PRG bank at $A000
    * M - //TODO MMC6 behavior
    * P - PRG-ROM bank mode
    *     0: $8000-$9FFF swappable, $C000-$DFFF fixed to second-last bank
    *     1: vice versa
    * C - CHR A12 inversion
    *     0: two 2KiB banks at $0000-$0FFF, four 1KiB banks at $1000-$1FFF
    *     1: vice versa
    */
    union
    {
        struct
        {
            unsigned bank           : 3;
            unsigned                : 2;
            unsigned mmc6           : 1;
            unsigned prg_bank_mode  : 1;
            unsigned chr_bank_mode  : 1;
        };
        ubyte reg;
    } reg_bank_select {.reg = 0};

    uint a12 = 0;
    int irq_counter = 0;
    ubyte irq_latch = 0;
    bool irq_reload = false;
    bool irq_enable = false;
};

// TODO mapper 5
/* MMC5
* https://wiki.nesdev.com/w/index.php/MMC5
* Games: 24
* Notable games: Castlevania III
*/
class Mapper005 : public Mapper
{
public:
    Mapper005(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
// Banks
    /* Program RAM
    * capacity: $0000 to $20,000 (0 to 128KiB) (note: might be 16K min)
    * banks:     0 to 16
    * window:   $2000 (at $6000), $4000 (at $8000 w/ PRG mode 1/2)
    */
    std::vector<std::array<ubyte,0x2000>> prg_ram = {};
    uint prg_ram_exists = false;

    /* Program ROM
    * capacity: $20,000 to $100,000 (128KiB to 1MiB)
    * banks:     16 to 128
    * window:   $2000, $4000, or $8000
    */
    std::vector<std::array<ubyte,0x2000>> prg_rom = {};

    /* Character ROM/RAM
    * capacity: $20,000 to $100,000 (128KiB to 1MiB)
    * banks:     256 to 2048
    * window:   $400, $800, $1000, or $2000
    */
    std::vector<std::array<ubyte,0x0400>> chr_mem = {};
// Registers
    uint prg_mode = 0; // 0-3
    uint chr_mode = 0; // 0-3
};

// TODO mapper 6
// Only used for a sound test ROM

/* AxROM
* https://wiki.nesdev.com/w/index.php/AxROM
* Games: 76
* Notable games: Battletoads, Battletoads & Double Dragon
*/
class Mapper007 : public Mapper
{
public:
    Mapper007(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
// Banks
    /* Program ROM
    * capacity: $8,000 to $80,000 (32KiB to 512KiB)
    *   note: can be smaller than 32K, in which case we need mirroring
    * banks:     1 to 16
    * window:   $8000
    */
    std::vector<std::vector<ubyte>> prg_rom = {};
    uint prg_bank = 0;

    /* Character ROM/RAM
    * capacity: <= $2000 (8KiB)
    * banks:     1
    * window:   $2000
    */
    std::vector<ubyte> chr_mem = {};
    bool chr_ram = false;
};

// TODO mapper 8
// Note: this mapper is considered "useless"

/* MMC2
* https://wiki.nesdev.com/w/index.php/MMC2
* Games: 11 (all versions of Mike Tyson's Punch Out!! and Punch Out!!)
*/
class Mapper009 : public Mapper
{
public:
    Mapper009(Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
private:
    /* Program RAM
    * capacity: $0000 or $2000 (0 or 8 KiB)
    */
    std::array<ubyte,0x2000> prg_ram = {};
    bool prg_ram_exists = false;

    /* Program ROM
    * capacity: $8000 to $20,000 (32 to 128 KiB)
    * banks:     4 to 16
    * window:   $2000 x 1 + $6000 x 1
    */
    std::vector<std::array<ubyte,0x2000>> prg_rom = {};
    uint prg_bank = 0; // Bank select for the first bank (last 3 fixed)

    /* Character ROM/RAM
    * capacity: $2000 to $20,000 (8 to 128 KiB)
    * banks:     2 to 32
    * window:   $1000 x 2
    */
    std::vector<std::array<ubyte,0x1000>> chr_mem = {};

    /* CHR-ROM/RAM bank selection
    * [0][0] - $0000 bank select when latch 0 = $FD
    * [0][1] - ''                             = $FE
    * [1][0] - $1000 bank select when latch 1 = $FD
    * [1][1] - ''                             = $FE
    */
    std::array<std::array<uint,2>,2> chr_bank = {};
    std::array<ubyte,2> chr_latch = {0xFD, 0xFD}; // not sure what the default is
    bool chr_ram = false;
};

// TODO mapper 10
// Includes "Famicom Wars" and two JP Fire Emblem games

/* Color Dreams
* https://wiki.nesdev.com/w/index.php/Color_Dreams
* Games: 31
* Notable games: riveting Christianity-themed titles including:
*   Baby Boomer, Bible Adventures, Joshua & the Battle of Jericho,
*   Spiritual Warfare, Sunday Funday: The Ride
*/
//class Mapper011 : public Mapper {};

// TODO mapper 12
// Probably only used by Dragon Ball Z 5

// TODO mapper 13
// Only used by Videomation

// TODO mappers 14
// Only used by an unlicensed Taiwanese game

// TODO mapper 15
// Used by two XXX-in-1 Konami boards

// TODO mapper 16
// Used by some Bandai boards

//