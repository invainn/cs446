#include <iostream>
#include <string>

class MetaDataCode {
	public:
		// Main Constructor
		MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string data);

		void setProcessingTime(double processingTimeInput);

		char getCode();
		std::string getDescriptor();
		int getCycles();
		std::string getData();
		double getProcessingTime();
	private:
		char code;
		std::string descriptor;
		int cycles;
		std::string data;

		double processingTime;
};
