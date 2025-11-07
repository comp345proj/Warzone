#include "Observer/LogObserver.h"
#include <fstream>

void LogObserver::Update(ILoggable* loggable) {
	std::string line = loggable->stringToLog();
	std::fstream logfile("gamelog.txt", std::ios::app);
	logfile << line << std::endl;
	logfile.close();
};