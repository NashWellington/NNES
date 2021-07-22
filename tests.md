# NNES Test Results

Test files are located in the test_roms folder. Most tests should be accompanied by a readme.txt file detailing what they test and what their failure codes mean.

None of these tests were written by me. Credit is given in this file to each test's author. Most (if not all) of these tests can be found through [this page](https://wiki.nesdev.com/w/index.php/Emulator_tests) of the NesDev wiki.

Note: "240p test suite" is not included, and has been added to .gitignore, because its license (GPL v2) is incompatible with NNES's license (GPL v3).

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
| CPU Dummy Writes | bisqwit | 0/2 | <l><li>OAM test fails #2 (OAM reads not reliable)</li> <li>PPU test fails #5 (single write to $2006 shouldn't change PPUADDR when vblank is on</li></l> | 
| CPU Exec Space | bisqwit | 0/2 | <l><li>APU test fails #2 ($4000 error)</li> <li>PPU test fails #6</li> <li>Note: this also tests open bus behavior, which is not fully emulated yet</li></l> |
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
| 240p Test Suite | tepples | 0/? | <l>Mapper 2 is supported, but this is probably not worth testing until I make an attempt at color emphasis and a few other things</l> |
| Color Test | rainwarrior | 0/1 | Emphasis/chroma/luma not implemented yet |
| Full Palette Test | blargg | 0/3 | <l><li>full_palette and full_palette_smooth show four different shades of blue (i.e. not correct but a step in the right direction)</li> <li>flowing_palette crashes because it has PRG-RAM size set to $C4,000 (I need to check boot.cpp for this)</li></l> |
| Misc PPU Tests | blargg | 1/4 | <l><li>palette_ram passes</li> <li>power_up_palette is ignored because palette values are undefined on startup</li> <li>sprite_ram fails #4</li> <li>vbl_clear_time fails #3 (vbl flag cleared too late)</li> <li>vram_access fails #6</li></l> |
| NMI Sync Test | blargg | 0/1 | |
| NTSC Torture Test | rainwarrior | 0/1 | D-pad scrolls but select doesn't seem to do anything |
| OAM Read Test | blargg | Pass | |
| OAM Stress Test | blargg | 0/1 | |
| OAM Test 3 | lidnariq | 0/1 | Looks to be somewhat working. I'll check this test out more later. |
| Palette Test | rainwarrior | 0/1 | Same as color test |
| PPU Open Bus Test | blargg | 0/1 | Fails #4 (Note: I probably won't be fixing this for a while. It seems to test obscure VRAM decay behavior) |
| PPU Read Buffer Test | bisqwit | 0/1 | Doesn't display any results. Attempts to write three times to pattern tables despite not having CHR-RAM. Forcing CHR-RAM doesn't seem to help. |
| Scanline Test | Quietust | 0/3 | |
| Sprite DMA and DMC DMA Tests | blargg | 0/2 | Flashes some text for a few frames and displays a black screen |
| Sprite Hit Tests | blargg | 1/11 | Test 1 fails # 8. Other tests probably aren't worth trying until test 1 passes |
| Sprite Overflow Tests | blargg | 1/5 | <l><li>Test 1 passes</li> <li>Test 2 fails #9</li> <li>Test 3 fails #9</li> <li>Test 4 fails #4</li> <li>Test 5 fails #3</li></l> |
| VBL NMI Tests | blargg | 0/10 | <l><li>Test 1 fails #7 (vbl period too short w/ bg off)</li> <li>Test 4 fails #5</li> <li>Tests 9 and 10 fail #2 (clock skipped too soon relative to enabling bg)</li> <li>All other tests have complex error messages that wouldn't make sense to type out in this table</li> <li>Note: it seems vblank flags are being set 2-3 frames early (relative to the CPU's clock)</li></l> |

## APU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| APU Envelope Test | blargg | 0/1 | Plays a constant tone instead of increasing volume |
| APU Mixer | blargg | 0/4 | |
| APU Phase Reset | Rahsennor | | No CHR-RAM/ROM? (If iNES, it should default to some CHR-RAM). Worth investigating. |
| APU Reset | blargg | 3/6 | <l><li>len_ctrs_enabled passes</li> <li>irq_flag_cleared passes</li> <li>4015_cleared passes</li></l> |
| APU Sweep Test | blargg | 1/2 | Noises at the beginning and end are swapped for Cutoff |
| APU Tests | blargg | 4/11 | <l><li>Tests 1-4 pass</li> <li>Test 5 fails #2 (first length clocked too soon)</li> <li>Test 6 fails #2 (first length clocked too soon)</li> <li>Test 7 fails #2 (first flag set too soon)</li> <li>Test 8 fails #2 (IRQ triggered too soon after writing $00 to $4017)</li> <li>Test 9 fails #3 (fourth step too soon)</li> <li>Test 10 passes</li> <li>Test 11 fails #4</li></l> |
| APU Tests 2 | x0000 | 6/11 | <l><li>Note: test 2 is supposed to fail after some resets on real hardware, likely because of CPU/PPU alignments</li> <li>Note: there's no readme provided so I'll have to figure out the specifics of these tests (beyond testing the frame counter)</li> <li>Tests 1, 3, 4, 7, 8, 11 pass</li> <li>Tests 2, 6, 5, 9, 10 fail</li></l> |
| APU Timer Test | blargg | 2/4 | <l><li>Square, DMC pass</li> <li>Triangle fails</li> <li>Noise wave fails (maybe clocking too fast?)</li> <li>Triangle fails (need to adjust frequency/linear counter?)</li></l> |
| APU Triangle Linear Counter Test | blargg | Pass | |
| DMC DMA During Read | blargg | | Not worth testing until I implement DMC DMA behavior |
| DMC Tests | ??? | | DMC not fully supported |
| DPCM Letterbox | tepples | | DPCM unsupported |
| PAL APU Tests | blargg | | PAL unsupported |
| Square Timer Div 2 | blargg | 0/1 | Sound output matches div2_after.wav |
| Volume Tests | tepples | Fail | |

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
| Holy Mapperel/Holy Diver Batman | tepples | 12/26 | <l><li>Mapper 0, 2, 3, 7, 9 tests pass</li> <li>Mapper 1 P128K tests pass (7 tests)</li> <li>Mapper 1 P512K tests fail: P512K bank switching isn't working, and PRG-RAM size > 8K isn't working</li> <li>All other mappers unsupported</li></l> |
| MMC3 Big CHR-RAM Test | tepples | | Mapper 4 unsupported |
| MMC3 Test | blargg | | Mapper 4 unsupported |
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
| Allpads | tepples | 1/? | Regression: black screen (but some visual feedback on button presses) |
| Ctrl Test | rainwarrior | | ROM unavailable |
| DMA Sync Test 2 | Rahsennor | | DMC not yet emulated |
| Input data line diagnostic | lidnariq | | No pass/fail because this isn't a test. I might just remove this |
| Joypad Read Test 3 | blargg | Pass | |
| Mouse Test | rainwarrior | | ROM unavailable |
| Mic Test | rainwarrior | | ROM unavailable |
| Paddle Test 3 (Arkanoid Controller) | 3gengames | | Arkanoid controller not yet emulated |
| Telling LYs? (per-scanline input change test) | tepples | | ROM unavailable|
| Zap Ruder (zapper test) | tepples | | Zapper not yet emulated |

## Misc Tests

None yet

# Bugs

## CPU

## PPU
* Pac-Man's top left background tile is set to 0 when it shouldn't be
* It's because all sprites are zeroed out by default, so it puts sprite tile 0 at index 0,0
* PPU should output a solid color based on the value at PPU $3F00 (palette RAM index 0)
* translating palette values to pixel values may be doable with integer math, instead of pre-defined values (http://forums.nesdev.com/viewtopic.php?f=2&t=14338)

## APU
* Donkey Kong: square channels mute after a few seconds
* Note: this may be tied to the PPU

## Mapper
* Cartridges don't save SRAM to file
* Mapper 1 doesn't work correctly with 512K PRG-ROM and/or >8K PRG-RAM

## Input
* Keyboard input seems to only take 1 key press at a time

## Misc
* Open bus behavior unemulated: http://wiki.nesdev.com/w/index.php/Open_bus_behavior

# Regressions

## CPU

## PPU
* OAM read test failure

## APU

## Mapper
* Mapper 4 not compiled (intentional)

## Input
* allpads test

## Misc
* Final Fantasy fails on assertion ((battery) == (prg_nv_ram > 0))
* Prior: crashes on opcode $22
* Note: figure out difference between battery-backed and nv