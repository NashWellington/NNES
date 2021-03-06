#pragma once

#include "../globals.hpp"
#include "../savestate.hpp"

#include "../console.hpp"

#include <algorithm> // Needed to .fill(0) PRG-RAM after resets

// TODO check nes20db for min/max RAM/ROM sizes

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
    virtual void reset() = 0;
    MirrorType mirroring = MirrorType::HORIZONTAL;

    void mirrorNametables(uword& address);
// protected:
//     virtual void saveToFile(uword address, ubyte data) = 0;

//     // Should only be called once per boot
//     // TODO also called at reset?
//     virtual void loadFromFile(std::string name) = 0;
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
    void reset() 
    {
        std::fill(prg_ram.begin(), prg_ram.end(), 0);
        if (chr_ram) chr_mem.fill(0);
    }
private:
    /* Program RAM
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
    ~Mapper001();
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
    void reset() 
    {
        uint v_ram_end = (nv_ram_i < 0) ? prg_ram.size() : nv_ram_i;
        std::for_each(prg_ram.begin(), prg_ram.begin() + v_ram_end, [](auto& a){ a.fill(0); });
        if (chr_ram) std::for_each(chr_mem.begin(), chr_mem.end(), [](auto& a){ a.fill(0); });
    }
private:
    void saveToFile(uword address, ubyte data);
    void loadFromFile(std::string name);
    std::string save_name = "";
    std::fstream save_file;

    uint submapper = 0;

// Registers
    byte reg_shift = 0x10;

    /* Controls PRG-ROM bank switching
    * 0, 1: switch 32KiB at $8000
    * 2: fix first bank at $8000 and switch 16KiB at $C000
    * 3: fix last bank at $C000 and switch 16KiB at $8000
    */
    ubyte prg_rom_bank_mode = 3;

    /* Controls CHR-ROM bank switching
    * 0: switch one 8KiB bank uses prg_bank_0 for indices
    * 1: switch two 4KiB banks uses prg_bank_0 and prg_bank_1 for indices
    */
    ubyte chr_bank_mode = 0;

    /* Controls PRG-ROM and PRG-RAM banking
    * bits 0-3: selects 16K PRG-ROM bank (low bit ignored in 32K mode)
    * bit 4: disable PRG-RAM (ignored on MMC1A)
    */
    ubyte reg_prg_bank = 0;

    /* Controls CHR-ROM/RAM banking and a few other things
    * bit 0: chr bank select (ignored in 8K mode)
    * bit 1: chr bank select (CHR >= 16K)
    * bit 2: chr bank select (CHR >= 32K)
    *        prg-ram bank select (PRG-RAM = 32K)
    * bit 3: chr bank select (CHR >= 64K)
    *        prg-ram bank select (PRG-RAM >= 16K)
    *        Note: bits 2 and 3 get switched when selecting PRG-RAM
    * bit 4: chr bank select (CHR = 128K)
    *        prg-rom bank select (PRG-ROM = 512K)
    *        prg-ram bank select (SZROM only; ignores bits 2&3)
    *        prg-ram disable (SNROM only; OR's with reg_prg_bank bit 4)
    */
    ubyte reg_chr_bank_0 = 0;

    // Same as above, except everything is ignored in 8K mode
    ubyte reg_chr_bank_1 = 0;

// Banks
    /* Program RAM (optional)
    * capacity: $0 to $8000 (0-32 KiB)
    * banks:      0-4
    * window:   $2000
    */
    std::vector<std::array<ubyte,0x2000>> prg_ram = {};
    int nv_ram_i = -1; // Index of the first PRG-RAM bank that's battery-backed

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
    void reset() { if (chr_ram) chr_mem.fill(0); }
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
    void reset() { if (chr_ram) std::for_each(chr_mem.begin(), chr_mem.end(), [](auto& a){ a.fill(0); }); }
private:
// Banks
    /* Program ROM
    * capacity: $4000 or $8000 (16 or 32 KiB)
    * banks:     1 (with mirroring) or 2 fixed
    * window:   $4000 x 2
    */
    std::vector<std::array<ubyte,0x4000>> prg_rom = {};

    /* Program RAM
    *  Exactly 1 mapper 3 game uses PRG-RAM,
    * but some test ROMs also use it
    */
    std::array<ubyte,0x2000> prg_ram = {};
    bool prg_ram_exists = false;

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
    Mapper004(NES& _nes, Header& header, std::ifstream& rom);
    std::optional<ubyte> cpuRead(uword address);
    bool cpuWrite(uword address, ubyte data);
    std::optional<ubyte> ppuRead(uword address);
    bool ppuWrite(uword address, ubyte data);
    void reset() 
    {
        prg_ram.fill(0);
        if (chr_ram) std::for_each(chr_mem.begin(), chr_mem.end(), [](auto& a){ a.fill(0); });
    }
private:
    NES& nes;
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
    * window:   $0800 x2 + $0400 x 4
    */
    std::vector<std::array<ubyte,0x0400>> chr_mem = {};
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
    void reset()
    {
        std::for_each(prg_ram.begin(), prg_ram.end(), [](auto& a){ a.fill(0); });  
    }
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
    void reset()
    {
        if (chr_ram) std::fill(chr_mem.begin(), chr_mem.end(), 0);
    }
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
    void reset()
    {
        prg_ram.fill(0);
        if (chr_ram) std::for_each(chr_mem.begin(), chr_mem.end(), [](auto& a){ a.fill(0); });
    }
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