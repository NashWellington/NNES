#include "scheduler.h"

Process::Process(std::shared_ptr<Processor> _processor)
{
    processor = _processor;
    time_scale = processor->time_scale;
    // TODO thread creation?
    time = 0;
    running = false;
}

Scheduler::Scheduler(std::vector<std::shared_ptr<Processor>> processors, uint64_t cpf)
{
    assert(processors.size() > 1);
    for (uint i = 0; i < processors.size(); i++)
        processes.push_back({ processors[i] });

    //min_cycles = std::min<Process>(processes[0], processes[1]);
    min_cycles = std::min_element(processes.begin(), processes.end())->time_scale;
    cycles_per_frame = cpf;
}

void Scheduler::run(Length length)
{
    assert(length == FRAME);
    if (mode == BROKEN)
    {
        std::thread thread_cpu(ticks, processes[0].processor, cycles_per_frame / processes[0].time_scale);
        std::thread thread_apu(ticks, processes[1].processor, cycles_per_frame / processes[1].time_scale);
        std::thread thread_ppu(ticks, processes[2].processor, cycles_per_frame / processes[2].time_scale);
        thread_cpu.join();
        thread_apu.join();
        thread_ppu.join();
    }
    else if (mode == SLOW)
    {
        frame += 1;
        while (cycle < frame * cycles_per_frame)
        {
            for(auto& process : processes)
            {
                if (cycle % process.time_scale == 0) process.processor->tick();
            }
            // FIXME this doesn't work unless all procs divide evenly in each other
            cycle += min_cycles;
        }
        if (cycle == frame * cycles_per_frame)
        {
            frame = 0;
            cycle = 0;
        }
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