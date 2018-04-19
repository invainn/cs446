#include <iostream>
#include <string>
#include <fstream>

#include "Log.h"
#include "Config.h"

Log::Log() {
    this->didFirstLog = false;
}

void Log::output(Config cf, std::string logOutput) {
	std::ofstream logFile;

	// these flags are set depending on where the config specifies where to log to
	bool monitorFlag = false;
	bool logFileFlag = false;

	if(cf.getLogTo() == "Both") {
		monitorFlag = true;
		logFileFlag = true;
	} else if(cf.getLogTo() == "monitor") {
		monitorFlag = true;
	} else if(cf.getLogTo() == "file") {
		logFileFlag = true;
	} else {
		std::cerr << "Cannot log to " << cf.getLogTo() << "!\n";
		exit(1);
	}

	if(monitorFlag) {
		Log::outputToStream(std::cout, logOutput);
	}

	if(logFileFlag) {
		logFile.open(cf.getLogFilePath(),std::ofstream::app);

		Log::outputToStream(logFile, logOutput);
		logFile.close();
	}
}

void Log::outputToStream(std::ostream& out, std::string logOutput) {
    out << logOutput << std::endl;
}