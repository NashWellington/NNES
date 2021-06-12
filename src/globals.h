#pragma once

// Uncomment this in release builds to prevent assertions and cerr logging
//#ifndef NDEBUG
//#define NDEBUG
//#endif

// Comment this out if building as NNES
// Uncomment this out if building as NNES - debugger
// TODO change this (and NDEBUG) to bemake flags or something
#ifndef DEBUGGER
#define DEBUGGER
#endif

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