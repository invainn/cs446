#include <iostream>
#include <string>
#include <deque>
#include <fstream>
#include <algorithm>

#include "MetaDataCode.h"

// constructor
MetaDataCode::MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string dataInput) {
	this->code = codeInput;
	this->descriptor = descriptorInput;
	this->cycles = cyclesInput;
	this->data = dataInput;
}

// only setter since I calculate processing time after construction
void MetaDataCode::setProcessingTime(double processingTimeInput) {
	this->processingTime = processingTimeInput;
}

void MetaDataCode::setStartTime(double startTimeInput) {
	this->startTime = startTimeInput;
}

// getters
char MetaDataCode::getCode() {
	return this->code;
}

std::string MetaDataCode::getDescriptor() {
	return this->descriptor;
}

int MetaDataCode::getCycles() {
	return this->cycles;
}

std::string MetaDataCode::getData() {
	return this->data;
}

double MetaDataCode::getProcessingTime() {
	return this->processingTime;
}

double MetaDataCode::getStartTime() {
	return this->startTime;
}

void MetaDataCode::readMetaDataFile(std::string filePath, std::deque<MetaDataCode>& MetaDataVector) {
	std::ifstream metaDataFile;
	metaDataFile.open(filePath);

	std::deque<std::string> tokens;
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