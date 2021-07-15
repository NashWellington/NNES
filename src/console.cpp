#include "console.h"

NES::NES(std::shared_ptr<Audio> audio, std::shared_ptr<Video> video)
{
    cpu = std::make_shared<CPU>();
    ppu = std::make_shared<PPU>(video);
    apu = std::make_shared<APU>(audio);
    mem = std::make_shared<Memory>(std::shared_ptr<NES>(this));
    cpu->mem = mem;
    ppu->mem = mem;
    apu->mem = mem;
}

void NES::reset()
{
    // TODO reset methods for all
    cpu->reset();
    //ppu->reset();
    //apu->reset();
    mem->reset();
}

void NES::insertROM(std::ifstream& rom)
{
    Header header = Boot::readHeader(rom);
    // TODO use header to determine revision & region
    cpu->setRegion(Region::NTSC);
    ppu->setRegion(Region::NTSC);
    apu->setRegion(Region::NTSC);
    // TODO use header to determine controller/expansion ports
    controllers.push_back(std::make_shared<NESStandardController>());
    controllers.push_back(std::make_shared<NESStandardController>());
    // TODO support for non-ROM carts
    cart = std::make_shared<Cartridge>(header, rom);
    cpu->start();
    // TODO have vals depend on region
    std::vector<std::shared_ptr<Processor>> processors;
    scheduler = std::make_unique<Scheduler>(std::vector<std::shared_ptr<Processor>>({cpu, apu, ppu}), 178683 * 2);
}

void NES::run(Scheduler::Mode mode)
{
    scheduler->run(mode);
}

void NES::processInputs()
{
    //cart->processInputs();
    controllers[0]->processInputs();
    controllers[1]->processInputs();
    //expansion->processInputs();
    // Input doesn't touch expansion or cartridge slots yet
}