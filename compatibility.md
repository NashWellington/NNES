# NNES game compatibility
Game-specific bugs are logged here.
Games listed here with no entries underneath them have no known compatibility issues.

Some bugs affect most or all games, and it would be redundant to add them under the games affected. They include:
* Noise channel not working correctly
* DMC channel not working correctly
* Random audio crackles/pops

## The Addams Family
* Ghost sprites are actually invisible instead of being translucent (spooky!)

## Barbie

## Battletoads
* Game freezes at the start of level 2

## Bionic Commando

## Castlevania

## Contra

## Donkey Kong
* Pulse sound channels mute when they're not supposed to
* Triangle sound channel's pitch seems to be too low (not sure about this one)

## Double Dragon

## Double Dribble
* Slam dunk sound doesn't loop correctly (noise channel issue)

## Dragon Warrior

## DuckTales

## Excitebike

## Final Fantasy
* Constantly reads from open bus after some power cycles (probably has something to do with random contents of RAM)
* High-pitched whine when selecting an option in the UI

## Ice Climber
* Some sound channels cut out at some point
* CPU read from unmapped address $58A9 (intentional?)
* Invisible sprites
* Broken blocks don't disappear

## The Legend of Zelda
* Missing sprite for selecting text

## Mega Man

## Mega Man 2
* Weird behavior when scrolling up on the opening credit scene
* Crashed on invalid opcode (only on one reset. Possibly due to randomized RAM values?)

## Metroid
* Mapper 1 PRG-ROM 128K CHR-RAM 8K
* All sprites are invisible

## Mike Tyson's Punch-Out!!
* Invisible sprites in some places
* Note: this might have something to do with MMC2 PPU fetch sniffing?
* Pulse (or triangle?) channels cut out after reset
* Glass Joe is bald at some point

## Ms. Pac-Man
* Fails to boot (only in the unlicensed version - mapper 0, 32K PRG-ROM, 8K CHR-ROM)

## Pac-Man

## Paperboy
* Mapper 3, 32K PRG-ROM, 32K CHR-ROM
* Controller input doesn't seem to work
* Note: according to https://wiki.nesdev.com/w/index.php/Open_bus_behavior "Paperboy relies on the controller port being exactly $41 for a button press to be recognized"
* The background is ridiculously screwed up

## R.C. Pro-AM

## River City Ransom
* Mapper 4 128K PRG-ROM, 128K CHR-ROM
* Scrolling is messed up (might have to do with mapper 4 IRQ behavior)
* Sound completely muted while ingame (but not at title screen)

## Silver Surfer
* Goes back to title screen after stage selection
* Audio is probably not correct

## Slalom

## Super Mario Bros.
* Mapper 0, 32K PRG-ROM, 8K CHR-ROM
* No animation when going into/out of a pipe (Mario just appears/disappears)
* Mushroom/star/power up sprites are invisible for a second after appearing

## Tennis

## Time Lord
* Sound effect made when getting the fifth orb is missing

## Zelda II
* Mapper 1 128K PRG-ROM 128K CHR-ROM 8K PRG-NV-RAM
* Vertical black bar on the right side of the screen in the overworld
* Completely wrong bg tiles after moving 1 tile up in the overworld