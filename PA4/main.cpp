#include <iostream>
#include <string>

#include "Config.h"
#include "MetaDataCode.h"
#include "Log.h"
#include "OperatingSystem.h"
#include "Process.h"

int main(int argc, char* argv[]) {
    std::string fileName(argv[1]);

    Config cf;
    OperatingSystem os(&cf, fileName);
    auto processQueue = os.getProcesses();

    Process p = processQueue.front();

    os.process(p,&cf);

    return 0;
}