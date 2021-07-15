#pragma once

// Forward declaration
class CPU;

#include "globals.h"
#include "mode.h"
#include "cpu.h"

namespace ISA
{
// Interrupts

    int IRQ(CPU& cpu);
    int NMI(CPU& cpu);

// Instructions

    int executeOpcode(CPU& cpu, ubyte instr);

    /* ADd memory to accumulator with Carry
    * Type: read
    * A + M + C -> A, C
    * Flags: C, Z, V, N
    * addressing            opc	bytes	cycles
    * immediate	            69	2	    2  
    * zeropage              65	2	    3  
    * zeropage,X            75	2	    4 
    * absolute	            6D	3	    4  
    * absolute,X            7D	3	    4* 
    * absolute,Y            79	3	    4* 
    * (indirect,X)          61	2	    6  
    * (indirect),Y          71	2	    5* 
    */
    int ADC(CPU& cpu, int cycles, ubyte val);

    /* AND memory with accumulator
    * Type: read
    * A AND M -> A
    * Flags: Z, N
    * addressing            opc	bytes	cycles
    * immediate             29	2	    2  
    * zeropage	            25	2	    3  
    * zeropage,X            35	2	    4  
    * absolute	            2D	3	    4  
    * absolute,X	        3D	3	    4* 
    * absolute,Y	        39	3	    4* 
    * (indirect,X)          21	2	    6  
    * (indirect),Y          31	2	    5* 
    */
    int AND(CPU& cpu, int cycles, ubyte val);

    /* Accumulator (or memory) Shift Left one bit
    * Note: set address to 0 (or anything) if flag_accumulator is true
    * Type: read-modify-write
    * A or M << 1
    * Flags: C, Z, N
    * addressing            opc	bytes	cycles
    * accumulator           0A	1	    2  
    * zeropage	            06	2	    5  
    * zeropage,X	        16	2	    6  
    * absolute	            0E	3	    6  
    * absolute,X	        1E	3	    7  
    */
    int ASL(CPU& cpu, int cycles, uword address, ubyte val, bool flag_accumulator);

    /* Branch on Carry Clear
    * Branch if C = 0
    * Flags: none
    * addressing	        opc	bytes	cycles
    * relative	            90	2	    2**
    */
    int BCC(CPU& cpu, int cycles, uword address);

    /* Branch on Carry Set
    * Branch if C = 1
    * Flags: none
    * addressing	        opc	bytes	cycles
    * relative	            B0	2	    2**
    */
    int BCS(CPU& cpu, int cycles, uword address);

    /* Branch on EQual
    * Branch if Z = 1
    * Flags: none
    * addressing	        opc	bytes	cycles
    * relative	            F0	2	    2**
    */
    int BEQ(CPU& cpu, int cycles, uword address);

    /* test BITs
    * Type: read
    * A AND M, M7 -> N, M6 -> V
    * Flags: Z, V, N
    * addressing	        opc	bytes	cycles
    * zeropage	            24	2	    3  
    * absolute	            2C	3	    4  
    */
    int BIT(CPU& cpu, int cycles, ubyte val);

    /* Branch on MInus
    * Branch if N = 1
    * Flags: none
    * addressing	    	opc	bytes	cycles
    * relative	            30	2	    2**
    */
    int BMI(CPU& cpu, int cycles, uword address);

    /* Branch on Not Equal
    * Branch if Z = 0
    * Flags: none
    * addressing	    	opc	bytes	cycles
    * relative	            D0	2	    2**
    */
    int BNE(CPU& cpu, int cycles, uword address);

    /* Branch on PLus
    * Branch if N = 0
    * Flags: none
    * addressing	    	opc	bytes	cycles
    * relative	            10	2	    2**
    */
    int BPL(CPU& cpu, int cycles, uword address);

    /* BReaK
    * Read (then throw away) byte, then
    * Force NMI
    * Flags: B
    * addressing            opc bytes   cycles
    * implied               00  1       7
    */
    int BRK(CPU& cpu);

    /* Branch on oVerflow Clear
    * Branch if V = 0
    * Flags: none
    * addressing	    	opc	bytes	cycles
    * relative	            50	2	    2**
    */
    int BVC(CPU& cpu, int cycles, uword address);

    /* Branch on oVerflow Set
    * Branch if V = 1
    * Flags: none
    * addressing	    	opc	bytes	cycles
    * relative	            70	2	    2**
    */
    int BVS(CPU& cpu, int cycles, uword address);

    /* CLear Carry flag
    * C => 0
    * Flags: C
    * addressing		    opc	bytes	cycles
    * implied	            18  1	    2 
    */
    int CLC(CPU& cpu);

    /* CLear Decimal flag
    * Note: this does nothing for two cycles
    * addressing		    opc	bytes	cycles
    * implied	            D8  1	    2 
    */
    int CLD(CPU& cpu);

    /* CLear Interrupt disable bit
    * I => 0
    * Flags: I
    * addressing		    opc	bytes	cycles
    * implied	            D8  1	    2 
    */
    int CLI(CPU& cpu);

    /* CLear oVerflow flag
    * V => 0
    * Flags: V
    * addressing		    opc	bytes	cycles
    * implied	            B8  1	    2 
    */
    int CLV(CPU& cpu);

    /* CoMPare memory with accumulator
    * Type: read
    * A - M
    * Flags: C, Z, N
    * addressing	        opc	bytes	cycles
    * immediate	            C9	2	    2  
    * zeropage              C5	2	    3  
    * zeropage,X	        D5	2	    4  
    * absolute	            CD	3	    4  
    * absolute,X	        DD	3	    4* 
    * absolute,Y	        D9	3	    4* 
    * (indirect,X)	        C1	2	    6  
    * (indirect),Y	        D1	2	    5* 
    */
    int CMP(CPU& cpu, int cycles, ubyte val);

    /* CoMPare memory with register X
    * Type: read
    * X - M
    * Flags: C, Z, N
    * addressing	        opc	bytes	cycles
    * immediate	            E0	2	    2  
    * zeropage              E4	2	    3  
    * absolute	            EC	3	    4  
    */
    int CPX(CPU& cpu, int cycles, ubyte val);

    /* CoMPare memory with register Y
    * Type: read
    * Y - M
    * Flags: C, Z, N
    * addressing	        opc	bytes	cycles
    * immediate	            C0	2	    2  
    * zeropage              C4	2	    3  
    * absolute	            CC	3	    4  
    */
    int CPY(CPU& cpu, int cycles, ubyte val);

    /* DECrement memory by 1
    * Type: read-modify-write
    * M - 1 -> M
    * Flags: Z, N
    * addressing            opc	bytes	cycles
    * zeropage	            C6	2   	5  
    * zeropage,X	        D6	2   	6  
    * absolute	            CE	3   	6  
    * absolute,X	        DE	3   	7  
    */
    int DEC(CPU& cpu, int cycles, uword address);

    /* DEcrement X by 1
    * X - 1 -> X
    * Flags: Z, N
    * addressing            opc	bytes	cycles
    * implied	            CA	1   	2  
    */
    int DEX(CPU& cpu);

    /* DEcrement Y by 1
    * Y - 1 -> Y
    * Flags: Z, N
    * addressing            opc	bytes	cycles
    * implied	            88	1   	2  
    */
    int DEY(CPU& cpu);

    /* bitwise Exclusive-OR memory with accumulator
    * Type: read
    * A EOR M -> A
    * Flags: Z, N
    * addressing	    	opc	bytes	cycles
    * immediate	        	49	2   	2  
    * zeropage	        	45	2   	3  
    * zeropage,X	    	55	2   	4  
    * absolute	        	4D	3   	4  
    * absolute,X	    	5D	3   	4* 
    * absolute,Y	        59	3   	4* 
    * (indirect,X)	    	41	2   	6  
    * (indirect),Y	    	51	2   	5*
    */
    int EOR(CPU& cpu, int cycles, ubyte val);

    /* INCrement memory by 1
    * Type: read-modify-write
    * M + 1 -> M
    * Flags: Z, N
    * addressing            opc	bytes	cycles
    * zeropage	            E6	2   	5  
    * zeropage,X	        F6	2   	6  
    * absolute	            EE	3   	6  
    * absolute,X	        FE	3   	7  
    */
    int INC(CPU& cpu, int cycles, uword address);

    /* INCrement X by 1
    * X + 1 -> X
    * Flags: Z, N
    * addressing            opc	bytes	cycles
    * implied	            E8	1   	2  
    */
    int INX(CPU& cpu);

    /* INCrement Y by 1
    * Y + 1 -> Y
    * Flags: Z, N
    * addressing            opc	bytes	cycles
    * implied	            C8	1   	2  
    */
    int INY(CPU& cpu);

    /* JuMP to new location
    * M -> PC
    * Flags: none
    * addressing	    	opc	bytes	cycles
    * absolute	            4C	3   	3  
    * indirect	        	6C	3   	5 
    */
    int JMP(CPU& cpu, int cycles, uword address);

    /* Jump to SubRoutine
    * Type: read-modify-write
    * push (PC + 2)
    * M -> PC
    * Flags: none
    * addressing	       	opc	bytes	cycles
    * absolute	            20	3	    6
    */
    int JSR(CPU& cpu, uword address);

    /* LoaD Accumulator with memory
    * Type: read
    * M -> A
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * immediate	            A9	2   	2  
    * zeropage	            A5	2   	3  
    * zeropage,X	        B5	2   	4  
    * absolute	            AD	3   	4  
    * absolute,X	        BD	3   	4* 
    * absolute,Y	        B9	3   	4* 
    * (indirect,X)	       	A1	2	    6  
    * (indirect),Y	       	B1	2	    5* 
    */
    int LDA(CPU& cpu, int cycles, ubyte val);

    /* LoaD index X with memory
    * Type: read
    * M -> X
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * immediate	            A2	2   	2  
    * zeropage	            A6	2   	3  
    * zeropage,Y	        B6	2   	4  
    * absolute	            AE	3   	4  
    * absolute,Y	        BE	3   	4*
    */
    int LDX(CPU& cpu, int cycles, ubyte val);

    /* LoaD index Y with memory
    * Type: read
    * M -> Y
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * immediate	            A0	2   	2  
    * zeropage	            A4	2   	3  
    * zeropage,X	        B4	2   	4  
    * absolute	            AC	3   	4  
    * absolute,X	        BC	3   	4*
    */
    int LDY(CPU& cpu, int cycles, ubyte val);

    /* Logical Shift Right (memory or accumulator)
    * Type: read-modify-write
    * A or M >> 1
    * Flags: C, Z
    * addressing	        opc	bytes	cycles
    * accumulator	    	4A	1       2
    * zeropage	          	46	2   	5  
    * zeropage,X	      	56	2   	6  
    * absolute	          	4E	3   	6  
    * absolute,X	      	5E	3   	7 
    */
    int LSR(CPU& cpu, int cycles, uword address, ubyte val, bool flag_accumulator);

    // TODO split up unofficial NOPs
    /* No OPeration
    * Type: read
    * Note: this operation does nothing
    * Note: the only official NOP opcode is EA
    * addressing	    opc	                    bytes	cycles
    * implied           1A,3A,5A,7A,DA,EA,FA    1       2
    * immediate         80,82,89,C2,E2          2       2
    * absolute          0C                      3       4
    * absolute,X        1C,3C,5C,7C,DC,FC       3       4 or 5
    * zeropage          04,44,64                2       3
    * zeropage,X        14,34,54,74,D4,F4       2       4
    */
    // int NOP(CPU& cpu);

    /* OR memory with Accumulator
    * Type: read
    * A OR M -> A
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * immediate	            09	2	    2  
    * zeropage	            05	2	    3  
    * zeropage,X	        15	2	    4  
    * absolute	            0D	3	    4  
    * absolute,X	        1D	3	    4* 
    * absolute,Y	        19	3	    4* 
    * (indirect,X)	        01	2	    6  
    * (indirect),Y	        11	2	    5* 
    */
    int ORA(CPU& cpu, int cycles, ubyte val);

    /* PusH Accumulator onto stack
    * push A
    * addressing	        opc	bytes	cycles
    * implied	            48	1       3
    */
    int PHA(CPU& cpu);

    /* PusH Processor status onto stack
    * push SR
    * addressing	        opc	bytes	cycles
    * implied	            08	1       3
    */
    int PHP(CPU& cpu);

    /* PuLl Accumulator from stack
    * pull A
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * implied	            68	1       4
    */
    int PLA(CPU& cpu);

    /* PuLl Processor status from stack
    * pull SR
    * addressing	        opc	bytes	cycles
    * implied	            28	1       4
    */
    int PLP(CPU& cpu);

    /* Rotate One bit Left
    * Type: read-modify-write
    * C <- [76543210] <- C
    * Flags: C, Z, N
    * addressing	        opc	bytes	cycles
    * accumulator	        2A	1	    2  
    * zeropage	            26	2	    5  
    * zeropage,X	        36	2	    6  
    * absolute	            2E	3	    6  
    * absolute,X	        3E	3	    7
    */
    int ROL(CPU& cpu, int cycles, uword address, ubyte val, bool flag_accumulator);

    /* Rotate One bit Right
    * Type: read-modify-write
    * C -> [76543210] -> C
    * Flags: C, Z, N
    * addressing	        opc	bytes	cycles
    * accumulator	        6A	1	    2  
    * zeropage	            66	2	    5  
    * zeropage,X	        76	2	    6  
    * absolute	            6E	3	    6  
    * absolute,X	        7E	3	    7
    */
    int ROR(CPU& cpu, int cycles, uword address, ubyte val, bool flag_accumulator);

    /* ReTurn from Interrupt
    * pull SR, pull PC
    * Flags: all
    * addressing            opc bytes   cycles
    * implied               40  1       6
    */
    int RTI(CPU& cpu);

    /* ReTurn from Subroutine
    * pull PC, PC++
    * Flags: none
    * addressing            opc bytes   cycles
    * implied               60  1       6
    */
    int RTS(CPU& cpu);

    /* SuBtract memory from accumulator with Carry (borrow)
    * Type: read
    * A - M - (1 - C) -> A
    * Flags: C, Z, V, N
    * addressing	        opc	    bytes	cycles
    * immediate	            E9	    2	    2  
    * zeropage	            E5	    2	    3  
    * zeropage,X	        F5	    2	    4  
    * absolute	            ED	    3	    4  
    * absolute,X	        FD	    3	    4* 
    * absolute,Y	        F9	    3	    4* 
    * (indirect,X)	        E1,EB   2	    6  
    * (indirect),Y	        F1	    2	    5*
    * Note: EB is an unofficial instruction
    */
    int SBC(CPU& cpu, int cycles, ubyte val);

    /* SEt Carry flag
    * 1 -> C
    * addressing	        opc	bytes	cycles
    * implied	            38	1	    2 
    */
    int SEC(CPU& cpu);

    /* SEt Decimal flag
    * NOTE: not implemented on 2A03
    * 1 -> D
    * addressing	        opc	bytes	cycles
    * implied	            F8	1	    2 
    */
    int SED(CPU& cpu);

    /* SEt Interrupt disable status
    * 1 -> I
    * addressing	        opc	bytes	cycles
    * implied	            78	1	    2 
    */
    int SEI(CPU& cpu);

    /* Store Accumulator in Memory
    * Type: write
    * A -> M
    * Flags: none
    * addressing	        opc	bytes	cycles
    * zeropage	            85	2	    3  
    * zeropage,X	        95	2	    4  
    * absolute	            8D	3	    4  
    * absolute,X	        9D	3	    5  
    * absolute,Y	        99	3	    5  
    * (indirect,X)	        81	2	    6  
    * (indirect),Y	        91	2	    6
    */
    int STA(CPU& cpu, int cycles, uword address);

    /* Store Index X in Memory
    * Type: write
    * X -> M
    * Flags: none
    * addressing	        opc	bytes	cycles
    * zeropage	            86	2	    3  
    * zeropage,Y	        96	2	    4  
    * absolute	            8E	3	    4
    */
    int STX(CPU& cpu, int cycles, uword address);

    /* Store Index Y in Memory
    * Type: write
    * Y -> M
    * Flags: none
    * addressing	        opc	bytes	cycles
    * zeropage	            84	2	    3  
    * zeropage,X	        94	2	    4  
    * absolute	            8C	3	    4
    */
    int STY(CPU& cpu, int cycles, uword address);

    /* Transfer Accumulator to index X
    * A -> X
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * implied	            AA	1	    2
    */
    int TAX(CPU& cpu);

    /* Transfer Accumulator to index Y
    * A -> Y
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * implied	            A8	1	    2
    */
    int TAY(CPU& cpu);

    /* Transfer Stack pointer to index X
    * SP -> X
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * implied	            BA	1	    2
    */
    int TSX(CPU& cpu);

    /* Transfer index X to Accumulator
    * X -> A
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * implied	            8A	1	    2
    */
    int TXA(CPU& cpu);

    /* Transfer index X to Stack pointer
    * X -> SP
    * Flags: none
    * addressing	        opc	bytes	cycles
    * implied	            9A	1	    2
    */
    int TXS(CPU& cpu);

    /* Transfer index Y to Accumulator
    * Y -> A
    * Flags: Z, N
    * addressing	        opc	bytes	cycles
    * implied	            98	1	    2
    */
    int TYA(CPU& cpu);

// Unofficial instructions

    /* And + LsR accumulator
    * AKA "ASR"
    * M and A -> A
    * LSR A -> A
    * Flags: C, Z, N (handled by AND and ALR ops)
    * addressing	        opc	    bytes	cycles
    * immediate             4B      2       2
    */
    int ALR(CPU& cpu, int cycles, ubyte val);

    /* ANd, set C
    * M and A -> A
    * N -> C
    * Flags: C, Z, N (Z and N handled by AND)
    * addressing	        opc	    bytes	cycles
    * immediate             0B,2B   2       2
    */
    int ANC(CPU& cpu, int cycles, ubyte val);

    /* And + RoR accumulator
    * M and A -> A
    * ROR A -> A
    * bit 6 -> C
    * bit 6 xor bit 5 -> V
    * Flags: C, Z, V, N
    * addressing	        opc	    bytes	cycles
    * immediate             6B      2       2
    */
    int ARR(CPU& cpu, int cycles, ubyte val);

    /* Accumulator and index X with Subtraction
    * AKA "SBX"
    * (A and X) - M -> X
    * Flags: C, Z, N
    * addressing	        opc	    bytes	cycles
    * immediate             CB      2       2
    */
    int AXS(CPU& cpu, int cycles, ubyte val);

    /* Dec + CmP
    * M - 1 -> M
    * A - M
    * Flags: C, Z, N
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          C3      2       8
    * zeropage              C7      2       5
    * absolute              CF      3       6
    * (indirect),Y          D3      2       8
    * zeropage,X            D7      2       6
    * absolute,Y            DB      3       7
    * absolute,X            DF      3       7
    */
    int DCP(CPU& cpu, int cycles, uword address);

    /* Inc + SbC
    * AKA "ISB"
    * M + 1 -> M
    * A - M - (1 - C) -> A
    * Flags: C, Z, V, N
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          E3      2       8
    * zeropage              E7      2       5
    * absolute              EF      3       6
    * (indirect),Y          F3      2       8
    * zeropage,X            F7      2       6
    * absolute,Y            FB      3       7
    * absolute,X            FF      3       7
    */
    int ISC(CPU& cpu, int cycles, uword address);

    /* Lda + tAX
    * M -> A -> X
    * Type: read
    * Flags: Z, N
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          A3	    2	    6
    * zeropage              A7      2       3
    * absolute              AF      3       4
    * (indirect),Y          B3      2       5
    * zeropage,Y            B7      2       4
    * absolute,Y            BF      3       4
    */
    int LAX(CPU& cpu, int cycles, ubyte val);

    /* Load to index X and Accumulator
    * AKA "ATX"
    * val -> A -> X
    * NOTE: this has weirder behavior on an actual system
    * Type: read
    * Flags: N, Z
    * addressing	        opc	    bytes	cycles
    * immediate             AB	    2	    2
    */
    int LXA(CPU& cpu, int cycles, ubyte val);

    /* RoL + And
    * Flags: C, Z, N
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          23      2       8
    * zeropage              27      2       5
    * absolute              2F      3       6
    * (indirect),Y          33      2       8
    * zeropage,X            37      2       6
    * absolute,Y            3B      3       7
    * absolute,X            3F      3       7
    */
    int RLA(CPU& cpu, int cycles, uword address);

    /* RoR + Adc
    * Flags: C, Z, N
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          63      2       8
    * zeropage              67      2       5
    * absolute              6F      3       6
    * (indirect),Y          73      2       8
    * zeropage,X            77      2       6
    * absolute,Y            7B      3       7
    * absolute,X            7F      3       7
    */
    int RRA(CPU& cpu, int cycles, uword address);

    /* Store bitwise and of the Accumulator and register X
    * A and X -> M
    * Type: write
    * Flags: none
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          83      2       6
    * zeropage              87      2       3
    * absolute              8F      3       4
    * zeropage,Y            97      2       4
    */
    int SAX(CPU& cpu, int cycles, uword address);

    /* Store High address bits and Accumulator and register X in memory
    * (A & X & (ADDR_HI + 1)) -> M
    * Type: write
    * //TODO flags
    * addressing	        opc	    bytes	cycles
    * (indirect),Y          93      2       6
    * absolute,Y            9F      3       5
    * //TODO testing
    */
    int SHA(CPU& cpu, int cycles, uword address);

    /* SHa + txS (where X is replaced by (A & X))
    * (A & X) -> SP????
    * Type: read
    * //TODO flags
    * addressing	        opc	    bytes	cycles
    * absolute,Y            9B      2       4
    * //TODO testing
    */
    int SHS(CPU& cpu, int cycles, uword address);

    /* Store High address bits and register X in memory
    * AKA "SXA"
    * (X & (ADDR_HI + 1)) -> M
    * Type: write
    * addressing	        opc	    bytes	cycles
    * absolute,Y            9E      2       5
    */
    int SHX(CPU& cpu, int cycles, uword address);

    /* Store High address bits and register Y in memory
    * AKA "SYA"
    * (Y & (ADDR_HI + 1)) -> M
    * Type: write
    * addressing	        opc	    bytes	cycles
    * absolute,X            9C      2       5
    */
    int SHY(CPU& cpu, int cycles, uword address);

    /* aSL + Ora
    * Flags: C, Z, N
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          03      2       8
    * zeropage              07      2       5
    * absolute              0F      3       6
    * (indirect),Y          13      2       8
    * zeropage,X            17      2       6
    * absolute,Y            1B      3       7
    * absolute,X            1F      3       7
    */
    int SLO(CPU& cpu, int cycles, uword address);

    /* lSR + Eor
    * Flags: C, Z, N
    * addressing	        opc	    bytes	cycles
    * (indirect,X)          43      2       8
    * zeropage              47      2       5
    * absolute              4F      3       6
    * (indirect),Y          53      2       8
    * zeropage,X            57      2       6
    * absolute,Y            5B      3       7
    * absolute,X            5F      3       7
    */
    int SRE(CPU& cpu, int cycles, uword address);
};