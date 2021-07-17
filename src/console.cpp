#include "console.h"

NES::NES(Audio& audio, Video& video)
{
    cpu = std::make_shared<CPU>(*this);
    apu = std::make_shared<APU>(*this, audio);
    ppu = std::make_shared<PPU>(*this, video);
    mem = std::make_unique<Memory>(*this);
}

void NES::reset()
{
    // TODO reset methods for all
    cpu->reset();
    ppu->reset();
    apu->reset();
}

void NES::insertROM(std::ifstream& rom)
{
    Header header = Boot::readHeader(rom);
    // TODO use header to determine revision & region
    cpu->setRegion(Region::NTSC);
    ppu->setRegion(Region::NTSC);
    apu->setRegion(Region::NTSC);
    // TODO use header to determine controller/expansion ports
    controllers.resize(2);
    controllers[0] = std::make_shared<NESStandardController>();
    controllers[1] = std::make_shared<NESStandardController>();
    // TODO support for non-ROM carts
    cart = std::make_unique<Cartridge>(header, rom);
    cpu->start();
    mem->start();
    // TODO have master cycle/frame vals depend on region
    std::vector<std::shared_ptr<Processor>> processors = {cpu, apu, ppu};
    scheduler = std::make_unique<Scheduler>(processors, 178683 * 2);
}

void NES::run(Scheduler::Length length)
{
    scheduler->run(length);
}

void NES::processInputs()
{
    //cart.processInputs();
    controllers[0]->processInputs();
    controllers[1]->processInputs();
    //expansion.processInputs();
    // Input doesn't touch expansion or cartridge slots yet
}