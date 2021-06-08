#include "globals.h"

std::string hex(uword x)
{
    std::string hex_string = "    ";
    sprintf(hex_string.data(), "%04X", x);
    return hex_string;
}

std::string hex(ubyte x)
{
    std::string hex_string = "  ";
    sprintf(hex_string.data(), "%02X", x);
    return hex_string;
}

std::string hex(byte x)
{
    return hex(static_cast<ubyte>(x));
}

// TODO add addressing mode support
std::string decompile(uword address, std::vector<byte> ops)
{
    std::string return_string = hex(address) + ": " + instructions[ops[0]];
}