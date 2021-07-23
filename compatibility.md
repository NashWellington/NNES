# NNES game compatibility
Game-specific bugs are logged here.
Games listed here with no entries underneath them have no known compatibility issues.

## The Addams Family
* Heavy visual corruption on the "______ Presents" screen
* The bottom half of the ingame background screen is a menu screen

## Barbie
* Crashes before title screen: opcode 0x02, address 0x093E

## Battletoads
* Wrong palettes in some places at the title screen

## Castlevania
* Some sprites look weird
* Horizontal scrolling is clearly broken
* Part of the "C" in "Castlevania" on the title screen is shifted 1 pixel left

## Donkey Kong
* Pulse sound channels mute when they're not supposed to
* Triangle sound channel's pitch seems to be too low

## Double Dragon
* Fails to boot
* Tries to write to CHR-ROM, then alternates between stack over/underflow

## DuckTales
* Horizontal scrolling is broken

## Final Fantasy
* Regression: exits on assertion ((battery) == (prg_nv_ram > 0))
* Prior: crashes on opcode $22
* Note: figure out difference between battery-backed and nn

## Ice Climber
* Audio seems to work, but there's no display

## Mega Man
* Horizontal scrolling is broken.
* Weird visual artifacts after selecting stage that should be investigated

## Metroid
* Visual corruption before title screen
* All sprites are invisible
* "Out of bounds palette index" warnings 

## Pac-Man

## Super Mario Bros.
* Black boxes around clods (wrong palette index 0 color?)
* Lots of weird visual artifacts
* Horizontal scrolling is broken
* Collisions are off (sprite 0 hit weirdness?)

## Tennis

