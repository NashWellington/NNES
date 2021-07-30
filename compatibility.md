# NNES game compatibility
Game-specific bugs are logged here.
Games listed here with no entries underneath them have no known compatibility issues.

## The Addams Family

## Barbie

## Battletoads
* Game freezes at the start of level 2

## Bionic Commando

## Castlevania

## Contra

## Donkey Kong

## Double Dragon

## Double Dribble
* Slam dunk sound doesn't loop correctly (noise channel issue)

## Dragon Warrior

## DuckTales

## Excitebike

## Final Fantasy
* Constantly reads from open bus after some power cycles (probably has something to do with random contents of RAM)
* Switching between options at the opening screen messes with sound

## Ice Climber
* CPU read from unmapped address $58A9 (intentional?)
* Some broken blocks don't disappear
* Weird scrolling behavior

## The Legend of Zelda

## Mega Man

## Mega Man 2
* Weird behavior when scrolling up on the opening credit scene
* Crashed on invalid opcode (only on one reset. Possibly due to randomized RAM values?)

## Metroid

## Mike Tyson's Punch-Out!!
* Constant buzzing noise starts at some point in match 1
* Glass Joe lost his hair

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
* Weird scrolling behavior after pressing select while ingame

## Silver Surfer
* Goes back to title screen after stage selection

## Slalom

## Super Mario Bros.

## Tennis

## Time Lord
* Potentially some issue with the health bar

## Zelda II
* Mapper 1 128K PRG-ROM 128K CHR-ROM 8K PRG-NV-RAM
* Vertical black bar on the right side of the screen in the overworld
* Completely wrong bg tiles after moving 1 tile up in the overworld