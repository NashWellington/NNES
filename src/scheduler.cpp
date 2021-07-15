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

void Scheduler::run(Mode mode)
{
    assert(mode == FRAME_SLOW);
    frame += 1;
    while (cycle < frame * cycles_per_frame)
    {
        for(auto& process : processes)
        {
            if (cycle % process.time_scale == 0) process.processor->tick();
        }
        cycle += min_cycles;
    }
    if (cycle == frame * cycles_per_frame)
    {
        frame = 0;
        cycle = 0;
    }
}

void Scheduler::sync()
{

}