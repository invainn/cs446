#include <vector>
#include <string>
#include <deque>

#include "Process.h"
#include "MetaDataCode.h"

Process::Process() {
    this->processState = ProcessState::START;
    this->numberOfIO = 0;
    this->numberOfTasks = 0;
}

// Factory where Processes are created and then inserted into the referenced deque
void Process::createProcesses(std::deque<Process>& processes, std::deque<MetaDataCode> systemOperations) {
    int numberOfProcesses = 1;
    // Loop over operations and count number of processes
    for(auto it = systemOperations.begin(); it != systemOperations.end(); ++it) {
        // Check if mdc is a Application Start
        if(it->getCode() == 'A' && it->getDescriptor() == "begin") {
            // Create new Process object to store new operations
            Process p;
            p.setProcessCount(numberOfProcesses);

            // Loop over vector to store operations into p
            // if it hits Application End, exit lsdf
            auto tempIterator = it;

            if(tempIterator->getCode() != 'S' && tempIterator->getDescriptor() != "finish")  {
                tempIterator = it+1;
            }
            while(tempIterator->getDescriptor() != "finish") {
                if(tempIterator->getCode() == 'O' || tempIterator->getCode() == 'I') {
                    p.incrementIOCount(); 
                }

                p.incrementTaskCount();

                p.push(*tempIterator);
                std::advance(tempIterator, 1);
            }

            processes.push_back(p);
            numberOfProcesses++;
        }
    }
}

void Process::push(MetaDataCode mdc) {
    this->operations.push_back(mdc);
}

void Process::incrementIOCount() {
    this->numberOfIO++;
}

void Process::incrementTaskCount() {
    this->numberOfTasks++;
}

void Process::setProcessCount(int count) {
    this->processCount = count;
}

int Process::getProcessCount() {
    return this->processCount;
}

MetaDataCode Process::popOperation() {
    auto mdc = this->operations.front();
    this->operations.pop_front();
    return mdc;
}

std::deque<MetaDataCode> Process::getOperations() {
    return this->operations;
}

Process::ProcessState Process::getProcessState() {
    return this->processState;
}

void Process::setProcessState(Process::ProcessState stateChange) {
    this->processState = stateChange; 
}

int Process::getIOCount() {
    return this->numberOfIO;
}

int Process::getTaskCount() {
    return this->numberOfTasks;
}