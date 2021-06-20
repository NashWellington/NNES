# NNES Test Results

Test files are located in the test_roms folder. Most tests should be accompanied by a readme.txt file detailing what they test and what their failure codes mean.

The "Details" column generally gives the reason for failure, or the error code returned by the test

None of these tests were written by me. Credit is given in this file to each test's author. Most (if not all) of these tests can be found through [this page](https://wiki.nesdev.com/w/index.php/Emulator_tests) of the NesDev wiki.

Note: "240p test suite" is not included, and has been added to .gitignore, because its license (GPL v2) is incompatible with NNES's license (GPL v3).

## CPU Tests

| Test | Author | Tests Passed | Details |
| :--- | :----: | :----------: | :------ |
| Branch Timing Tests | blargg | 3/3 | |
| CPU Dummy Reads | blargg | 0/1 | No support for mapper 3 yet |
| CPU Dummy Writes | bisqwit | 0/2 | OAM test fails #2; PPU test fails #5 | 
| CPU Exec Space | bisqwit | 0/2 | APU  test crashes; PPU test fails #6 |
| CPU Flag Concurrency | bisqwit | 0/1 | Fails #2 |
| CPU Reset | blargg | N/A | Not tested yet as resets aren't fully implemented |
| Instruction Behavior Misc Tests | blargg | 2/4 | Test 3 fails #3; Test 4 fails #2; Combined test displays nothing |
| Instruction Test v5 | blargg | 11/11 | Passes all 11 individual tests, but exhibits weird behavior when running the combined test ROMs that needs to be investigated |
| Instruction Timing Tests | blargg | 0/2 | Test 1 fails #5; Test 2 fails |
| Interrupt Test v2 | blargg | 0/5 | Test 1 fails #3; Tests 2-5 fails |
| nestest | kevtris | 1/1 | |

## PPU Tests

| Test | Author | Tests Passed | Details |
| :--- | :----: | :----------: | :------ |
| 240p Test Suite | tepples | 0/2 | No support for mappers 2 or 34 yet |
| Color Test | rainwarrior | 0/1 | Only shows a gray screen |
| Full Palette Test | blargg | 0/3 | full_palette and full_palette_smooth only show black screens; flowing_palette crashes because its PRG-RAM size is larger than the maximum PRG-RAM size for mapper 0 |
| Misc PPU Tests | blargg | 1/5 | power_up_palette fails #2; sprite_ram fails #6; vbl_clear_time fails #3; vram_access fails #6 |
| NMI Sync Test | blargg | 0/1 | Only shows a black screen |
| NTSC Torture Test | rainwarrior | 0?/1 | Inputs don't seem to change anything on screen |
| OAM Read Test | blargg | 0/1 | |
| OAM Stress Test | blargg | 0/1 | |
| OAM Test 3 | lidnariq | 0/1 | No support for mapper 7 yet |
| Palette Test | rainwarrior | ?/1 | Inputs don't seem to change anything on screen |
| PPU Open Bus Test | blargg | 0/1 | Fails #4 |
| PPU Read Buffer Test | bisqwit | 0/1 | No support for mapper 3 yet |
| Scanline Test | Quietust | 0/1 | |
| Sprite DMA and DMC DMA Tests | blargg | 0/2 | |
| Sprite Hit Tests | blargg | 0/11 | Sprites not yet implemented |
| Sprite Overflow Tests | blargg | 0/5 | Sprites not yet implemented |
| VBL NMI Tests | blargg | 0/10 | Test 1 fails #7; Test 4 fails #5; Test 9 fails #2; Test 10 fails #2 |