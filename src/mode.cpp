#include "mode.hpp"

// TODO dummy reads for indexed instructions

std::pair<uword,int> Mode::zeroPage(CPU& cpu, byte offset)
{
    uword address = static_cast<uword>(static_cast<ubyte>(cpu.nextByte() + offset));
    return std::make_pair(address, 3);
}

std::pair<uword,int> Mode::absolute(CPU& cpu, std::optional<ubyte> offset, bool read_instr)
{
    int cycles = 3;

    // Get two ubytes
    ubyte aa = cpu.nextByte();
    ubyte bb = cpu.nextByte();

    // Combine ubytes into uword w/ format bbaa to get un-indexed address
    uword address = (static_cast<uword>(bb) << 8) + static_cast<uword>(aa);

    // Indexed
    if (offset)
    {
        address += static_cast<uword>(offset.value());

        // Dummy read of address pre-fixing of high byte
        aa += offset.value();
        uword pre_fixed_addr = (static_cast<uword>(bb) << 8) + static_cast<uword>(aa);
        // TODO this seems to break controller input
        //cpu.read(pre_fixed_addr);

        // add one cycle if read instr and page cross
        if (address != pre_fixed_addr)
            if (read_instr) cycles += 1;
    }

    return std::make_pair(address, cycles);
}

std::pair<uword,int> Mode::indirect(CPU& cpu, ubyte offset, ubyte index_type, bool read_instr)
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
        post_indexed_address += static_cast<uword>(offset);
        
        // handle page boundary cross if read instr
        if (read_instr && (post_indexed_address >> 8 != address >> 8))
            cycles += 1;
    }

    return std::make_pair(post_indexed_address, cycles);
}


std::pair<ubyte,int> Mode::immediate(CPU& cpu)
{
    ubyte val = cpu.nextByte();
    return std::make_pair(val, 2);
}

std::pair<uword,int> Mode::relative(CPU& cpu)
{
    int cycles = 2;
    byte offset = static_cast<byte>(cpu.nextByte());
    uword pc = cpu.reg_pc;

    uword address = pc;

    // Sign extends when casting from (signed) byte to word
    address += static_cast<word>(offset);

    // check if page boundary is crossed
    if (address >> 8 != pc >> 8) 
        cycles += 1;

    return std::make_pair(address, cycles);
}