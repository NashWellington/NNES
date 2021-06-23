#pragma once

#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic warning "-Wsign-conversion"

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

typedef uint8_t ubyte;
typedef int8_t byte;
typedef uint16_t uword;
typedef int16_t word;

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
    return hex_string;
}

template<typename T>
std::string binary(const T x)
{
    std::string bin_str = "";
    for (int i = 0; i < static_cast<int>(8*sizeof(T)); i++)
    {
        bin_str += (x & (1 << static_cast<int>(8*sizeof(T)-1-i)) ? '1' : '0');
    }
    return bin_str;
}