#include <iostream>
#include <string>
#include <deque>

#include "Config.h"
#include "Process.h"
#include "Scheduler.h"

void Scheduler::scheduleProcesses(Config* cf, std::deque<Process> &processes) {
    // schedule processes based on choice

    if(cf->getSchedulingAlgorithm() == "FIFO") {
        this->firstInFirstOut(cf, processes);
    } else if(cf->getSchedulingAlgorithm() == "PS") {
        this->priority(cf, processes);
    } else if(cf->getSchedulingAlgorithm() != "SJF") {
        // this is to check if it's FIFO (do nothing) and if not, then throw error
        std::cerr << cf->getSchedulingAlgorithm() << " is not a valid scheduling algorithm" << std::endl;
        exit(1);
    }
}

void Scheduler::priority(Config* cf, std::deque<Process> &processes) {

}

void Scheduler::shortestJobFirst(Config* cf, std::deque<Process> &processes) {

}