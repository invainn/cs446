#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "Config.h"
#include "MetaDataCode.h"

void calculateProcessingTime(Config, MetaDataCode&, int&, int&);
void readMetaDataFile(std::string, std::vector<MetaDataCode>&);
void outputToLogFile(Config, std::vector<MetaDataCode>);
void output(Config, std::vector<MetaDataCode>, std::ostream&, int);

// Simulation starts here
// Might need to refactor into separate class depending on future projects
int main(int argc, char *argv[]) {
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


// Gets code and validates descriptor and calculates process time
// Throws error if system or application order does not make sense (finishing before starting application)
void calculateProcessingTime(Config cf, MetaDataCode& mdc, int& systemStatus, int& applicationStatus) {
	if(mdc.getCode() == 'S') {
		if(mdc.getDescriptor() == "begin" && systemStatus == 0) {
			systemStatus = 1;
		} else if(mdc.getDescriptor() == "finish" && systemStatus == 1 && !applicationStatus) {
			systemStatus = 2;
		} else {
			std::cerr << "Missing begin or finish operation for OS!\n";
			exit(1);
		}
	} else if(mdc.getCode() == 'A') {
		if(mdc.getDescriptor() == "begin" && applicationStatus == 0) {
			applicationStatus = 1;
		} else if(mdc.getDescriptor() == "finish" && applicationStatus == 1) {
			applicationStatus = 0;
		} else {
			std::cerr << "Missing begin or finish operation for OS!\n";
			exit(1);
		}
	} else if(mdc.getCode() == 'P' && mdc.getDescriptor() == "run") {
		mdc.setProcessingTime(mdc.getCycles() * cf.getPCT());
	} else if(mdc.getCode() == 'I') {
		if(mdc.getDescriptor() == "hard drive") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getHCT());
		} else if(mdc.getDescriptor() == "keyboard") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getKCT());
		} else if(mdc.getDescriptor() == "scanner") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getSCT());
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'O') {
		if(mdc.getDescriptor() == "hard drive") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getHCT());
		} else if(mdc.getDescriptor() == "monitor") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getMDT());
		} else if(mdc.getDescriptor() == "projector") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getProCT());
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'M') {
		if(mdc.getDescriptor() == "block") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getMemCT());
		} else if(mdc.getDescriptor() == "allocate") {
			mdc.setProcessingTime(mdc.getCycles() * cf.getMemCT());
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	}
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
	out << "Configuration File Data" << std::endl;
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
	}
	
	out << std::endl;
	out << "Meta-Data Metrics" << std::endl;
	
	for(auto it = mdv.begin(); it != mdv.end(); it++) {
		MetaDataCode mdc = *it;
		if(!(mdc.getCode() == 'S') && !(mdc.getCode() == 'A')) {
			out << mdc.getData() << " - " << mdc.getProcessingTime() << " ms" << std::endl;
		}
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
