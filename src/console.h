#pragma once

#include "globals.h"

#include "scheduler.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "mem.h"
#include "peripheral.h"
#include "cart.h"

class Console
{
public:
    virtual ~Console() {}
    virtual void reset() = 0;
    virtual void insertROM(std::ifstream& rom) = 0;
    virtual void run(Scheduler::Mode mode) = 0;
    virtual void processInputs() = 0;
    std::string revision = {};
    std::string region = {};
    std::vector<std::shared_ptr<Controller>> controllers;
    //Scheduler scheduler;
};

class NES : public Console
{
public:
    NES(std::shared_ptr<Audio> audio, std::shared_ptr<Video> video);
    void reset();
    void insertROM(std::ifstream& rom);
    void run(Scheduler::Mode mode);
    // Called at the end of Input polling (not NES software input polling)
    void processInputs();
    
    std::shared_ptr<CPU> cpu;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<APU> apu;
    std::shared_ptr<Memory> mem;

    std::shared_ptr<Cartridge> cart;
    std::shared_ptr<Expansion> expansion;
    std::unique_ptr<Scheduler> scheduler;
};