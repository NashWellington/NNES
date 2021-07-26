# NNES game compatibility
Game-specific bugs are logged here.
Games listed here with no entries underneath them have no known compatibility issues.

## The Addams Family
* Noise channel sfx sounds wrong
* Invisible sprites?

## Barbie
* Incorrect background color (green instead of black) for some tiles on the title screen

## Battletoads
* All noise channel sfx sound wrong
* Game freezes at the start of level 2

## Bionic Commando
* Mapper 1, 256K PRG-ROM, 8K CHR-RAM
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
* Mapper 1, 128K PRG-ROM, 128K CHR-ROM
* Fails to boot
* Tries to write to about 0x400 bytes of CHR-ROM, then alternates between stack over/underflow

## Double Dribble
* "Double Dribble" sfx is muted (DMC is probably not emulated properly)

## Dragon Warrior

## DuckTales
* Horizontal scrolling is broken

## Excitebike
* Horizontal scrolling weirdness

## Final Fantasy
* Mapper 1, 256K PRG-ROM, 8K CHR-RAM
* Crashes on opcode $12

## Ice Climber
* Vertical scrolling is broken
* Some sound channels cut out at some point
* CPU read from unmapped address $58A9 (intentional?)

## The Legend of Zelda
* Missing/corrupted sprites for selecting save file, text, etc.
* No sound when Link gets hit (probably DMC)

## Mega Man
* Horizontal scrolling is broken.
* Weird visual artifacts after selecting stage that should be investigated
* High pitched noise at title screen

## Mega Man 2
* Slightly glitchy vertical scrolling at the title screen
* High pitched noise before title screen
* Crashed on invalid opcode (only on one reset. Possibly due to randomized RAM values?)

## Metroid
* Mapper 1 PRG-ROM 128K CHR-RAM 8K (should have PRG-RAM but I'm reading iNES headers wrong)
* All sprites are invisible

## Mike Tyson's Punch-Out!!
* Sprite weirdness
* Noise channel SFX weirdness

## Ms. Pac-Man
* Fails to boot (only in the unlicensed version - mapper 0, 32K PRG-ROM, 8K CHR-ROM)

## Pac-Man

## Paperboy
* Mapper 3, 32K PRG-ROM, 32K CHR-ROM
* Controller input doesn't seem to work
* Note: according to https://wiki.nesdev.com/w/index.php/Open_bus_behavior "Paperboy relies on the controller port being exactly $41 for a button press to be recognized"
* Most of the background isn't rendered
* General scrolling weirdness that we know and love

## R.C. Pro-AM
* Artifacts at the bottom split screen

## Super Mario Bros.
* Mapper 0, 32K PRG-ROM, 8K CHR-ROM
* Black boxes around clouds (wrong palette index 0 color?)
* Lots of weird visual artifacts
* Horizontal scrolling is broken
* Wrong tiles loaded on first screen (after starting game)

## Tennis

## Time Lord
* Missing/broken sfx