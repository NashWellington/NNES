# NNES game compatibility
Game-specific bugs are logged here.
Games listed here with no entries underneath them have no known compatibility issues.

These games were picked for testing because they are popular, because they are good at quickly and easily testing specific features (Double Dribble is good for testing DMC and noise channel behavior), or because they are [tricky to emulate](https://wiki.nesdev.com/w/index.php/Tricky-to-emulate_games) and can be used to identify emulator bugs that would not normally be identifiable in other games or test ROMs.

## The Addams Family

## Barbie

## Battletoads
* Game freezes at the start of level 2

## Balloon Fight

## Bases Loaded II

## Bionic Commando

## Burai Fighter
* Mapper 4, 32K PRG-ROM, 32K CHR-ROM
* Fails to boot, plays random noise

## Castlevania

## Contra

## Crystalis
* Mapper 4, 256K PRG-ROM, 128K CHR-ROM
* crashes on $42 at PC $C50B
* Battery is set to true in the header, so maybe this is supposed to have PRG-NV-RAM?

## Donkey Kong
* Mario's jumps sound wrong (some have ascending pitch, some descending)

## Double Dragon

## Double Dribble
* Slam dunk sound doesn't loop correctly (noise channel issue)

## Dragon Warrior

## DuckTales

## Excitebike

## Final Fantasy
* Switching between options at the opening screen messes with sound

## Ice Climber
* CPU read from unmapped address $58A9 (intentional?)
* Some broken blocks don't disappear
* Weird scrolling behavior

## The Legend of Zelda

## Mega Man

## Mega Man 2
* Weird behavior when scrolling up on the opening credit scene

## Metroid

## Mike Tyson's Punch-Out!!
* Constant buzzing noise starts at some point in match 1
* Glass Joe lost his hair

## Ms. Pac-Man
* Fails to boot (only in the unlicensed version - mapper 0, 32K PRG-ROM, 8K CHR-ROM)
* Relies on obscure NMI delay behavior with PPUSTATUS

## Pac-Man

## Paperboy
* Mapper 3, 32K PRG-ROM, 32K CHR-ROM
* The background is ridiculously screwed up (might be an issue with vertical scrolling?)

## Pirates
* Some text boxes are glitched after docking at a port

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

## Super Mario Bros. 2
* Mapper 4, 128K PRG-ROM, 128K CHR-ROM
* Freeze on purple screen after starting game
* Infinite stack under/overflow loop

## Super Mario Bros. 3
* Mapper 4, 256K PRG-ROM, 128K CHR-ROM
* Completely fails to boot

## Teenage Mutant Ninja Turtles
* Incorrect overworld scrolling behavior
* Missing/invisible text on death

## Tennis

## Time Lord
* Potentially some issue with the health bar (DMC IRQ timing issue?)

## Wizards and Warriors 3
* Mapper 7, 256K PRG-ROM, 8K CHR-RAM
* Constant whining noise at title screen
* Black bars over doors when crouching
* "Life" meter is cut off at the top

## The Young Indiana Jones Chronicles
* Mapper 4, 128K PRG-ROM, 128K CHR-ROM
* stack overflow x 50, then crash on opcode $32 at PC $2013

## Zelda II
* Mapper 1 128K PRG-ROM 128K CHR-ROM 8K PRG-NV-RAM
* Vertical black bar on the right side of the screen in the overworld
* Completely wrong bg tiles after moving 1 tile up in the overworld