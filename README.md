# NNES
NNES is a cycle-accurate NES emulator for Linux

## Demo Video:
[![NNES demo video](http://img.youtube.com/vi/1g9pW5pqyn4/0.jpg)](http://www.youtube.com/watch?v=1g9pW5pqyn4)

## Roadmap
This is a rough order of features I am planning to add:
* MMC5 support (used for Castlevania III and some other games)
* Cooperative threading mode for the scheduler (and other optimizations)
* Automated testing
* Other QoL features (savestates, cheats, etc.)
* Cross-platform support
* Mapper 11 support (used for incredible games by Color Dreams including Bible Adventures)
* Support for other mappers

## Compiling
NNES requires an x64 system running Linux, a C++17 compiler, and the following dependencies:
* GLEW      (Debian package: libglew-dev)
* SDL2      (Debian package: libsdl2-dev)
* FreeType2 (Debian package: libfreetype-dev)
* {fmt}     (Debian package: libfmt-dev)

To compile, navigate to the src/ directory and run "make". The executable will be compiled as "NNES" in the build/ directory.

## Testing and Compatibility
Test roms are located in the roms/tests/ directory. All test results should be added to [tests.md](tests.md). Testing is not yet automated.

Game compatibility notes can be found in [compatibility.md](compatibility.md).

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