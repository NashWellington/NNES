# NNES
NNES is a NES emulator and debugger built in C++.
NNES is **very** early in development, and is currently not expected to work on anything that isn't a select few test roms.
NNES has currently only been tested on Ubuntu 20.04 with gcc 9.3.0 on an x64 system.

## Roadmap
This is a rough order of things I am planning to implement or change:
* Sprites
* Audio
* Automated testing
* Change the CPU to being cycle-accurate
* More mappers
* Other QoL features (savestates, cheats, etc)
* Cross-platform support

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