#include <iostream>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H
class Config {
	public:
		Config();
		void readConfigFile(std::string configPath);
		void readMetaDataFile();
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
		std::string getSchedulingAlgorithm();
		int getMemoryBlockSize();
		int getMaxMemorySize();
		int getProjectorResources();
		int getHarddriveResources();

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

		int projectorResources;
		int harddriveResources;

		int memoryBlockSize;
		int maxMemorySize;
		std::string maxMemoryType;
		std::string memoryBlockType;
		std::string logTo;
		std::string logFilePath;
		std::string schedulingAlgorithm;

		bool validate();
		void convertMemoryType(int&, std::string);
		
};
#endif