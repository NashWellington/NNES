#include "mode.h"


std::pair<uword,int> Mode::zeroPage(byte offset)
{
    uword address = static_cast<uword>(static_cast<ubyte>(cpu.nextByte() + offset));
    return std::make_pair(address, 3);
}

std::pair<uword,int> Mode::absolute(byte offset, bool read_instr)
{
    int cycles = 3;

    // Get two ubytes
    ubyte aa = cpu.nextByte();
    ubyte bb = cpu.nextByte();

    // Combine ubytes into uword w/ format bbaa to get un-indexed address
    uword unindexed_addr = (static_cast<uword>(bb) << 8) + static_cast<uword>(aa);

    // add or subtract offset
    uword address = unindexed_addr;
    address += static_cast<uword>(static_cast<ubyte>(offset));

    // add one cycle if read instr and page cross
    if (read_instr && (address >> 8 != unindexed_addr >> 8))
        cycles += 1;

    return std::make_pair(address, cycles);
}

std::pair<uword,int> Mode::indirect(byte offset, ubyte index_type, bool read_instr)
{
    int cycles = 5;
    uword address = 0;

    // cannot be INDIRECT_X and INDIRECT_Y
    assert(index_type < 3);

    // Set up the first address (i.e. pointer to the real address)
    ubyte bb = cpu.nextByte();
    ubyte cc = 0;

    // Two operands -> first address
    if (index_type == 0)
    {
        cc = cpu.nextByte();
    }
    // If pre-indexed, first address will always be on zero page
    else if (index_type == 1)
    {
        bb += offset;   // note: zero page boundary cross intentionally NOT handled
    }

    // Combine to get actual address
    ubyte xx = cpu.read((static_cast<uword>(cc) << 8) + static_cast<uword>(bb));
    // Note: page boundary crossing is intentionally NOT handled
    bb += 1;
    ubyte yy = cpu.read((static_cast<uword>(cc) << 8) + static_cast<uword>(bb));

    // address = yyxx
    address = (static_cast<uword>(yy) << 8) + static_cast<uword>(xx);

    uword post_indexed_address = address;

    // post-indexing
    if (index_type == 2)
    {
        post_indexed_address += static_cast<uword>(static_cast<ubyte>(offset));
        
        // handle page boundary cross if read instr
        if (read_instr && (post_indexed_address >> 8 != address >> 8))
            cycles += 1;
    }

    return std::make_pair(post_indexed_address, cycles);
}


std::pair<byte,int> Mode::immediate()
{
    ubyte val = cpu.nextByte();
    return std::make_pair(val, 2);
}

std::pair<uword,int> Mode::relative()
{
    int cycles = 3;     // TODO is this really 2?
    byte offset = static_cast<byte>(cpu.nextByte());
    uword pc = cpu.reg_pc;

    // add or subtract offset
    uword address = pc;
    address += static_cast<uword>(offset);  // casts positives to 00xx and negatives to FFxx

    // check if page boundary is crossed
    if (address >> 8 != pc >> 8) 
        cycles += 1;

    return std::make_pair(address, cycles);
}