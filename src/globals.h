#pragma once

// Uncomment this in release builds to prevent assertions and logging
//#ifndef NDEBUG
//#define NDEBUG
//#endif

//#ifndef NDEBUG
//#define DISP_INSTR
//#endif

#ifdef DISP_INSTR
#define LOG_INSTR(instruction, mode) Current_State.addInstr(instruction, mode)
#else
#define LOG_INSTR(instruction, mode)
#endif

#include <iostream>         // Used for logging errors etc.
#include <stdexcept>        // exceptions
#include <assert.h>         // assertions
#include <iomanip>          // Used for converting to hex //TODO ios? // TODO hex function
#include <fstream>          // Used for reading files
#include <cmath>            // Used for exponential functions // TODO just replace w/ bitshifting
#include <chrono>           // Used for clocking
#include <memory>           // Needed for shared_ptr
#include <vector>           // needed for dynamic memory allocation
#include <optional>         // for determining number of operands // TODO
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