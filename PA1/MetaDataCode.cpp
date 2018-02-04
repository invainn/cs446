#include <iostream>
#include <string>

#include "MetaDataCode.h"

MetaDataCode::MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string dataInput) {
	this->code = codeInput;
	this->descriptor = descriptorInput;
	this->cycles = cyclesInput;
	this->data = dataInput;
}

void MetaDataCode::setProcessingTime(int processingTimeInput) {
	this->processingTime = processingTimeInput;
}

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

int MetaDataCode::getProcessingTime() {
	return this->processingTime;
}
