# NNES Test Results

Test files are located in the test_roms folder. Most tests should be accompanied by a readme.txt file detailing what they test and what their failure codes mean. None of these tests were written by me. Credit is given in this file to each test's author. Most (if not all) of these tests can be found through [this page](https://wiki.nesdev.com/w/index.php/Emulator_tests) of the NesDev wiki.

## CPU Tests

| Test | Author |  Tests Passed  | Details |
| :--- | :----: | :------------: | :------ |
| Branch Timing Tests | blargg | 3/3 | |
| CPU Dummy Reads | blargg | 0/1 | No support for mapper 3 yet |
| CPU Dummy Writes | bisqwit | 0/2 | OAM test failed #2; PPU test failed #5 | 
| CPU Exec Space | bisqwit | 0/2 | APU  test crashes; PPU test fails because RTS doesn't do a dummy read |
| CPU Flag Concurrency | bisqwit | 0/1 | Failed #2 |
| CPU Reset | blargg | N/A | Not tested yet as resets aren't fully implemented |
| Instruction Behavior Misc Tests | blargg | 2/4 | Test 3 failed #3; Test 4 failed #2; Combined test displays nothing |
| Instruction Test v5 | blargg | 11/11 | Passes all 11 individual tests, but exhibits weird behavior when running the combined test ROMs that needs to be investigated |
| Instruction Timing Tests | blargg | 0/2 | Test 1 failed #5; Test 2 failed |
| Interrupt Test v2 | blargg | 0/5 | Test 1 failed #3; Tests 2-5 failed |
| nestest | kevtris | 1/1 | Passes all tests when PC is set to C000, but since input doesn't currently work, it can't be run normally |