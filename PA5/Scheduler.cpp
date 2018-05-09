#include <iostream>
#include <string>
#include <deque>
#include <algorithm>

#include "Config.h"
#include "Process.h"
#include "Scheduler.h"

void Scheduler::scheduleProcesses(Config* cf, std::deque<Process> &processes) {
    // if no scheduler present, then quit
    if(cf->getSchedulingAlgorithm() == "") return;

    // schedule processes based on choice
    if(cf->getSchedulingAlgorithm() == "FIFO") {
        // do nothing since already sorted as is
    } else if(cf->getSchedulingAlgorithm() == "PS") {
        priority(processes);
    } else if(cf->getSchedulingAlgorithm() == "SJF") {
        shortestJobFirst(processes);
    } else {
        std::cerr << cf->getSchedulingAlgorithm() << " is not a valid scheduling algorithm" << std::endl;
        exit(1);
    }
}

void Scheduler::priority(std::deque<Process> &processes) {
    std::sort(processes.begin(), processes.end(), sortByPriority);
}

void Scheduler::shortestJobFirst(std::deque<Process> &processes) {
    std::sort(processes.begin(), processes.end(), sortByShortest);
}

// Will sort queue from greatest IO count to least
bool Scheduler::sortByPriority(Process p1, Process p2) {
    return p1.getIOCount() > p2.getIOCount();
}

// Will sort queue from shortest task count to greatest
bool Scheduler::sortByShortest(Process p1, Process p2) {
    return p1.getTaskCount() < p2.getTaskCount();
}