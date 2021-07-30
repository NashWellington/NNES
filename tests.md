# NNES Test Results

Test files are located in the roms/tests/ folder. Most tests should be accompanied by a readme.txt file detailing what they test and what their failure codes mean.

None of these tests were written by me. Credit is given in this file to each test's author to the best of my ability. Most (if not all) of these tests can be found through [this page](https://wiki.nesdev.com/w/index.php/Emulator_tests) of the NesDev wiki.

Note: "240p test suite" is not included, and has been added to the .gitignore, because its license (GPL v2) is incompatible with NNES's license (GPL v3).

Note: some tests (notably mapper/input tests by rainwarrior) are missing because of corruption of the NesDev forums database.

## CPU Tests

At this point, CPU tests that fail depend on:
* cycle accurate CPU (including dummy reads/writes, interrupt hijacking)
* open bus behavior
* APU/PPU registers working correctly

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| Branch Timing Tests | blargg | Pass | |
| CPU Dummy Reads | blargg | 0/1 | LDA abs,X fails #3 |
| CPU Dummy Writes | bisqwit | 0/2 | <l><li>OAM test fails #2 (OAM reads not reliable)</li> <li>PPU test fails #9</li></l> | 
| CPU Exec Space | bisqwit | 1/2 | <l><li>APU test fails #2 ($4000 error)</li> <li>PPU test passes</li></l> |
| CPU Flag Concurrency | bisqwit | Pass? | Flags and APU IRQ timing work as intended |
| CPU Reset | blargg | Pass | |
| CPU Timing Test v6 | Zepper | Pass | |
| Instruction Behavior Misc Tests | blargg | 2/4 | <l><li>Tests 1 and 2 pass</li> <li>Test 3 fails #3 (LDA abs,X) (Note: this tests mid-instruction reads, and will likely not pass until I make the CPU cycle-accurate)</li> <li>Test 4: a long list of opcodes fail #2. Note: this probably won't work until I get all APU regs emulated</li></l> |
| Instruction Test v5 | blargg | Pass | |
| Instruction Timing Tests | blargg | Pass | |
| Interrupt Test v2 | blargg | 0/5 | <l><li>Test 1 fails #4</li> <li>Tests 2-5 fail with no error code</li> <li>Note: tests 2-4 are likely failing beccause interrupt hijacking isn't emulated</li></l> |
| nestest | kevtris | Pass | |
| RAM Retention Test | rainwarrior | | ROM unavailable; this might be the same as the one included in blargg's CPU Reset test folder |
| XAA Test | Aureus | | |

## PPU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| 240p Test Suite | tepples | 0/? | Mapper 2 is supported, but this is probably not worth testing until I make an attempt at color emphasis and a few other things |
| Color Test | rainwarrior | 0/1 | Emphasis/chroma/luma not implemented yet |
| Full Palette Test | blargg | 0/3 | <l><li>full_palette and full_palette_smooth show four different shades of blue (i.e. not correct but a step in the right direction)</li> <li>flowing_palette crashes because it has PRG-RAM size set to $C4,000 (I need to check boot.cpp for this)</li></l> |
| Misc PPU Tests | blargg | 1/4 | <l><li>palette_ram passes</li> <li>power_up_palette is ignored because palette values are undefined on startup</li> <li>sprite_ram fails #4</li> <li>vbl_clear_time fails #3 (vbl flag cleared too late)</li> <li>vram_access fails #6</li></l> |
| NMI Sync Test | blargg | ? | Not sure what a passing result is supposed to look like |
| NTSC Torture Test | rainwarrior | ? | D-pad scrolls but select doesn't seem to do anything |
| OAM Read Test | blargg | 0/1 | |
| OAM Stress Test | blargg | 0/1 | |
| OAM Test 3 | lidnariq | 0/1 | Looks to be somewhat working. I'll check this test out more later. |
| Palette Test | rainwarrior | 0/1 | Same as color test |
| PPU Open Bus Test | blargg | 0/1 | Fails #3 (Note: I probably won't be fixing this for a while. It seems to test obscure VRAM decay behavior) |
| PPU Read Buffer Test | bisqwit | 0/1 | Doesn't display any results. Attempts to write three times to pattern tables despite not having CHR-RAM. Forcing CHR-RAM doesn't seem to help. |
| Scanline Test | Quietust | Pass | |
| Sprite DMA and DMC DMA Tests | blargg | 0/2 | Flashes some text for a few frames and displays a black screen |
| Sprite Hit Tests | blargg | Pass | |
| Sprite Overflow Tests | blargg | 2/5 | <l><li>Test 1 passes</li> <li>Test 2 fails #9</li> <li>Test 3 fails #12</li> <li>Test 4 fails #5</li> <li>Test 5 passes</li></l> |
| VBL NMI Tests | blargg | 0/10 | <l><li>Test 1 fails #7 (vbl period too short w/ bg off)</li> <li>Test 4 fails #5</li> <li>Tests 9 and 10 fail #2 (clock skipped too soon relative to enabling bg)</li> <li>All other tests have complex error messages that wouldn't make sense to type out in this table</li> <li>Note: it seems vblank flags are being set 2-3 frames early (relative to the CPU's clock)</li></l> |

## APU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| APU Envelope Test | blargg | 0/1 | Plays a constant tone instead of increasing volume |
| APU Mixer | blargg | 0/4 | |
| APU Phase Reset | Rahsennor | | No CHR-RAM/ROM? (If iNES, it should default to some CHR-RAM). Worth investigating. |
| APU Reset | blargg | 3/6 | <l><li>len_ctrs_enabled passes</li> <li>irq_flag_cleared passes</li> <li>4015_cleared passes</li></l> |
| APU Sweep Test | blargg | 1/2 | Noises at the beginning and end are swapped for Cutoff |
| APU Tests | blargg | 5/11 | <l><li>Tests 1-4 pass</li> <li>Test 5 fails #2 (first length clocked too soon)</li> <li>Test 6 fails #2 (first length clocked too soon)</li> <li>Test 7 fails #2 (first flag set too soon)</li> <li>Test 8 fails #2 (IRQ triggered too soon after writing $00 to $4017)</li> <li>Test 9 passes</li> <li>Test 10 passes</li> <li>Test 11 fails #4</li></l> |
| APU Tests 2 | x0000 | 5/11 | <l><li>Note: test 2 is supposed to fail after some resets on real hardware, likely because of CPU/PPU alignments</li> <li>Note: there's no readme provided so I'll have to figure out the specifics of these tests (beyond testing the frame counter)</li> <li>Tests 1, 3, 4, 7, 8 pass</li> <li>Tests 2, 5, 6, 9, 10, 11 fail</li></l> |
| APU Timer Test | blargg | 2/4 | <l><li>Square, triangle pass</li> <li>Noise wave fails & sounds like noise_pitch_bad</li> <li>DMC is almost there but sounds buzzy</li></l> |
| APU Triangle Linear Counter Test | blargg | Fail | Pitch slightly too low |
| DMC DMA During Read | blargg | | Not worth testing until I implement DMC DMA behavior |
| DMC Tests | ??? | ?/4 | Not sure what a valid test result is supposed to look like, but all four tests sound the same when played |
| DPCM Letterbox | tepples | Pass? | I'm almost positive it looks correct, but I have nothing to compare this to |
| PAL APU Tests | blargg | | PAL unsupported |
| Square Timer Div 2 | blargg | 0/1 | Sound output matches div2_after.wav + a constant high pitched whine |
| Volume Tests | tepples | Fail | Regression: Doesn't play any sound |

## Mapper Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| BNTest | tepples | 1/3 | <l><li>AxROM passes</li> <li>Other tests fail because of no support for mapper 34</li><l> |
| BxROM 512k Test | rainwarrior | | ROM unavailable |
| exram Test | Quietust | 0/1 | Mapper 5 unsupported |
| FDS IRQ Tests v7 | Sour | 0/1 | FDS format unsupported |
| Famicom Audio Swap Tests | rainwarrior | | ROM unavailable |
| FME-7 Ack Test | tepples | 0/1 | Mapper 69 unsupported |
| FME-7 RAM Test | tepples | 0/1 | Mapper 69 unsupported |
| Holy Mapperel/Holy Diver Batman | tepples | 14/26 | <l><li>Mapper 0, 1, 2, 3, 7, 9 tests pass</li> <li>Mapper 4 tests fail IRQ portion</li> <li>All other mappers unsupported</li></l> |
| MMC3 Big CHR-RAM Test | tepples | Pass? | Passes test, but there is visible VRAM corruption |
| MMC3 Test | blargg | 0/6 | All tests fail + VRAM corruption |
| MMC5 RAM Size Tests | rainwarrior | | ROM unavailable |
| MMC5 Test | Drag | | ROM unavailable |
| MMC5 Test v2 | AWJ | | .prg/.chr filetypes unsupported |
| Mapper 28 Tests | tepples | | Mapper 28 unsupported |
| Mapper 31 Tests | rainwarrior | | ROMs unavailable |
| NES 2.0 Submapper Tests | rainwarrior | | ROMs unavailable |
| SEROM Test | lidnariq | 0/1 | Fails, likely because mapper 1 submapper 5 unsupported |
| VRC 2/4 Tests | AWJ | | Mappers 21, 22, 23, 25 unsupported |
| VRC6 Tests | natt | | Mappers 24, 26 unsupported |

## Input Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| Allpads | tepples | ? | Controller 2 not yet tested |
| Ctrl Test | rainwarrior | | ROM unavailable |
| DMA Sync Test 2 | Rahsennor | | DMC not yet emulated correctly |
| Input data line diagnostic | lidnariq | | No pass/fail because this isn't a test. I might just remove this |
| Joypad Read Test 3 | blargg | Pass | |
| Mouse Test | rainwarrior | | ROM unavailable |
| Mic Test | rainwarrior | | ROM unavailable |
| Paddle Test 3 (Arkanoid Controller) | 3gengames | | Arkanoid controller not yet emulated |
| Telling LYs? (per-scanline input change test) | tepples | | ROM unavailable |
| Zap Ruder (zapper test) | tepples | | Zapper not yet emulated |

## Misc Tests

None yet

# Bugs

## CPU

## PPU
* translating palette values to pixel values may be doable with integer math, instead of pre-defined values (http://forums.nesdev.com/viewtopic.php?f=2&t=14338)
* Non-palette PPU memory reads should have one-byte buffer

## APU
* MAJOR: DMC channel not properly emulated
* MAJOR: Crackles and pops in audio (note: this may be because I'm not letting SDL resample?)

## Mapper
* Mapper 1 doesn't work correctly with 512K PRG-ROM and/or >8K PRG-RAM
* Mapper 4's IRQs don't work properly
* Some mapper 4 tests have corrupted nametable or CHR data

## Input
* Keyboard input seems to only take 1 key press at a time

## Misc
* Open bus behavior unemulated: http://wiki.nesdev.com/w/index.php/Open_bus_behavior

# Regressions

## CPU

## PPU

## APU

## Mapper

## Input

## Misc