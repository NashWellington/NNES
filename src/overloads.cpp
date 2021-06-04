#include "globals.h"

// TODO delete or move to savestate

std::ostream& operator<<(std::ostream& strm, State& state)
{
    // example line: C000  4C F5 C5  A:00 X:00 Y:00 P:24 SP:FD CYC:  0

    strm << std::setfill('0') << std::right << std::setw(4) << std::uppercase << std::hex << int(state.reg_PC);
    strm << "  ";
    for (auto& op : state.ops)
    {
        if (op) strm << op.value() << ' ';
        else strm << "   ";
    }
    strm << ' ';

    #ifdef DISP_INSTR
    // Example: JSR $C72D                       
    // = instr + 1 space, then 28 spaces

    strm << state.instruction << ' ';

    std::string mode_string = "";
    std::stringstream hex_buffer;

    switch(state.mode)
    {
        case 1: // accumulator -> "A"
            mode_string = "A";
            break;

        case 2: // absolute -> "$LLHH"
            hex_buffer << state.ops[2].value() << state.ops[1].value();
            mode_string += "$" + hex_buffer.str();
            break;

        case 3: // absolute, x-indexed -> "$LLHH,X"
            hex_buffer << state.ops[2].value() << state.ops[1].value();
            mode_string += "$" + hex_buffer.str() + ",X";
            break;

        case 4: // absolute, y-indexed -> "$LLHH,Y"
            hex_buffer << state.ops[2].value() << state.ops[1].value();
            mode_string += "$" + hex_buffer.str() + ",Y";
            break;

        case 5: // immediate -> "#$BB"
            hex_buffer << state.ops[1].value();
            mode_string += "#$" + hex_buffer.str();
            break;

        case 6: // indirect -> "($LLHH)"
            hex_buffer << state.ops[2].value() << state.ops[1].value();
            mode_string += "($" + hex_buffer.str() + ")";
            break;

        case 7: // x-indexed, indirect -> "($LL,X)"
            hex_buffer << state.ops[1].value();
            mode_string += "($" + hex_buffer.str() + ",X)";
            break;

        case 8: // indirect, y-indexed -> "($LL),Y"
            hex_buffer << state.ops[1].value();
            mode_string += "($" + hex_buffer.str() + "),Y";
            break;

        case 9: // indirect, relative -> "$BB"
            hex_buffer << state.ops[1].value();
            mode_string += "$" + hex_buffer.str();
            break;

        case 10: // zero page -> "$LL"
            hex_buffer << state.ops[1].value();
            mode_string += "$" + hex_buffer.str();
            break;

        case 11: // zero page, x-indexed -> "$LL,X"
            hex_buffer << state.ops[1].value();
            mode_string += "$" + hex_buffer.str() + ",X";
            break;

        case 12: // zero page, y-indexed -> "$LL,Y"
            hex_buffer << state.ops[1].value();
            mode_string += "$" + hex_buffer.str() + ",Y";
            break;

        default: // should just be implied -> ""
            break;

    }

    strm << std::left << std::setfill(' ') << std::setw(10) << mode_string;

    #endif

    strm << std::right;
    strm << "A:" << state.reg_A << ' ';
    strm << "X:" << state.reg_X << ' ';
    strm << "Y:" << state.reg_Y << ' ';
    strm << "P:" << state.reg_SR << ' ';
    strm << "SP:" << state.reg_SP << ' ';
    strm << "CYC:";
    strm << std::setfill(' ') << std::setw(3) << std::dec;
    strm << state.cycle;
    strm << std::left;          // TODO figure out if this is necessary
    strm << std::endl;

    return strm;
}

// TODO move to a better file
void State::addOp(byte b)
{
    for (auto& op : ops)
    {
        if (!op)
        {
            op = std::make_optional<byte>(b);
            break;
        }
    }
}

#ifdef LOG_INSTR
void State::addInstr(std::string instruction, int mode)
{
    this->instruction = instruction;
    this->mode = mode;
}
#endif

void State::clearState()
{
    reg_PC = 0;
    reg_SP = 0;
    reg_A = 0;
    reg_X = 0;
    reg_Y = 0;
    reg_SR = 0;

    for (auto& op : ops)
    {
        op = {};
    }

    #ifdef LOG_INSTR
    instruction = "";
    mode = 0;
    #endif
}

// TODO move to another module
State Current_State;