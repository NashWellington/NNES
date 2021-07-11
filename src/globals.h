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