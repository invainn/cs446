#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

#include "Config.h"

// Nothing since readConfigFile fills the object
Config::Config() {}

// Reads entire file into a string, then splits string into tokens and parsed one by one
void Config::readConfigFile(std::string configPath) {
	// File stream	
	std::ifstream configFile;
	configFile.open(configPath);

	// Throw error if unable to open file
	if(!configFile) {
		std::cerr << "Unable to open file\n";
		exit(1);
	}

	// This vector holds each individual "token"
	std::vector<std::string> tokens;
	std::string s;

	// Read a string of text up until whitespace and pushes to vector
	while(configFile >> s) {
		tokens.push_back(s);
	}

	// Iterates through vector checking for values
	for(auto it = tokens.begin(); it != tokens.end(); it++) {
		if(*it == "Log:") {
			this->logTo = *(it+3);
		} else if(*it == "File") {
			s = *(it+2);
			if(s.find(".mdf") != std::string::npos) {
				this->filePath = s;
			}
		} else if(*it == "Version/Phase:") {
			this->version = std::stof(*(it+1));
		} else if(*it == "Log") {
			std::string temp = *(it+3);
			if(temp.find(".lgf") != std::string::npos) {
				this->logFilePath = temp;
			}
		} else if(*it == "Monitor") {
			this->monitorDisplayTime = std::stoi(*(it+4));
		} else if(*it == "Processor") {
			this->processorCycleTime = std::stoi(*(it+4));
		} else if(*it == "Scanner") {
			this->scannerCycleTime = std::stoi(*(it+4));
		} else if(*it == "Hard") {
			this->harddriveCycleTime = std::stoi(*(it+5));
		} else if(*it == "Keyboard") {
			this->keyboardCycleTime = std::stoi(*(it+4));
		} else if(*it == "Memory") {
			this->memoryCycleTime = std::stoi(*(it+4));
		} else if(*it == "Projector") {
			this->projectorCycleTime = std::stoi(*(it+4));
		}
	}
	configFile.close();

	if(!this->validate()) {
		std::cerr << "Missing Data!\n" << std::endl;
		exit(1);
	}
}

bool Config::validate() {
	bool monitor = monitorDisplayTime > 0;
	bool processor = this->processorCycleTime > 0;
	bool scanner = this->scannerCycleTime > 0;
	bool harddrive = this->harddriveCycleTime > 0;
	bool keyboard = this->keyboardCycleTime > 0;
	bool memory = this->memoryCycleTime > 0;
	bool projector = this->projectorCycleTime > 0;

	bool ver = this->version > 0;

	return monitor && processor && scanner && harddrive && keyboard && memory && projector && ver;
}

std::string Config::getFilePath() {
	return this->filePath;
}

int Config::getMDT() {
	return this->monitorDisplayTime;
}

int Config::getPCT() {
	return this->processorCycleTime;
}

int Config::getSCT() {
	return this->scannerCycleTime;
} 

int Config::getHCT() {
	return this->harddriveCycleTime;
}

int Config::getKCT() {
	return this->keyboardCycleTime;
}

int Config::getMemCT() {
	return this->memoryCycleTime;
}

int Config::getProCT() {
	return this->projectorCycleTime;
}

std::string Config::getLogTo() {
	return this->logTo;
}

std::string Config::getLogFilePath() {
	return this->logFilePath;
}


