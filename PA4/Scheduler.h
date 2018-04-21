#include <iostream>
#include <string>
#include <deque>

#include "Config.h"
#include "Process.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H
class Scheduler {
    public:
        void scheduleProcesses(Config*, std::deque<Process>&);

    private:
       void priority(Config*, std::deque<Process>&);
       void shortestJobFirst(Config*, std::deque<Process>&);
};
#endif