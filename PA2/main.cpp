#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <pthread.h>
#include <chrono>
#include <stdlib.h>
#include <time.h>

#include "Config.h"
#include "MetaDataCode.h"

struct processState {
	int state;
};

void calculateProcessingTime(Config, MetaDataCode&, int&, int&);
void readMetaDataFile(std::string, std::vector<MetaDataCode>&);
void outputToLogFile(Config, std::vector<MetaDataCode>);
void output(Config, std::vector<MetaDataCode>, std::ostream&, int);
void* timer(void*);
double processThread(processState);
void runProcess(int, MetaDataCode&);
std::string generateMemoryLocation();

const int START = 0;
const int READY = 1;
const int RUNNING = 2;
const int WAIT = 3;
const int EXIT = 4;

processState ps;

const auto START_TIME = std::chrono::system_clock::now();

static int TIMER = 0;

static std::vector<std::string> log;

// Simulation starts here
// Might need to refactor into separate class depending on future projects
int main(int argc, char *argv[]) {
	// Instantiating Config and MetaData
	ps.state = EXIT;
	std::string configFilePath = argv[1];	
	Config* cf = new Config();
	std::vector<MetaDataCode> MetaDataVector;

	int systemStatus = 0;
	int applicationStatus = 0;

	cf->readConfigFile(configFilePath);

	readMetaDataFile(cf->getFilePath(), MetaDataVector);	

	for(auto& mdc : MetaDataVector) {
		calculateProcessingTime(*cf, mdc, systemStatus, applicationStatus);
	}

	outputToLogFile(*cf, MetaDataVector);
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

std::string generateMemoryLocation() {
	std::string input;
	srand(time(NULL));

	input = std::to_string(rand() % 10000000 + 10000000);

	return input;
}


// Gets code and validates descriptor and calculates process time
// Throws error if system or application order does not make sense (finishing before starting application)
void calculateProcessingTime(Config cf, MetaDataCode& mdc, int& systemStatus, int& applicationStatus) {
	int timeLimit;

	if(mdc.getCode() == 'S') {
		if(mdc.getDescriptor() == "begin" && ps.state == EXIT) {
			ps.state = START;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Simulator program starting");
		} else if(mdc.getDescriptor() == "finish" && ps.state == READY) {
			ps.state = EXIT;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setProcessingTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Simulator program ending");
		} else {
			std::cerr << "Missing begin or finish operation for OS!\n";
			exit(1);
		}
	} else if(mdc.getCode() == 'A') {
		if(mdc.getDescriptor() == "begin" && ps.state == START) {
			ps.state = READY;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "OS: starting process 1");
		} else if(mdc.getDescriptor() == "finish" && ps.state == READY) {
			auto currentTime = std::chrono::system_clock::now();
			mdc.setProcessingTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "OS: removing process 1");
		} else {
			std::cerr << "Missing begin or finish operation for OS!\n";
		exit(1);
		}
	} else if(mdc.getCode() == 'P' && mdc.getDescriptor() == "run") {
		ps.state = RUNNING;	
		timeLimit = mdc.getCycles() * cf.getPCT();
		runProcess(timeLimit, mdc);
		log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start processing action");
		log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end processing action");
		ps.state = READY;	
	} else if(mdc.getCode() == 'I') {
		if(mdc.getDescriptor() == "hard drive") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getHCT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start hard drive input");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end hard drive input");
		ps.state = READY;	
		} else if(mdc.getDescriptor() == "keyboard") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getKCT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start keyboard input");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end keyboard input");
		ps.state = READY;	
		} else if(mdc.getDescriptor() == "scanner") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getSCT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start scanner input");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end scanner input");
		ps.state = READY;	
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'O') {
		if(mdc.getDescriptor() == "hard drive") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getHCT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start hard drive output");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end hard drive output");
		ps.state = READY;	
		} else if(mdc.getDescriptor() == "monitor") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getMDT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start monitor output");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end monitor output");
		ps.state = READY;	
		} else if(mdc.getDescriptor() == "projector") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getProCT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start projector output");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end projector output");
		ps.state = READY;	
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'M') {
		if(mdc.getDescriptor() == "block") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getMemCT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: start memory blocking");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end memory blocking");
		ps.state = READY;	
		} else if(mdc.getDescriptor() == "allocate") {
		ps.state = RUNNING;	
			timeLimit = mdc.getCycles() * cf.getMemCT();
			runProcess(timeLimit, mdc);
			log.push_back(std::to_string(mdc.getStartTime()) + " - " + "Process 1: allocating memory");
			log.push_back(std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: memory allocated at 0x" + generateMemoryLocation());
		ps.state = READY;	
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	}
}

void runProcess(int milliseconds, MetaDataCode &mdc) {
	auto currentTime = std::chrono::system_clock::now();
	mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
	auto endTime = processThread(milliseconds);
	mdc.setProcessingTime(endTime);
}

// Takes the MetaDataCode vector and config file and outputs to log file
void outputToLogFile(Config cf, std::vector<MetaDataCode> mdv) {
	std::ofstream logFile;

	int loggedToOption = 0;
	// these flags are set depending on where the config specifies where to log to
	bool monitorFlag = false;
	bool logFileFlag = false;

	if(cf.getLogTo() == "Both") {
		monitorFlag = true;
		logFileFlag = true;
		loggedToOption = 0;
	} else if(cf.getLogTo() == "monitor") {
		monitorFlag = true;
		loggedToOption = 1;
	} else if(cf.getLogTo() == "file") {
		logFileFlag = true;
		loggedToOption = 2;
	} else {
		std::cerr << "Cannot log to " << cf.getLogTo() << "!\n";
		exit(1);
	}

	if(monitorFlag) {
		output(cf, mdv, std::cout, loggedToOption);	
	} 
	
	if(logFileFlag) {
		logFile.open(cf.getLogFilePath());
		output(cf, mdv, logFile, loggedToOption);
		logFile.close();
	}
}

// Takes an ostream obj (cout or ofstream) and outputs the text to the log file
void output(Config cf, std::vector<MetaDataCode> mdv, std::ostream& out, int loggedToOption) {
	/*out << "Configuration File Data" << std::endl;
	out << "Monitor = " << cf.getMDT() << " ms/cycle" << std::endl;
	out << "Processor = " << cf.getPCT() << " ms/cycle" << std::endl;
	out << "Scanner = " << cf.getSCT() << " ms/cycle" << std::endl;
	out << "Hard Drive = " << cf.getHCT() << " ms/cycle" << std::endl;
	out << "Keyboard = " << cf.getKCT() << " ms/cycle" << std::endl;
	out << "Memory = " << cf.getMemCT() << " ms/cycle" << std::endl;
	out << "Projector = " << cf.getProCT() << " ms/cycle" << std::endl;
	
	// loggedToOption is chosen in outputToLogFile
	// it provides where to log
	if(loggedToOption == 0) {
		out << "Logged to: monitor and " << cf.getLogFilePath() << std::endl;
	} else if(loggedToOption == 1) {
		out << "Logged to: " << cf.getLogTo() << std::endl;
	} else if(loggedToOption == 2) {
		out << "Logged to: " << cf.getLogFilePath() << std::endl;
	}*/
	
	out << std::endl;
	out << "Meta-Data Metrics" << std::endl;

	for(auto it = log.begin(); it != log.end(); it++) {
		out << std::fixed << std::setprecision(6) << *it << std::endl;
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
