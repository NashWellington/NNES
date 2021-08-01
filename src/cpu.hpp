#pragma once

// Forward declaration
class NES;

// Include
#include "globals.hpp"
#include "processor.hpp"
#include "mem.hpp"
#include "savestate.hpp"

class CPU : public Processor
{
public:
    CPU(NES& _nes) : nes(_nes) {}
    void setRegion(Region _region);

    ubyte read(uword address);
    void write(uword address, ubyte data);

    ubyte pop();
    void push(ubyte data);

    /* Return the byte pointed at by PC and increment PC
    */
    ubyte nextByte();

    // Read the next byte, but don't increment PC
    void dummyNextByte();

    void tick();

    /* Set PC to the value located in the reset vector
    */
    void start();

    /* Reset CPU registers
    */
    void reset();

    void save(Savestate& savestate);
    void load(Savestate& savestate);

    NES& nes;

    // CPU registers

    /* Program Counter
    * 16-bit register 
    * Holds the address of the next instruction executed
    */
    uword reg_pc = 0xFFFC;

    /* Stack Pointer
    * 8-bit register 
    * Used as an offset from $0100
    * No stack overflow detection (will wrap around)
    */
    ubyte reg_sp = 0xFD;

    /* Accumulator
    * 8-bit register
    * Stores the result of arithmetic and logic operations
    */
    ubyte reg_a = 0;

    /* Index Register X
    * 8-bit register
    * Counter or offset for certain addressing modes
    * Can be used to get or set the stack pointer
    */
    ubyte reg_x = 0;

    /* Index Register Y
    * 8-bit register
    * Counter or offset
    * Cannot affect the stack pointer
    */
    ubyte reg_y = 0;

    /* Status Register
    *
    * 7 6 5 4   3 2 1 0
    * N V   B   D I Z C
    * 
    * C - Carry Flag
    *     Set if the last instructed resulted in overflow from bit 7 or underflow from bit 0
    * Z - Zero Flag
    *     Set if the last instructed resulted in a 0
    * I - Interrupt Disable
    *     Prevents the system from responding to IRQs
    *     Set by SEI (Set Interrupt Disable) instruction
    *     Unset by CLI (Clear Interrupt Disable) instruction
    * D - Decimal Mode
    *     Not implemented on the 2A03 (see documentation)
    * B - Break Command
    *     Indicates break instruction has been executed
    *     Causes IRQ
    *     Note: this one isn't a real flag, and has quirky behavior. Read the docs
    * V - Overflow Flag
    *     Set if the last instruction resulted in an invalid two's complemented result
    * N - Negative Flag
    *     Set if sign bit (TODO: of what?) is 1 (meaning negative)
    */
    union
    {
        struct 
        {
            unsigned c : 1;
            unsigned z : 1;
            unsigned i : 1;
            unsigned d : 1;
            unsigned b : 1;
            unsigned u : 1; // "unused" bit that is sometimes set or cleared
            unsigned v : 1;
            unsigned n : 1;
        };
        ubyte reg;
    } reg_sr = { .reg = 0x34 };

    enum class Interrupt
    {
        NONE = 0,
        IRQ  = 1,
        NMI  = 2
    };

    /* Add a new interrupt to the queue
    */
    void queueInterrupt(Interrupt interrupt);

private:
    bool odd_cycle = false;

// Interrupts
    Interrupt serviced_interrupt = Interrupt::NONE;
    Interrupt queued_interrupt = Interrupt::NONE;

    void serviceInterrupt();
    void clearInterrupt();

    void BRK() noexcept;
    void IRQ() noexcept;
    void NMI() noexcept;

// Instructions

    struct
    {
        ubyte code = 0;
        uint cycle = 1;     // Cycle within the instruction
        uword address = 0;  // Address to read from/write to
        ubyte value = 0;    // Value to be written to the address in a RMW instruction
        bool page_cross = false;
        bool branch_taken = false;
    } instr = {};

    /* Call nextByte() and execute that byte as an instruction
    */
    void executeInstruction();

// Instructions accessing the stack
    void RTI() noexcept;
    void RTS() noexcept;
    void PHA() noexcept;
    void PHP() noexcept;
    void PLA() noexcept;
    void PLP() noexcept;
    void JSR() noexcept;
    void JMP() noexcept;
    void JMP_IND() noexcept;

// Branch instructions
    bool BCC() noexcept;
    bool BCS() noexcept;
    bool BNE() noexcept;
    bool BEQ() noexcept;
    bool BPL() noexcept;
    bool BMI() noexcept;
    bool BVC() noexcept;
    bool BVS() noexcept;

// Read instructions
    void ADC(ubyte value) noexcept;
    void ALR(ubyte value) noexcept; // (aka ASR) unintended op
    void ANC(ubyte value) noexcept; // (aka ANC2, ANA, ANB) unintended op
    void AND(ubyte value) noexcept;
    void ANE(ubyte value) noexcept; // (aka XAA, AXM) unstable/magic const op
    void ARR(ubyte value) noexcept; // unintended op
    void BIT(ubyte value) noexcept;
    void CMP(ubyte value) noexcept;
    void CPX(ubyte value) noexcept;
    void CPY(ubyte value) noexcept;
    void EOR(ubyte value) noexcept;
    void LAS(ubyte value) noexcept; // (aka LAR) unstable op
    void LAX(ubyte value) noexcept; // unstable op
    void LDA(ubyte value) noexcept;
    void LDX(ubyte value) noexcept;
    void LDY(ubyte value) noexcept;
    void LXA(ubyte value) noexcept; // (aka LAX #imm, ATX, OAL, ANX) unstable/magic const op
    void NOP(ubyte value) noexcept;
    void ORA(ubyte value) noexcept;
    void SBC(ubyte value) noexcept;
    void SBX(ubyte value) noexcept; // (aka AXS, SAX, XMA) unintended op

// Write instructions
    [[nodiscard]] ubyte SAX() noexcept; // (aka AXS, AAX) unintended op
    [[nodiscard]] ubyte SHA() noexcept; // (aka AXA, AHX, TEA) unstable op
    [[nodiscard]] ubyte SHX() noexcept; // (aka A11, SXA, XAS, TEX) unstable op
    [[nodiscard]] ubyte SHY() noexcept; // (aka A11, SYA, SAY, TEY) unstable op
    [[nodiscard]] ubyte STA() noexcept;
    [[nodiscard]] ubyte STX() noexcept;
    [[nodiscard]] ubyte STY() noexcept;
    [[nodiscard]] ubyte TAS() noexcept; // (aka XAS, SHS) unstable op

// Read-Modify-Write instructions
    [[nodiscard]] ubyte ASL(ubyte value) noexcept;
    [[nodiscard]] ubyte DCP(ubyte value) noexcept; // (aka DCM) unintended op
    [[nodiscard]] ubyte DEC(ubyte value) noexcept;
    [[nodiscard]] ubyte INC(ubyte value) noexcept;
    [[nodiscard]] ubyte ISC(ubyte value) noexcept; // (aka ISB, INS) unintended op
    [[nodiscard]] ubyte LSR(ubyte value) noexcept;
    [[nodiscard]] ubyte RLA(ubyte value) noexcept; // (aka RLN) unintended op
    [[nodiscard]] ubyte ROL(ubyte value) noexcept;
    [[nodiscard]] ubyte ROR(ubyte value) noexcept;
    [[nodiscard]] ubyte RRA(ubyte value) noexcept; // (aka RRD) unintended op
    [[nodiscard]] ubyte SLO(ubyte value) noexcept; // (aka ASO) unintended op
    [[nodiscard]] ubyte SRE(ubyte value) noexcept; // (aka LSE) unintended op

// Other (mostly paired with implied/no addressing mode)
    void CLC() noexcept;
    void CLD() noexcept;
    void CLI() noexcept;
    void CLV() noexcept;
    void DEX() noexcept;
    void DEY() noexcept;
    void INX() noexcept;
    void INY() noexcept;
    void NOP() noexcept;
    void SEC() noexcept;
    void SED() noexcept;
    void SEI() noexcept;
    void SEV() noexcept;
    void TAX() noexcept;
    void TAY() noexcept;
    void TSX() noexcept;
    void TXA() noexcept;
    void TXS() noexcept;
    void TYA() noexcept;

    void STP(); // aka KIL, JAM
};