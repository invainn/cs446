#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <pthread.h>
#include <chrono>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#include "Config.h"
#include "MetaDataCode.h"

struct processState {
	int state;
};

void calculateProcessingTime(Config*, MetaDataCode&, int&, int&);
void readMetaDataFile(std::string, std::vector<MetaDataCode>&);
void outputToLogFile(Config, std::vector<MetaDataCode>);
void output(Config, std::vector<MetaDataCode>, std::ostream& );
void* timer(void*);
double processThread(processState);
void runProcess(int, MetaDataCode&);
void initSem();
void directOutput(Config, std::string); 
void outputToStream(std::ostream&, std::string);
std::string generateMemoryLocation(int);

const int START = 0;
const int READY = 1;
const int RUNNING = 2;
const int WAIT = 3;
const int EXIT = 4;

sem_t projectorSemaphore;
sem_t harddriveSemaphore;
sem_t scannerSemaphore;
sem_t monitorSemaphore;
sem_t keyboardSemaphore;

pthread_mutex_t projectorLock;
pthread_mutex_t harddriveLock;

static int harddriveOutCount = 0;
static int harddriveInCount = 0;
static int projectorCount = 0;

static int memoryBlocksAllocated = 0;

processState ps;
const auto START_TIME = std::chrono::system_clock::now();

static int TIMER = 0;

static std::vector<std::string> logFile;

// Simulation starts here
// Might need to refactor into separate class depending on future projects
int main(int argc, char *argv[]) {
	// Instantiating Config and MetaData
	ps.state = EXIT;
	Config cf;
	std::string configFilePath = argv[1];
	std::vector<MetaDataCode> MetaDataVector;

	int systemStatus = 0;
	int applicationStatus = 0;

	cf.readConfigFile(configFilePath);

	readMetaDataFile(cf.getFilePath(), MetaDataVector);
	initSem();

	for(auto& mdc : MetaDataVector) {
		calculateProcessingTime(&cf, mdc, systemStatus, applicationStatus);
	}


	return 0;
}

// should probably put this in it's own class
void* timer(void *emp) {

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
// set global, doesn't need to pass in data
double processThread(int timeLimit) {

	int time = timeLimit;
	pthread_t timerThread;

	auto timeEnd = std::chrono::system_clock::now() + std::chrono::milliseconds(timeLimit);

	while(std::chrono::system_clock::now() < timeEnd) {
		pthread_create(&timerThread, NULL, timer, NULL);
		pthread_join(timerThread, NULL);
	}

	auto currentTime = std::chrono::system_clock::now();

	return std::chrono::duration<double>(currentTime-START_TIME).count();
}

std::string generateMemoryLocation(int memory) {
	std::string input;

	std::stringstream sstr;
	sstr << std::hex << std::setw(8) << std::setfill('0') << memory;

	return sstr.str();
}


// Gets code and validates descriptor and calculates process time
// Throws error if system or application order does not make sense (finishing before starting application)
void calculateProcessingTime(Config* cf, MetaDataCode& mdc, int& systemStatus, int& applicationStatus) {
	int timeLimit;

	if(mdc.getCode() == 'S') {
		if(mdc.getDescriptor() == "begin" && ps.state == EXIT) {
			ps.state = START;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Simulator program starting");
		} else if(mdc.getDescriptor() == "finish" && ps.state == READY) {
			ps.state = EXIT;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setProcessingTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Simulator program ending");
		} else {
			std::cerr << "Missing begin or finish operation for OS!\n";
			exit(1);
		}
	} else if(mdc.getCode() == 'A') {
		if(mdc.getDescriptor() == "begin" && ps.state == START) {
			ps.state = READY;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "OS: starting process 1");
		} else if(mdc.getDescriptor() == "finish" && ps.state == READY) {
			auto currentTime = std::chrono::system_clock::now();
			mdc.setProcessingTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "OS: removing process 1");
		} else {
			std::cerr << "Missing begin or finish operation for OS!\n";
		exit(1);
		}
	} else if(mdc.getCode() == 'P' && mdc.getDescriptor() == "run") {
		ps.state = RUNNING;
		timeLimit = mdc.getCycles() + cf->getPCT();
		// set start time for right now
		auto currentTime = std::chrono::system_clock::now();
		mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
		//now output start log
		directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start processing action");
		//output end time
		auto endTime = processThread(timeLimit);
		mdc.setProcessingTime(endTime);
		directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end processing action");
		ps.state = READY; 
	} else if(mdc.getCode() == 'I') {
		if(mdc.getDescriptor() == "hard drive") {
			// RUNNING OPERATIONS TEMPLATE
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getHCT();
			int currentCount;
			if(harddriveInCount == cf->getHarddriveResources()) {
				harddriveInCount = 0;
			}
			currentCount = harddriveInCount;

			// kdset start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start hard drive input on HDD " + std::to_string(currentCount));
			// actually start now

			// in this operation, we need to wait for a semaphore to be available
			sem_wait(&harddriveSemaphore);

			// after waiting, we have access to the resources. For now, we need 1 resource
			pthread_mutex_lock(&harddriveLock);
			// critical section
			harddriveInCount++;
			// unlock 
			pthread_mutex_unlock(&harddriveLock);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);

			sem_post(&harddriveSemaphore);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end hard drive input on HDD " + std::to_string(currentCount));

			ps.state = READY; 


		} else if(mdc.getDescriptor() == "keyboard") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getKCT();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start keyboard output");
			// actually start now
			// in this operation, we need to wait for a semaphore to be available
			sem_wait(&keyboardSemaphore);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			sem_post(&keyboardSemaphore);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end keyboard output");
			ps.state = READY; 
		} else if(mdc.getDescriptor() == "scanner") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getSCT();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start scanner output");
			// actually start now
			// in this operation, we need to wait for a semaphore to be available
			sem_wait(&scannerSemaphore);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			sem_post(&scannerSemaphore);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end scanner output");
			ps.state = READY; 
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'O') {
		if(mdc.getDescriptor() == "hard drive") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getHCT();
			int currentCount;
			if(harddriveOutCount == cf->getHarddriveResources()) {
				harddriveOutCount = 0;
			}
			currentCount = harddriveOutCount;
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start hard drive output on HDD " + std::to_string(currentCount));
			// actually start now

			// in this operation, we need to wait for a semaphore to be available
			sem_wait(&harddriveSemaphore);

			// after waiting, we have access to the resources. For now, we need 1 resource
			pthread_mutex_lock(&harddriveLock);
			// critical section
			harddriveOutCount++;
			// unlock 
			pthread_mutex_unlock(&harddriveLock);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);

			sem_post(&harddriveSemaphore);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end hard drive output on HDD " + std::to_string(currentCount));

			ps.state = READY; 
		} else if(mdc.getDescriptor() == "monitor") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getMDT();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start monitor output");
			// actually start now
			// in this operation, we need to wait for a semaphore to be available
			sem_wait(&monitorSemaphore);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			sem_post(&monitorSemaphore);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end monitor output");
			ps.state = READY; 
		} else if(mdc.getDescriptor() == "projector") {
			ps.state = RUNNING;
			int currentCount;
			if(projectorCount == cf->getProjectorResources()) {
				projectorCount = 0;
			}
			currentCount = projectorCount; 
			timeLimit = mdc.getCycles() + cf->getProCT();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start projector output on PROJ " + std::to_string(currentCount));
			// actually start now
			// in this operation, we need to wait for a semaphore to be available
			sem_wait(&projectorSemaphore);

			pthread_mutex_lock(&projectorLock);
			//critical section
			projectorCount++;
			pthread_mutex_unlock(&projectorLock);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			sem_post(&projectorSemaphore);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end projector output on PROJ " + std::to_string(currentCount));
			ps.state = READY; 
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'M') {
		if(mdc.getDescriptor() == "block") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getMemCT();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start memory blocking");
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end memory blocking");
			ps.state = READY; 
		} else if(mdc.getDescriptor() == "allocate") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getMemCT();
			int memory = memoryBlocksAllocated * cf->getMemory();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: allocating memory");
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: memory allocated at 0x" + generateMemoryLocation(memory));
			memoryBlocksAllocated++;
			ps.state = READY; 

		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	}
}

// Takes an ostream obj (cout or ofstream) and outputs the text to the selected stream 
void outputToStream(std::ostream& out, std::string logOutput) {
	out << logOutput << std::endl;
}

// This function directs the output of the log to the selected streams
void directOutput(Config cf, std::string logOutput) {
	std::ofstream logFile;

	// these flags are set depending on where the config specifies where to log to
	bool monitorFlag = false;
	bool logFileFlag = false;

	if(cf.getLogTo() == "Both") {
		monitorFlag = true;
		logFileFlag = true;
	} else if(cf.getLogTo() == "monitor") {
		monitorFlag = true;
	} else if(cf.getLogTo() == "file") {
		logFileFlag = true;
	} else {
		std::cerr << "Cannot log to " << cf.getLogTo() << "!\n";
		exit(1);
	}

	if(monitorFlag) {
		outputToStream(std::cout, logOutput);
	}

	if(logFileFlag) {
		logFile.open(cf.getLogFilePath());
		outputToStream(logFile, logOutput);
		logFile.close();
	}
}

// Gets the filepath from the config and then reads the MetaData file into the MetaDataVector
// by constructing an object of each MetaDataCode and inserts it into the vector
void readMetaDataFile(std::string filePath, std::vector<MetaDataCode>& MetaDataVector) {
	std::ifstream metaDataFile;
	metaDataFile.open(filePath);

	std::vector<std::string> tokens;
	std::string s;
	std::string temp;

	char codeInput;
	std::string descriptorInput;
	int cyclesInput;
	bool readOverFlag = false;

	while(metaDataFile >> s) {
		if(s.find("hard") != std::string::npos) {
			metaDataFile >> temp;
			s.append(" ");
			s.append(temp);
		}

		tokens.push_back(s);
	}

	metaDataFile.close();

	// iterates over vector
	for(auto it = tokens.begin(); it != tokens.end(); it++) {
		if(*it == "Start") {
			std::advance(it, 4);
		} else if(*it == "End") {
			break;
		} else if(readOverFlag) {
			// Gets thrown if another process is read after the period '.'
			std::cerr << "Cannot find end!" << std::endl;
			exit(1);
		}

		s = *it;

		codeInput = s[0];
		s.erase(0, 2);

		descriptorInput = s.substr(0, s.find('}'));
		s.erase(0, s.find('}')+1);

		// Gets the cycle number
		temp = s.substr(0, s.find(';'));

		// this finds out if the process ends in a period
		// which indicates it is the last process to be read.
		if(temp.find('.') != std::string::npos) {
			readOverFlag = true;
			s.erase(s.find('.'), 1);
		}

		cyclesInput = std::stoi(temp);

		// need to delete semicolon
		s = *it;
		s.erase(std::remove(s.begin(), s.end(), ';'), s.end());

		MetaDataCode mdcTemp(codeInput, descriptorInput, cyclesInput, s);
		MetaDataVector.push_back(mdcTemp);
	}
}

void initSem() {
	pthread_mutex_init(&projectorLock, NULL);
	pthread_mutex_init(&harddriveLock, NULL);

	sem_init(&harddriveSemaphore, 0, 1); 
	sem_init(&monitorSemaphore, 0, 1); 
	sem_init(&keyboardSemaphore, 0, 1); 
	sem_init(&scannerSemaphore, 0, 1); 
	sem_init(&projectorSemaphore, 0, 1); 
}
