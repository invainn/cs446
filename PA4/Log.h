#include <iostream>
#include <string>
#include <fstream>

#include "Config.h"

#ifndef LOG_H
#define LOG_H
class Log {
    private:
        Log();


    public:
        bool didFirstLog;
        static void outputToStream(std::ostream&, std::string);

        static void output(Config, std::string);       
};
#endif