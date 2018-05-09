#include <iostream>
#include <string>
#include <fstream>

#include "Config.h"

#ifndef LOG_H
#define LOG_H
class Log {
    private:
        static bool didFirstLog;
        static bool checkFirstLog();

    public:
        static void outputToStream(std::ostream&, std::string);

        static void output(Config, std::string);       
};
#endif