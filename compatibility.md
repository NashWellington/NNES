# NNES game compatibility
Game-specific bugs are logged here.
Games listed here with no entries underneath them have no known compatibility issues.

## The Addams Family
* Noise channel sfx sounds wrong
* Invisible sprites?

## Barbie
* Incorrect background color (green instead of black) for some tiles on the title screen

## Battletoads
* 

## Bionic Commando
* Crash on opcode 32 at PC $8012

## Castlevania
* Sprites are being loaded from incorrect pattern tiles
* Horizontal scrolling is clearly broken
* Whip sfx incorrect

## Contra
* Glitchy horizontal scrolling

## Donkey Kong
* Pulse sound channels mute when they're not supposed to
* Triangle sound channel's pitch seems to be too low

## Double Dragon
* Fails to boot
* Tries to write to CHR-ROM, then alternates between stack over/underflow

## Double Dribble
* "Double Dribble" sfx is muted (DMC is probably not emulated properly)

## Dragon Warrior
* Exits on assertion (battery) == (prg_nv_ram > 0)

## DuckTales
* Horizontal scrolling is broken

## Excitebike
* Horizontal scrolling weirdness

## Final Fantasy
* Regression: exits on assertion ((battery) == (prg_nv_ram > 0))
* Prior: crashes on opcode $22
* Note: figure out difference between battery-backed and nv

## Ice Climber
* Vertical scrolling is broken
* Some sound channels cut out at some point
* CPU read from unmapped address $58A9 (intentional?)

## The Legend of Zelda
* Fails on battery/nv-ram assertion

## Mega Man
* Horizontal scrolling is broken.
* Weird visual artifacts after selecting stage that should be investigated
* High pitched noise at title screen

## Mega Man 2
* Slightly glitchy vertical scrolling at the title screen
* High pitched noise before title screen
* Crashed on invalid opcode (only on one reset. Possibly due to randomized RAM values?)

## Metroid
* All sprites are invisible

## Mike Tyson's Punch-Out!!
* Sprite weirdness
* Noise channel SFX weirdness

## Ms. Pac-Man
* Fails to boot (only in the unlicensed version)

## Pac-Man

## Paperboy
* Controller input doesn't seem to work
* Most of the background isn't rendered
* General scrolling weirdness that we know and love

## R.C. Pro-AM
* Artifacts at the bottom split screen

## Super Mario Bros.
* Black boxes around clouds (wrong palette index 0 color?)
* Lots of weird visual artifacts
* Horizontal scrolling is broken

## Tennis

## Time Lord
* Missing/broken sfx