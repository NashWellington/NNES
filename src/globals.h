#pragma once

// Uncomment this in release builds to prevent assertions and logging
//#ifndef NDEBUG
//#define NDEBUG
//#endif

#include <iostream>         // Used for logging errors etc.
#include <stdexcept>        // exceptions
#include <assert.h>         // assertions
#include <fstream>          // Used for reading files
#include <chrono>           // Used for clocking
#include <memory>           // Needed for shared_ptr
#include <vector>           // needed for dynamic memory allocation
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

std::string hex(uword x);
std::string hex(ubyte x);
std::string hex(byte x);

std::string binary(byte x);