#pragma once

#include "globals.hpp"
#include "processor.hpp"
#include <thread>
#include <algorithm>

/* Container class for a processor, its thread, and some member vars
*/
struct Process
{
    Process(std::shared_ptr<Processor> _processor);
    std::shared_ptr<Processor> processor;
    std::thread thread;
    uint64_t time_scale;
    uint64_t time;
    bool running;
    bool operator<(const Process& other_proc)
    {
        return (this->time_scale < other_proc.time_scale);
    }
};

void ticks(std::shared_ptr<Processor> p, uint64_t cycles);

class Scheduler
{
public:
    //Scheduler(std::vector<std::shared_ptr<Processor>> processors, uint64_t cpf);
    Scheduler(std::vector<std::shared_ptr<Processor>> procs, uint64_t cpf)
        : cpu(procs[0]), apu(procs[1]), ppu(procs[2]), cycles_per_frame(cpf)
        { min_cycles = procs[2]->time_scale; }
    enum Length
    {
        PPU_CYCLE,
        CPU_CYCLE,
        CPU_INSTR,
        FRAME_SLOW,
        FRAME
    };
    enum Mode
    {
        // No multithreading, ticks each processor one at a time
        SLOW,
        // Puts all processors in three differnt threads and runs them
        // Currently only displays a black screen
        BROKEN
    } mode = SLOW;
    void run(Length length);
    void sync();
private:
    // The main process that other processes will sync to is at index 0
    // I won't really be using this until I implement cooperative threading
    //std::vector<std::unique_ptr<Process>> processes;
    std::shared_ptr<Processor> cpu;
    std::shared_ptr<Processor> apu;
    std::shared_ptr<Processor> ppu;
    uint64_t min_cycles = UINT64_MAX;
    uint64_t cycle = 0;
    uint64_t cycles_per_frame = 0;
};