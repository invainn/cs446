#include <iostream>
#include <deque>

#ifndef METADATACODE_H
#define METADATACODE_H
class MetaDataCode {
	public:
		// Main Constructor
		// other constructor header
		MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string data);

		void setStartTime(double startTimeInput);
		void setProcessingTime(double processingTimeInput);

		static void readMetaDataFile(std::string, std::deque<MetaDataCode>&);

		char getCode();
		std::string getDescriptor();
		std::string getData();

		int getCycles();
		double getProcessingTime();
		double getStartTime();
	private:
		char code;
		std::string descriptor;
		std::string data;

		int cycles;
		double processingTime;
		double startTime;
};
#endif