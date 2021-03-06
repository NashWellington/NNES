#pragma once

// These catch a lot of false positives from integer promotion
// Switch these from "ignored" to "warning" every once in a while to catch bugs
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

// Uncomment this in release builds to prevent assertions and cerr logging
//#ifndef NDEBUG
#define NDEBUG
//#endif

#include <iostream>
#include <stdexcept>
#include <assert.h>
#include <fstream>
#include <chrono>
#include <memory>
#include <vector>
#include <optional>
#include <queue>
#include <thread>
#include <fmt/core.h>
#include <fmt/format.h>

/* 1-byte unsigned integer
* Used by default (as opposed to byte)
*/
typedef uint8_t ubyte;

/* 1-byte signed integer
* Used for casting to word with sign extension
*/
typedef int8_t byte;

/* 2-byte unsigned integer
* Used by default (usually for addresses)
*/
typedef uint16_t uword;

/* 2-byte signed integer
* Used for sign extending bytes
*/
typedef int16_t word;

/* Used for timing
* Duration of one frame
* // TODO actual exact value
* // TODO PAL/Dendy vals
*/
typedef std::chrono::duration<uint64_t,std::ratio<1,60>> frame;

enum class HeaderType
{
    NO_HEADER,  // Not an iNES, NES 2.0, or UNIF header
    INES,
    NES20,
    UNIF        // Currently unimplemented
};

/* Master clock cycles per processer cycle
*        PPU | CPU | APU
* NTSC:   4     12    24   
* PAL:    5     16    32
* Dendy:  5     15    30
*/
enum class Region
{
    NTSC,
    PAL,
    OTHER,
    Dendy
};

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

struct Header
{
    std::string name = "";

    HeaderType type = HeaderType::NO_HEADER;
    MirrorType mirroring = MirrorType::HORIZONTAL;
    Region region = Region::NTSC;

    uint mapper = 0;
    uint submapper = 0;
    bool trainer = false;
    bool battery = false;  // Battery-backed save
    uint64_t prg_rom_size = 0;
    uint64_t chr_rom_size = 0;
    uint64_t prg_ram_size = 0;
    uint64_t prg_nv_ram_size = 0;
    uint64_t chr_ram_size = 0;
    uint64_t chr_nv_ram_size = 0;
    uint misc_rom_num = 0; // Number of misc ROMs present

    void print(std::ostream& stream)
    {
        stream << "Mapper: " << mapper << std::endl;
        stream << "Submapper: " << submapper << std::endl;
        stream << "Battery? " << battery << std::endl;
        stream << "PRG-ROM size: " << prg_rom_size << std::endl;
        stream << "PRG-RAM size: " << prg_ram_size << std::endl;
        stream << "PRG-NV-RAM size: " << prg_nv_ram_size << std::endl;
        stream << "CHR-ROM size: " << chr_rom_size << std::endl;
        stream << "CHR-RAM size: " << chr_ram_size << std::endl;
        stream << "CHR-NV-RAM size: " << chr_nv_ram_size << std::endl;
        stream << "Misc ROMs: " << misc_rom_num << std::endl;
    }
};

struct Pixel
{
    Pixel() {}
    Pixel(ubyte red, ubyte green, ubyte blue) : r(red), g(green), b(blue) {}
    ubyte r = 0;
    ubyte g = 0;
    ubyte b = 0;
};

template<typename T>
std::string hex(const T x)
{
    std::string hex_string = {};
    sprintf(hex_string.data(), "%0*X", static_cast<int>(2*sizeof(T)), x);
    return {hex_string.data()};
}

template<typename T>
std::string binary(const T x)
{
    std::string bin_str = "";
    for (int i = 0; i < static_cast<int>(8*sizeof(T)); i++)
        bin_str += (x & (1 << static_cast<int>(8*sizeof(T)-1-i)) ? '1' : '0');
    return bin_str;
}