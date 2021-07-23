# NNES
NNES is a NES emulator built in Modern C++

## Roadmap
This is a rough order of features I am planning to implement or change. Features I am currently working on are *italicized*:
* *Improved accuracy across the board*
* CPU rewrite (to make cycle-accurate)
* Mapper 5 support (used for Castlevania III and some other games)
* Multithreading between CPU, PPU, and APU (if feasible)
* Automated testing
* Other QoL features (savestates, cheats, more debug features, etc.)
* Cross-platform support
* Mapper 11 support (used for Bible Adventures and other incredible games)
* Support for other mappers

## Testing and Compatibility
Test roms are located in the roms/tests/ directory. All test results should be added to [tests.md](tests.md). Testing is not yet automated.

ROM compatibility hasn't been tested extensively yet. In the near future, I will add a file detailing compatibility for each ROM.

## Compiling
NNES requires a C++17 compiler, as well as the following dependencies:
* GLEW      (Debian package: libglew-dev)
* SDL2      (Debian package: libsdl2-dev)
* FreeType2 (Debian package: libfreetype-dev)

To compile, navigate to the src/ directory and run "make". The executable will be compiled as "NNES" in the build/ directory.

## License

NNES is available under the GPL V3 license.  Full text here: <http://www.gnu.org/licenses/gpl-3.0.en.html>

Copyright (C) 2021 Nash Wellington

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.