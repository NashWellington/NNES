# NNES Test Results

Test files are located in the test_roms folder. Most tests should be accompanied by a readme.txt file detailing what they test and what their failure codes mean.

None of these tests were written by me. Credit is given in this file to each test's author. Most (if not all) of these tests can be found through [this page](https://wiki.nesdev.com/w/index.php/Emulator_tests) of the NesDev wiki.

Note: "240p test suite" is not included, and has been added to .gitignore, because its license (GPL v2) is incompatible with NNES's license (GPL v3).

Note: some tests (notably mapper tests by rainwarrior) are missing because of corruption of the NesDev forums database.

Note: blargg's APU tests (not v2) are in 2 separate folders until I can identify if there's a redundancy

## CPU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| Branch Timing Tests | blargg | Pass | |
| CPU Dummy Reads | blargg | 0/1 | LDA abs,X fails #3 |
| CPU Dummy Writes | bisqwit | 0/2 | <l><li>OAM test fails #2 (OAM reads not reliable)</li> <li>PPU test fails #5 (single write to $2006 shouldn't change PPUADDR when vblank is on</li></l> | 
| CPU Exec Space | bisqwit | 0/2 | <l><li>APU test crashes (APU not implemented)</li> <li>PPU test fails #6</li></l> |
| CPU Flag Concurrency | bisqwit | 0/1 | Fails #2 |
| CPU Reset | blargg | N/A | Not tested yet as resets aren't fully implemented |
| CPU Timing Test v6 | Zepper | Pass | |
| Instruction Behavior Misc Tests | blargg | 2/4 | <l><li>Tests 1 and 2 pass</li> <li>Test 3 fails #3 (Note: this tests mid-instruction reads, and will likely not pass until I make the CPU cycle-accurate)</li> <li>Test 4 fails #2</li> <li>Combined test displays nothing</li></l> |
| Instruction Test v5 | blargg | Pass | |
| Instruction Timing Tests | blargg | 0/2 | <l><li>Test 1 fails #5</li> <li>Test 2 fails with no error code</li> <li>Note: these tests require a functional APU</li></l> |
| Interrupt Test v2 | blargg | 0/5 | <l><li>Test 1 fails #4 (exactly one execution after CLI should execute before IRQ is taken)</li> <li>Tests 2-5 fail with no error code</li> <li>Note: tests 2-4 are likely failing beccause interrupt hijacking isn't emulated</li></l> |
| nestest | kevtris | Pass | |

## PPU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| 240p Test Suite | tepples | 0/? | <l>Mapper 2 is supported, but this is probably not worth testing until I make an attempt at color emphasis and a few other things</l> |
| Color Test | rainwarrior | 0/1 | Emphasis/chroma/luma not implemented yet |
| Full Palette Test | blargg | 0/3 | <l><li>full_palette and full_palette_smooth show four different shades of blue (i.e. not correct but a step in the right direction)</li> <li>flowing_palette crashes because it has PRG-RAM size set to $C4,000 (I need to check boot.cpp for this)</li></l> |
| Misc PPU Tests | blargg | 1/5 | <l><li>palette_ram passes</li> <li>power_up_palette fails #2 (start/reset palette setting not implemented yet)</li> <li>sprite_ram fails #4</li> <li>vbl_clear_time fails #3 (vbl flag cleared too late)</li> <li>vram_access fails #6</li></l> |
| NMI Sync Test | blargg | 0/1 | |
| NTSC Torture Test | rainwarrior | 0/1 | No color display |
| OAM Read Test | blargg | Pass | |
| OAM Stress Test | blargg | 0/1 | |
| OAM Test 3 | lidnariq | 0/1 | Looks to be somewhat working. I'll check this test out more later. |
| Palette Test | rainwarrior | 0/1 | Same as color test |
| PPU Open Bus Test | blargg | 0/1 | Fails #4 (Note: I probably won't be fixing this for a while. It seems to test obscure VRAM decay behavior) |
| PPU Read Buffer Test | bisqwit | 0/1 | Doesn't display any results. Attempts to write three times to pattern tables despite not having CHR-RAM. Forcing CHR-RAM doesn't seem to help. |
| Scanline Test | Quietust | 0/1 | Only shows a gray screen |
| Sprite DMA and DMC DMA Tests | blargg | 0/2 | Flashes some text for a few frames and displays a black screen |
| Sprite Hit Tests | blargg | 1/11 | <l><li>Tests 1-4 fail #2</li> <li>Test 5 fails #4</li> <li>Tests 6-10 fail #3</li> <li>Test 11 passes somehow</li></l> |
| Sprite Overflow Tests | blargg | 1/5 | <l><li>Test 1 fails #6 (shouldn't be set when all rendering off)</li> <li>Test 2 fails #6 (shouldn't be set when sprite y coords are 240)</li> <li>Test 3 fails #5</li> <li>Test 4 passes</li> <li>Test 5 fails #3</li></l> |
| VBL NMI Tests | blargg | 0/10 | <l><li>Test 1 fails #7 (vbl period too short w/ bg off)</li> <li>Test 4 fails #5</li> <li>Tests 9 and 10 fail #2 (clock skipped too soon relative to enabling bg)</li> <li>All other tests have complex error messages that wouldn't make sense to type out in this table</li> <li>Note: it seems vblank flags are being set 2-3 frames early (relative to the CPU's clock)</li></l> |

## APU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| APU Envelope Test | blargg | 0/1 | |
| APU Mixer | blargg | 0/4 | <l><li>Square wave test fails (probably something to do with the mixer)</li> <li>Other sound channels not supported</li></l> |
| APU Phase Reset | Rahsennor | | Resets not fully supported |
| APU Reset | blargg | | Resets not fully supported |
| APU Sweep Test | blargg | | APU sweep not supported |
| APU Tests | blargg | 0/12 | <l><li>Test 1 fails #4</li> <li>Test 2 fails and displays "$F8 $FF $1E $02"</li> <li>Test 3 fails #5</li> <li>Test 4 fails #2</li> <li>Test 5 fails #3</li> <li>Test 6 fails #3</li> <li>Test 7 fails #2</li> <li>Test 8 shows a gray screen</li> <li>Test 9 fails #4</li> <li>Test 10 fails #3</li> <li>Test 11 fails #2</li> <li>Combined ROM test 1 fails #4</li></l> |
| APU Tests 2 | x0000 | 4/11 | <l><li>Note: this is supposed to fail after some resets on real hardware, likely because of CPU/PPU alignments</li> <li>Note: there's no readme provided so I'll have to figure out the specifics of these tests (beyond testing the frame counter)</li> <li>Tests 1, 2, 5, 6 pass</li> <li>Tests 3, 4, 7, 8, 9, 10, 11 fail</li></l> |
| APU Timer Test | blargg | | <l><li>Square wave sounds like it probably should (minus some periods of audio delay) but I can't tell the difference between square_pitch_wave.wav and square_pitch_wave_bad.wav</li></l> |
| APU Triangle Linear Counter Test | blargg | | Triangle channel unsupported |
| DMC DMA During Read | blargg | 2/5 | <l><li>DMA $2007 write/read-write pass</li> <li>DMA/Double $2007 read fail with unintelligible error codes<li>DMA $4016 read fails</li> <li> |
| DMC Tests | ??? | 0/4 | All tests show gray screens. This test probably depends on other sound channels |
| DPCM Letterbox | tepples | | DPCM unsupported |
| PAL APU Tests | blargg | | PAL unsupported |
| Square Timer Div 2 | blargg | 0/1 | Sound output matches div2_after.wav |
| Volume Tests | tepples | | I will test this after an attempt at fixing volume |

## Mapper Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| BNTest | tepples | 0/3 | <l><li>AxROM test segfaults</li> <li>Other tests fail because of no support for mapper 34</li><l> |
| BxROM 512k Test | rainwarrior | | ROM unavailable |
| exram Test | Quietust | 0/1 | Mapper 5 unsupported |
| FDS IRQ Tests v7 | Sour | 0/1 | FDS format unsupported |
| Famicom Audio Swap Tests | rainwarrior | | ROM unavailable |
| FME-7 Ack Test | tepples | 0/1 | Mapper 69 unsupported |
| FME-7 RAM Test | tepples | 0/1 | Mapper 69 unsupported |
| Holy Mapperel/Holy Diver Batman | tepples | 3/26 | <l><li>Mapper 0 test passes</li> <li>Mapper 1 P128K test fails #4300</li> <li>Mapper 1 P128K tests with CHR memory fail because the headers have CHR-ROM and CHR-RAM sizes set</li> <li>Mapper 1 P512K tests fail #0300</li> <li>Mapper 2 test passes</li> <li>Mapper 3 test passes</li> <li>Mapper 7 test segfaults</li> <li>Mapper 9 has both CHR-ROM and -RAM set</li> <li>All other mappers unsupported</li></l> |
| MMC3 Big CHR-RAM Test | tepples | | Mapper 4 unsupported |
| MMC3 Test | blargg | | Mapper 4 unsupported |
| MMC5 RAM Size Tests | rainwarrior | | ROM unavailable |
| MMC5 Test | Drag | | ROM unavailable |
| MMC5 Test v2 | AWJ | | .prg/.chr filetypes unsupported |
| Mapper 28 Tests | tepples | | Mapper 28 unsupported |
| Mapper 31 Tests | rainwarrior | Mapper 31 unsupported | ROMs unavailable |
| NES 2.0 Submapper Tests | rainwarrior | | ROMs unavailable |
| SEROM Test | lidnariq | 0/1 | Fails because CHR-ROM and CHR-RAM are both set in the header |
| VRC 2/4 Tests | AWJ | | Mappers 21, 22, 23, 25 unsupported |
| VRC6 Tests | natt | | Mappers 24, 26 unsupported |

## Input Tests

None yet

## Misc Tests

None yet

# Bugs

## CPU

## PPU
* Pac-Man's top left background tile is set to 0 when it shouldn't be

## APU
* Volume too quiet/too loud/not correctly implemented. Try using unsigned 16-bit ints

## Mapper

## Input

## Misc

# Regressions

## CPU

## PPU

## APU

## Mapper
* Mapper 4 not compiled (intentional)
* Mapper 7 segfaults

## Input
* Donkey Kong starts with "2 Player Game B" selected, when it should have "1 Player Game A" selected
* Donkey Kong does not go to preview screen

## Misc
* Program segfaults at exit