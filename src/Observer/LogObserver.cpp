#include <fstream>
#include "Observer/LogObserver.h"

void LogObserver::Update(ILoggable* loggable) {
    std::string line = loggable->stringToLog();
    std::fstream logfile("gamelog.txt", std::ios::app);
    logfile << line << std::endl;
    logfile.close();
};