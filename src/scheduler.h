#pragma once

#include "globals.h"
#include "processor.h"
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
    Scheduler(std::vector<std::shared_ptr<Processor>> processors, uint64_t cpf);
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
        SLOW,   // No multithreading, ticks each processor one at a time
        BROKEN  // Puts all processors in three differnt threads and runs them
    } mode = BROKEN;
    void run(Length length);
    void sync();
private:
    // The main process that other processes will sync to is at index 0
    std::vector<Process> processes;
    uint min_cycles = 0;
    uint64_t cycle = 0;
    uint frame = 0;
    uint64_t cycles_per_frame = 0;
};