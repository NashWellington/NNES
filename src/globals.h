#ifndef NNES_GLOBALS_H
#define NNES_GLOBALS_H

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
#include <iomanip>          // Used for converting to hex //TODO ios?
#include <fstream>          // Used for reading files
#include <cmath>            // Used for exponential functions // TODO just replace w/ bitshifting
#include <chrono>           // Used for clocking
#include <memory>           // Needed for shared_ptr
#include <vector>           // needed for dynamic memory allocation
#include <limits>           // needed for overflow detection
#include <optional>         // for determining number of operands

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


/* A global struct for keeping track of state values in each cycle
* This will be used for logging states
*/
struct State
{
    // CPU registers
    uword reg_PC = 0;
    ubyte reg_SP = 0;
    byte reg_A = 0;
    byte reg_X = 0;
    byte reg_Y = 0;
    byte reg_SR = 0;

    unsigned long long int cycle = 0;
    std::optional<byte> ops[3] = {};    // opcode (index 0) and operands

    #ifdef LOG_INSTR
    std::string instruction = "";       // instruction name shorthand

    /* Addressing mode
    *   0 - implied
    *   1 - accumulator
    *   2 - absolute
    *   3 - absolute, x-indexed
    *   4 - absolute, y-indexed
    *   5 - immediate
    *   6 - indirect
    *   7 - x-indexed, indirect aka pre-indexed indirect
    *   8 - indirect, y-indexed aka post-indexed indirect
    *   9 - relative
    *   10 - zero page
    *   11 - zero page, x-indexed
    *   12 - zero page, y-indexed
    */
    int mode = 0;                
    #endif

    // TODO maybe log PPU values?

    void addOp(byte op);

    #ifdef LOG_INSTR
    /* Logs the instruction shorthand, e.g. "LDX #$00"
    */
    void addInstr(std::string instruction, int mode);
    #endif
    void clearState();
};

extern State Current_State;

/* The number of times the main loop is run per frame
*/
const int SUBFRAMES_PER_FRAME = 1;

// TODO deprecate
class InvalidOpcodeException : public std::exception
{
public:
    InvalidOpcodeException (ubyte opcode, uword address) 
        : opcode(opcode), address(address) {};
    ubyte opcode;
    uword address;
};

// TODO make a function for converting hex to string
// << overloads
//std::ostream& operator<<(std::ostream& strm, uword data);
std::ostream& operator<<(std::ostream& strm, byte data);
std::ostream& operator<<(std::ostream& strm, ubyte data);
std::ostream& operator<<(std::ostream& strm, InvalidOpcodeException& e);
std::ostream& operator<<(std::ostream& strm, State& state);


#endif