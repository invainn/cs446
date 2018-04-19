#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include "MetaDataCode.h"
#include "Config.h"

#ifndef PROCESS_H
#define PROCESS_H
class Process {
    public:
        Process();

        enum class ProcessState { START, READY, RUNNING, WAIT, EXIT };

        void readOperations(Config); // Read in operations from Config

		static void createProcesses(std::deque<Process>&, std::deque<MetaDataCode>);
        void push(MetaDataCode);
        void incrementIOCount();
        void incrementTaskCount();

        void setProcessCount(int);
        int getProcessCount();

        ProcessState getProcessState();
        void setProcessState(ProcessState);

        std::deque<MetaDataCode> getOperations();

        MetaDataCode popOperation();

    private:
        ProcessState processState;
        int numberOfIO; // Counts the number of I/O operations for use in Priority Scheduling
        int numberOfTasks; // Number of tasks used in SJF
        int processCount;
        std::deque<MetaDataCode> operations;
};
#endif