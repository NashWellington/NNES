This NES program tests the illegal instructions XAA (also called ANE, opcode $8B) and LAX ($AB).
According to [1], these two opcodes are combinations of an immediate and an implied command, and involve a
highly unstable 'magic constant', which is chip and/or temperature dependent. See also [2].

To change between XAA and LAX test, press the button SELECT. To start, press A.


PHASE 1:

The first test consists of executing XAA #$ff with A = 0 and X = #$ff, in order to get the "magic" number [1].
In the case of LAX, this test executes LAX #$ff with A = 0.
During the first phase, the screen is divided in 3 parts:

+-----------+
|     1     |
+-----------+
|     2     |
+-----------+
|     3     |
+-----------+

Part 1 shows the magic numbers reached, part 2 and 3 show the (lower and upper) number of hits.
For instance, if magics #$AA and #$BB are reached then, in that positions of part 1, these numbers
are shown:

PART1 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
   +------------------------------------------------------------------------------------------------
00 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
20 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
40 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
60 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
80 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
A0 | 00 00 00 00 00 00 00 00 00 00 AA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BB 00 00 00 00
C0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
E0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

If "AA" was hitted 10000 times (#$2710) and BB 1000 times (#$03E8), part 2 shows the digits #$10 and
#$E8 in positions #$AA and #$BB, while part 3 shows digits #$27 and #$03 respectively. That is:

PART2 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
   +------------------------------------------------------------------------------------------------
00 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
20 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
40 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
60 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
80 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
A0 | 00 00 00 00 00 00 00 00 00 00 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 00 00 00 00
C0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
E0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

PART3 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
   +------------------------------------------------------------------------------------------------
00 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
20 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
40 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
60 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
80 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
A0 | 00 00 00 00 00 00 00 00 00 00 27 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00
C0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
E0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

Two or three minutes are enough for this phase.


PHASE 2:

When B is pressed, it starts the next test, where the (last found) magic number is tested over all combinations
of values for A, X and imm. value of XAA and two combinations of Y (namely #$00 and #$FF) just to "add noise"
(as it isn't expected that the content of Y alters the behavior of XAA), a total of (256^3)*2 possibilities.
The following known formula is applied [1]:

  A <- (A or magic) and X and imm

and if the magic number is stable (i.e. it doesn't change from the one found in the first phase) then screen
turns green and it stops. This phase should take less than half an hour. Otherwise, screen turns red and it
stops showing the last combination where it fails:

A = Value of the accumulator before XAA
X = Value of X register before XAA
I = Value of the immediate value, e.g. XAA #$imm
R = Result obtained in the accumulator
E = Expected result, e.g. (A or magic) and X and imm

In the case of LAX, the test is similar. The magic number is tested over all combinations of values for A and
imm. value of LAX and two combinations of X and Y, a total of (256^2)*4 possibilities. Here we have:

A = Value of the accumulator before XAA
I = Value of the immediate value, e.g. XAA #$imm
R = Result obtained in the accumulator
E = Expected result, e.g. (A or magic) and imm
X = Result obtained in the X register


SOURCE CODE:

compile.bat - Batch that compiles it with CC65 (requires also cfg\nes.cfg).
main.s - Startup, NMI and some routines.
text_xaa.s - The test code for XAA.
text_lax.s - The test code for LAX.
vars.inc - Some definitions.
font.chr - A mix between C64 fonts and home-made hex characters.
genfont.cpp - Generator of file font.chr (requires "chargen", taken from Vice emulator).


COMPUTATIONAL EXPERIMENTS:

Real hardware:

1) On a japanese famicom with an Everdrive N8, magic number seems to be #$FF for XAA and LAX. However, LAX
   doesn't behave as expected in [1]. At the beginning of the 2nd. phase the test fails with:
       A = 0,  I = 1,  R = 0,  E = 1,  X = 0

2) On a Hypekin RetroN 3 (non-HDMI version) with and Everdrive N8 + 60-to-72 pin converter, magic seems to
   be #$FF for XAA and #$00 for LAX. Here, XAA doesn't behave as expected. At the beginning of the 2nd. phase
   the test fails with:
       A = 1,  X = 1,  I = 0,  R = 1,  E = 0

Emulators:

1) Mesen 0.9.8: XAA: magic = 0, fails on 2nd. phase with A = 1, X = 0, I = 0, R = 1, E = 0
                LAX: magic = FF, ok

2) Nintendulator 0.975: same results as Mesen.

3) Nestopia 1.39: XAA: magic = EE, ok
                  LAX: magic = EE, ok

4) EmuVT 1.13: same as Nestopia

5) FCEUX 2.2.3: XAA: magic = EE, ok
				LAX: magic = FF, ok 

Best,
Daniel.-

[1] No More Secrets (NMOS 6510 unintended opcodes)
    http://csdb.dk/release/?id=152327
[2] http://visual6502.org/wiki/index.php?title=6502_Opcode_8B_%28XAA,_ANE%29
