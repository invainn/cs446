#include <iostream>
#include <string>
#include <deque>
#include <semaphore.h>
#include <pthread.h>
#include <chrono>

#include "Config.h"
#include "MetaDataCode.h"
#include "Process.h"
#include "Log.h"

#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H
class OperatingSystem {
    public:
        OperatingSystem(Config*, std::string);

        void process(Process&, Config*);
        void processSystemApp(std::string, Config*);

        std::deque<Process> getProcesses();
    private:
        static void* timer(void*);
        double processThread(int);
        void runProcess(int, MetaDataCode&);
        void threadOperation(int, MetaDataCode&, sem_t&, pthread_mutex_t&);
        void processIOOperation(MetaDataCode, Config*, sem_t&, pthread_mutex_t&, int&, int, int, std::string);
        void processAction(std::string, Config*, MetaDataCode, int, int);

        std::string generateMemoryLocation(int);

        std::chrono::time_point<std::chrono::system_clock> START_TIME;

        sem_t projectorSemaphore;
        sem_t harddriveSemaphore;
        sem_t scannerSemaphore;
        sem_t monitorSemaphore;
        sem_t keyboardSemaphore;

        pthread_mutex_t projectorLock;
        pthread_mutex_t harddriveLock;
        pthread_mutex_t scannerLock;
        pthread_mutex_t monitorLock;
        pthread_mutex_t keyboardLock;

        int harddriveOutCount;
        int harddriveInCount;
        int projectorCount;
        int memoryBlocksAllocated;

        std::deque<Process> processes;
};
#endif