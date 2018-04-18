#include <iostream>

#ifndef METADATACODE_H
#define METADATACODE_H
class MetaDataCode {
	public:
		// Main Constructor
		MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string data);

		void setStartTime(double startTimeInput);
		void setProcessingTime(double processingTimeInput);

		char getCode();
		std::string getDescriptor();
		int getCycles();
		std::string getData();
		double getProcessingTime();
		double getStartTime();
	private:
		char code;
		std::string descriptor;
		int cycles;
		std::string data;

		double processingTime;
		double startTime;
};
#endif