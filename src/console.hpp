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
class Cartridge;
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
    // Scheduler scheduler;
    std::shared_ptr<Config> config;

    void log(std::vector<std::string_view> disassembled);
    
    bool paused = false;
    uint frame = 0;
    uint64_t cycle = 0;
};

class NES : public Console
{
public:
    NES(Audio& audio, Video& video, std::shared_ptr<Config> _config);
    void reset();
    void insertROM(std::ifstream& rom, std::string filename);
    void run(Scheduler::Length length);
    // Called at the end of Input polling (not NES software input polling)
    void processInputs();

    void log();
    struct
    {
        uword pc = 0;
        std::queue<ubyte> byte_queue = {};
    } log_buffer = {};
    
    std::shared_ptr<CPU> cpu;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<APU> apu;
    std::unique_ptr<Memory> mem;

    std::unique_ptr<Cartridge> cart;
    std::unique_ptr<Expansion> expansion;
    std::unique_ptr<Scheduler> scheduler;

    uint64_t cycles_per_two_frames = 178683;
};