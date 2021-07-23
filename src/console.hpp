#pragma once

#include "globals.hpp"

class Processor;
class CPU;
class APU;
class PPU;
class Memory;

#include "scheduler.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "apu.hpp"
#include "mem.hpp"
#include "peripheral.hpp"
#include "cart.hpp"
#include "config.hpp"

class Console
{
public:
    virtual ~Console() {}
    virtual void reset() = 0;
    virtual void insertROM(std::ifstream& rom, std::string filename) = 0;
    virtual void run(Scheduler::Length length) = 0;
    virtual void processInputs() = 0;
    std::string revision = {};
    std::string region = {};
    std::vector<std::shared_ptr<Controller>> controllers;
    //Scheduler scheduler;
    std::unique_ptr<Config> config;
};

class NES : public Console
{
public:
    NES(Audio& audio, Video& video);
    void reset();
    void insertROM(std::ifstream& rom, std::string filename);
    void run(Scheduler::Length length);
    // Called at the end of Input polling (not NES software input polling)
    void processInputs();
    
    std::shared_ptr<CPU> cpu;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<APU> apu;
    std::unique_ptr<Memory> mem;

    std::unique_ptr<Cartridge> cart;
    std::unique_ptr<Expansion> expansion;
    std::unique_ptr<Scheduler> scheduler;

    // TODO testing
    uint frame = 0;
    uint64_t cycle = 0;
    uint64_t cycles_per_two_frames = 178683;
};