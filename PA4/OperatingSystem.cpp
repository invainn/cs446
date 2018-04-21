#include <iostream>
#include <string>
#include <deque>
#include <semaphore.h>
#include <pthread.h>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "Config.h"
#include "MetaDataCode.h"
#include "Process.h"
#include "OperatingSystem.h"
#include "Log.h"

OperatingSystem::OperatingSystem(Config *cf, std::string configFilePath) {
    cf->readConfigFile(configFilePath);

    std::deque<MetaDataCode> systemOperations;
    MetaDataCode::readMetaDataFile(cf->getFilePath(), systemOperations);
    Process::createProcesses(this->processes, systemOperations);

	pthread_mutex_init(&this->projectorLock, NULL);
	pthread_mutex_init(&this->harddriveLock, NULL);
	pthread_mutex_init(&this->monitorLock, NULL);
	pthread_mutex_init(&this->scannerLock, NULL);
	pthread_mutex_init(&this->keyboardLock, NULL);

	sem_init(&this->harddriveSemaphore, 0, cf->getHarddriveResources()); 
	sem_init(&this->monitorSemaphore, 0, 1); 
	sem_init(&this->keyboardSemaphore, 0, 1); 
	sem_init(&this->scannerSemaphore, 0, 1); 
	sem_init(&this->projectorSemaphore, 0, cf->getProjectorResources()); 

    this->START_TIME = std::chrono::system_clock::now();

    this->harddriveInCount = 0;
    this->harddriveOutCount = 0;
    this->projectorCount = 0;
    this->memoryBlocksAllocated = 0;
}

void* OperatingSystem::timer(void *emp) {
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;

	double limit = 0.1;

	std::chrono::duration<double> elapsedTime;

	start = std::chrono::system_clock::now();

	while(elapsedTime.count() < limit) {
		end = std::chrono::system_clock::now();
		elapsedTime = end - start;
	}

	pthread_exit(NULL);
	return NULL;
}

double OperatingSystem::processThread(int timeLimit) {
	int time = timeLimit;
	pthread_t timerThread;

	auto timeEnd = std::chrono::system_clock::now() + std::chrono::milliseconds(timeLimit);

	while(std::chrono::system_clock::now() < timeEnd) {
		pthread_create(&timerThread, NULL, this->timer, NULL);
		pthread_join(timerThread, NULL);
	}

	auto currentTime = std::chrono::system_clock::now();

	return std::chrono::duration<double>(currentTime-this->START_TIME).count();
}

void OperatingSystem::threadOperation(int timeLimit, MetaDataCode &mdc, sem_t &semaphore, pthread_mutex_t &mutex) {
    // in this operation, we need to wait for a semaphore to be available
    sem_wait(&semaphore);
    // now lock critical section
    pthread_mutex_lock(&mutex);
    // critical section
    // this is can be flexible as a reference to a function can be called
    auto endTime = processThread(timeLimit);
    // unlock 
    pthread_mutex_unlock(&mutex);
    //output end time
    mdc.setProcessingTime(endTime);
    // signal that resource is released
    sem_post(&semaphore);
}

std::string OperatingSystem::generateMemoryLocation(int memory) {
    std::string input;

    std::stringstream sstr;
    sstr << std::hex << std::setw(8) << std::setfill('0') << memory;

    return sstr.str();
}

void OperatingSystem::processSystemApp(std::string printout, Config* cf) {
    auto currentTime = std::chrono::system_clock::now();

    auto timed = (std::chrono::duration<double>(currentTime-this->START_TIME).count());
    Log::output(*cf, std::to_string(timed) + " - " + printout);
}

void OperatingSystem::processIOOperation(MetaDataCode mdc, Config* cf, sem_t &semaphore, 
                                         pthread_mutex_t &lock, int &count, int processNumber, 
                                         int cycleTime, std::string printout) {
    int timeLimit = mdc.getCycles() + cycleTime;
    // set start time for right now
    auto currentTime = std::chrono::system_clock::now();
    mdc.setStartTime(std::chrono::duration<double>(currentTime-this->START_TIME).count());
    //now output start log

    if(count >= 0) {
        Log::output(*cf, std::to_string(mdc.getStartTime()) 
                    + " - " + "Process " + std::to_string(processNumber) + ": start " + printout + " " + std::to_string(count));
        this->threadOperation(timeLimit, mdc, semaphore, lock);
        Log::output(*cf, std::to_string(mdc.getProcessingTime()) 
                    + " - " + "Process " + std::to_string(processNumber) + ": end " + printout + " " + std::to_string(count));

        count++;
    } else {
        Log::output(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process " + std::to_string(processNumber) + ": start " + printout); 
        this->threadOperation(timeLimit, mdc, semaphore, lock);
        Log::output(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process " + std::to_string(processNumber) + ": end " + printout);
    }
}

// Processes an action (such as P or M)
void OperatingSystem::processAction(std::string printout, Config* cf, MetaDataCode mdc, int processNumber, int cycleTime) {
    auto timeLimit = mdc.getCycles() + cycleTime;
    auto currentTime = std::chrono::system_clock::now();
    mdc.setStartTime(std::chrono::duration<double>(currentTime-this->START_TIME).count());
    Log::output(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process " + std::to_string(processNumber) + ": start " + printout);
    mdc.setProcessingTime(this->processThread(timeLimit));
    Log::output(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process " + std::to_string(processNumber) + ": end " + printout);
}

// Processes each process in the processing queue 
void OperatingSystem::process(Process &p, Config* cf) {
    auto processOperations = p.getOperations();
    int noResources = -1;

    this->processSystemApp("OS: starting process " + std::to_string(p.getProcessCount()), cf);
    p.setProcessState(Process::ProcessState::READY);
    for(auto mdc : processOperations) {
        if(mdc.getCode() == 'P' && mdc.getDescriptor() == "run") {
            this->processAction("processing action", cf, mdc, p.getProcessCount(), cf->getPCT());
        } else if(mdc.getCode() == 'I') {
            if(mdc.getDescriptor() == "hard drive") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(mdc, cf, this->harddriveSemaphore, this->harddriveLock, this->harddriveInCount, p.getProcessCount(), cf->getHCT(), "hard drive input on HDD");
                p.setProcessState(Process::ProcessState::READY);

            } else if(mdc.getDescriptor() == "keyboard") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(mdc, cf, this->keyboardSemaphore, this->keyboardLock, noResources, p.getProcessCount(), cf->getKCT(), "keyboard input");
                p.setProcessState(Process::ProcessState::READY);
            } else if(mdc.getDescriptor() == "scanner") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(mdc, cf, this->scannerSemaphore, this->scannerLock, noResources, p.getProcessCount(), cf->getSCT(), "scanner input");
                p.setProcessState(Process::ProcessState::READY);
            } else {
                std::cerr << "Invalid descriptor!" << std::endl;
                exit(1);
            }
        } else if(mdc.getCode() == 'O') {
            if(mdc.getDescriptor() == "hard drive") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(mdc, cf, this->harddriveSemaphore, this->harddriveLock, this->harddriveOutCount, p.getProcessCount(), cf->getHCT(), "hard drive output on HDD");
                p.setProcessState(Process::ProcessState::READY);
            } else if(mdc.getDescriptor() == "monitor") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(mdc, cf, this->monitorSemaphore, this->monitorLock, noResources, p.getProcessCount(), cf->getMDT(), "monitor output");
                p.setProcessState(Process::ProcessState::READY);
            } else if(mdc.getDescriptor() == "projector") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(mdc, cf, this->projectorSemaphore, this->projectorLock, this->projectorCount, p.getProcessCount(), cf->getProCT(),  "projector output on PROJ");
                p.setProcessState(Process::ProcessState::READY);
            } else {
                std::cerr << "Invalid descriptor!" << std::endl;
                exit(1);
            }
        } else if(mdc.getCode() == 'M') {
            if(mdc.getDescriptor() == "block") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processAction("memory blocking", cf, mdc, p.getProcessCount(), cf->getMemCT());
                p.setProcessState(Process::ProcessState::READY);
            } else if(mdc.getDescriptor() == "allocate") {
                p.setProcessState(Process::ProcessState::RUNNING);
                auto timeLimit = mdc.getCycles() + cf->getMemCT();
                auto currentTime = std::chrono::system_clock::now();

                // if the memory blocks allocated are greater than the max memory blocks
                // then reset memory, if not, then do nothing 
                this->memoryBlocksAllocated = (this->memoryBlocksAllocated > ((float) cf->getMaxMemorySize()/(float) cf->getMemoryBlockSize())) ? 0 : this->memoryBlocksAllocated;

                auto memory = this->memoryBlocksAllocated * cf->getMemoryBlockSize();
                mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
                Log::output(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process " + std::to_string(p.getProcessCount()) + ": " + "allocating memory");
                mdc.setProcessingTime(this->processThread(timeLimit));
                Log::output(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process " + std::to_string(p.getProcessCount()) + ": " + "memory allocated at 0x" + this->generateMemoryLocation(memory));
                this->memoryBlocksAllocated++;
                p.setProcessState(Process::ProcessState::READY);
            } else {
                std::cerr << "Invalid descriptor " << mdc.getDescriptor() << std::endl;
                exit(1);
            }
        }
    }
    this->processSystemApp("OS: removing process " + std::to_string(p.getProcessCount()), cf);
    p.setProcessState(Process::ProcessState::EXIT);
}

std::deque<Process> OperatingSystem::getProcesses() {
    return this->processes;
}

void OperatingSystem::resetMemory() {
    this->memoryBlocksAllocated = 0;
}