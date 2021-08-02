#include "scheduler.hpp"

// TODO clean this up so it's efficient AND extensible AND not a mess

Process::Process(std::shared_ptr<Processor> _processor) : processor(_processor)
{
    time_scale = processor->time_scale;
    // TODO thread creation?
    time = 0;
    running = false;
}

// Scheduler::Scheduler(std::vector<std::shared_ptr<Processor>> processors, uint64_t cpf)
// {
//     assert(processors.size() > 1);
//     for (auto& processor : processors)
//     {
//         min_cycles = std::min(min_cycles, processor->time_scale);
//         processes.push_back(std::make_unique<Process>(processor));
//     }
//     cycles_per_frame = cpf;
// }

void Scheduler::run(Length length)
{
    assert(length == FRAME);
    // if (mode == BROKEN)
    // {
    //     std::thread thread_cpu(ticks, processes[0]->processor, cycles_per_frame / processes[0]->time_scale);
    //     std::thread thread_apu(ticks, processes[1]->processor, cycles_per_frame / processes[1]->time_scale);
    //     std::thread thread_ppu(ticks, processes[2]->processor, cycles_per_frame / processes[2]->time_scale);
    //     thread_cpu.join();
    //     thread_apu.join();
    //     thread_ppu.join();
    // }
    if (mode == SLOW)
    {
        // while (cycle < frame * cycles_per_frame)
        // {
        //     for(auto& process : processes)
        //     {
        //         if (cycle % process->time_scale == 0) process->processor->tick();
        //     }
        //     // FIXME this doesn't work unless all procs divide evenly in each other
        //     cycle += min_cycles;
        // }
        while (cycle < cycles_per_frame)
        {
            cpu->tick();
            apu->tick();
            ppu->tick();
            ppu->tick();
            ppu->tick();
            cpu->tick();
            ppu->tick();
            ppu->tick();
            ppu->tick();
            cycle += 6 * min_cycles;
        }
        cycle %= cycles_per_frame;
    }
}

void ticks(std::shared_ptr<Processor> p, uint64_t cycles)
{
    for (uint i = 0; i < cycles; i++)
        p->tick();
}

void Scheduler::sync()
{

}