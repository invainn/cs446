#include <iostream>
#include <string>
#include <deque>

#include "Config.h"
#include "Process.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H
class Scheduler {
    public:
        static void scheduleProcesses(Config*, std::deque<Process>&);

    private:
       static void priority(std::deque<Process>&);
       static void shortestJobFirst(std::deque<Process>&);

       static bool sortByPriority(Process, Process);
       static bool sortByShortest(Process, Process);

};
#endif