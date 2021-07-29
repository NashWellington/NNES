# NNES game compatibility
Game-specific bugs are logged here.
Games listed here with no entries underneath them have no known compatibility issues.

Some bugs affect most or all games, and it would be redundant to add them under the games affected. They include:
* Noise channel not working correctly
* DMC channel not working correctly
* Random audio crackles/pops

## The Addams Family
* Ghost sprites are actually invisible instead of being translucent

## Barbie

## Battletoads
* Game freezes at the start of level 2

## Bionic Commando
* Mapper 1, 256K PRG-ROM, 8K CHR-RAM
* Crash on opcode 32 at PC $8012

## Castlevania

## Contra

## Donkey Kong
* Pulse sound channels mute when they're not supposed to
* Triangle sound channel's pitch seems to be too low (not sure about this one)

## Double Dragon
* Mapper 1, 128K PRG-ROM, 128K CHR-ROM
* Fails to boot
* Tries to write to about 0x400 bytes of CHR-ROM, then alternates between stack over/underflow

## Double Dribble

## Dragon Warrior

## DuckTales

## Excitebike

## Final Fantasy
* Mapper 1, 256K PRG-ROM, 8K PRG-NV-RAM, 8K CHR-RAM
* Crashes on invalid opcodes at different addresses most of the time
* This is definitely because of randomized RAM contents
* High-pitched whine when selecting an option in the UI

## Ice Climber
* Some sound channels cut out at some point
* CPU read from unmapped address $58A9 (intentional?)

## The Legend of Zelda
* Missing sprite for selecting text

## Mega Man
* High pitched noise at title screen

## Mega Man 2
* Weird behavior when scrolling up on the opening credit scene
* High pitched noise before title screen
* Crashed on invalid opcode (only on one reset. Possibly due to randomized RAM values?)

## Metroid
* Mapper 1 PRG-ROM 128K CHR-RAM 8K (should have PRG-RAM but I'm reading iNES headers wrong)
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
* Scrolling is messed up
* Music muted? (Need to compare to Mesen/console)

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
* Missing DMC sounds
* Opening theme sounds wrong after a reset