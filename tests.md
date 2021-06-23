# NNES Test Results

Test files are located in the test_roms folder. Most tests should be accompanied by a readme.txt file detailing what they test and what their failure codes mean.

None of these tests were written by me. Credit is given in this file to each test's author. Most (if not all) of these tests can be found through [this page](https://wiki.nesdev.com/w/index.php/Emulator_tests) of the NesDev wiki.

Note: "240p test suite" is not included, and has been added to .gitignore, because its license (GPL v2) is incompatible with NNES's license (GPL v3).

## CPU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| Branch Timing Tests | blargg | Pass | |
| CPU Dummy Reads | blargg | 0/1 | No support for mapper 3 yet |
| CPU Dummy Writes | bisqwit | 0/2 | <l><li>OAM test fails #2</li> <li>PPU test fails #5</li></l> | 
| CPU Exec Space | bisqwit | 0/2 | <l><li>APU  test crashes</li> <li>PPU test fails #6</li></l> |
| CPU Flag Concurrency | bisqwit | 0/1 | Fails #2 |
| CPU Reset | blargg | N/A | Not tested yet as resets aren't fully implemented |
| Instruction Behavior Misc Tests | blargg | 2/4 | <l><li>Tests 1 and 2 pass</li> <li>Test 3 fails #3</li> <li>Test 4 fails #2</li> <li>Combined test displays nothing</li></l> |
| Instruction Test v5 | blargg | 11/11 | Passes all 11 individual tests, but exhibits weird behavior when running the combined test ROMs that needs to be investigated |
| Instruction Timing Tests | blargg | 0/2 | <l><li>Test 1 fails #5</li> <li>Test 2 fails with no error code</li></l> |
| Interrupt Test v2 | blargg | 0/5 | <l><li>Test 1 fails #3</li> <li>Tests 2-5 fail with no error code</li></l> |
| nestest | kevtris | Pass | |

## PPU Tests

| Test | Author | Status | Details |
| :--- | :----: | :----: | :------ |
| 240p Test Suite | tepples | 0/2 | No support for mappers 2 or 34 yet |
| Color Test | rainwarrior | 0/1 | Only shows a gray screen |
| Full Palette Test | blargg | 0/3 | <l><li>full_palette and full_palette_smooth show four different shades of blue (i.e. not correct but a step in the right direction)</li> <li>flowing_palette crashes because its PRG-RAM size is larger than the maximum PRG-RAM size for mapper 0</li></l> |
| Misc PPU Tests | blargg | 1/5 | <l><li>power_up_palette fails #2</li> <li>sprite_ram fails #6</li> <li>vbl_clear_time fails #3</li> <li>vram_access fails #6</li></l> |
| NMI Sync Test | blargg | 0/1 | Shows a black screen with an attempt at sprites |
| NTSC Torture Test | rainwarrior | 0/1 | Inputs don't seem to change anything on screen (except maybe the sprites) |
| OAM Read Test | blargg | Pass | |
| OAM Stress Test | blargg | 0/1 | |
| OAM Test 3 | lidnariq | 0/1 | No support for mapper 7 yet |
| Palette Test | rainwarrior | ?/1 | Inputs don't seem to change anything on screen (except maybe the sprites) |
| PPU Open Bus Test | blargg | 0/1 | Fails #4 |
| PPU Read Buffer Test | bisqwit | 0/1 | No support for mapper 3 yet |
| Scanline Test | Quietust | 0/1 | Only shows a gray screen |
| Sprite DMA and DMC DMA Tests | blargg | 0/2 | Flashes some text for a few frames and displays a black screen |
| Sprite Hit Tests | blargg | 1/11 | <l><li>Tests 1-4 fail #2</li> <li>Test 5 fails #4</li> <li>Tests 6-10 fail #3</li> <li>Test 11 passes somehow</li></l> |
| Sprite Overflow Tests | blargg | 0/5 | <l><li>Tests 1, 2, 4, and 5 fail #2</li> <li>Test 3 fails #3</li></l> |
| VBL NMI Tests | blargg | 0/10 | <l><li>Test 1 fails #7</li> <li>Test 4 fails #5</li> <li>Tests 9 and 10 fail #2</li> <li>All other tests have complex error messages that wouldn't make sense to type out in this table</li></l> |