#pragma once

// These catch a lot of false positives from integer promotion
// Switch these from "ignored" to "warning" every once in a while to catch bugs
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"

// Uncomment this in release builds to prevent assertions and cerr logging
//#ifndef NDEBUG
//#define NDEBUG
//#endif

// Comment this out if building as NNES
// Uncomment this out if building as NNES - debugger
// TODO cmake argument
#ifndef DEBUGGER
//#define DEBUGGER
#endif

#include <iostream>         // Used for logging errors etc.
#include <stdexcept>        
#include <assert.h>         
#include <fstream>          
#include <chrono>           
#include <memory>           // Needed for shared_ptr
#include <vector>           
#include <optional>         
#include <queue>
#include <thread>

/* 1-byte unsigned integer
* Used by default (as opposed to byte)
*/
typedef uint8_t ubyte;

/* 1-byte signed integer
* Used mostly for signed addition or for setting/clearing zero flag
*/
typedef int8_t byte;

/* 2-byte unsigned integer
* Used by default (usually for addresses)
*/
typedef uint16_t uword;

/* 2-byte signed integer
* Never or almost never used
*/
typedef int16_t word;

/* Used for timing
* Duration of one frame
* // TODO actual exact value
* // TODO PAL/Dendy vals
*/
typedef std::chrono::duration<int64_t,std::ratio<1,60>> frame;

enum InterruptType
{
    NO_INTERRUPT,   // no interrupt
    IRQ,            // maskable interrupt
    NMI,            // non-maskable interrupt
    RESET
};

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