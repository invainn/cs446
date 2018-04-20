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

    // simulator starts once processes and data is ready
    os.processSystemApp("Simulator program starting", &cf);//

    // should schedule processes here

    // processes each process in the scheduled process queue
    for(auto process : processQueue) {
        os.process(process, &cf);
    }

    os.processSystemApp("Simulator program ending", &cf);//

    return 0;
}