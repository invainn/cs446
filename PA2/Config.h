#include <iostream>
#include <string>

class Config {
	public:
		Config();
		void readConfigFile(std::string configPath);
		void readMetaDataFile();
		bool validate();
		float getVersion();
		std::string getFilePath();
		int getMDT();
		int getPCT();
		int getSCT();
		int getHCT();
		int getKCT();
		int getMemCT();
		int getProCT();
		std::string getLogTo();
		std::string getLogFilePath();
	private:
		float version;
		std::string filePath;
		int monitorDisplayTime;
		int processorCycleTime;
		int scannerCycleTime;
		int harddriveCycleTime;
		int keyboardCycleTime;
		int memoryCycleTime;
		int projectorCycleTime;

		int memory;
		std::string logTo;
		std::string logFilePath;
		
};
