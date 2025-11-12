#include "LoggingObserver/LoggingObserver.h"
#include <fstream>
#include <iostream>

//---------------------------Observer-------------------------------
Observer::Observer() = default;
Observer::~Observer() = default;

//---------------------------Subject--------------------------------
Subject::Subject() {
    observers = new std::list<Observer*>();
}

Subject::~Subject() {
    delete observers;
}

void Subject::Attach(Observer* o) {
    observers->push_back(o);
}

void Subject::Detach(Observer* o) {
    observers->remove(o);
}

void Subject::Notify(ILoggable* loggable) {
    std::list<Observer*>::iterator i = observers->begin();
    for (; i != observers->end(); ++i) {
        (*i)->Update(loggable);
    }
}

//---------------------------LogObserver----------------------------
bool LogObserver::fileCleared = false;

LogObserver::LogObserver() {
    // Clear the log file on first LogObserver creation
    if (!fileCleared) {
        std::ofstream logfile("gamelog.txt", std::ios::trunc);
        logfile.close();
        fileCleared = true;
    }
}

LogObserver::~LogObserver() = default;

void LogObserver::Update(ILoggable* loggable) {
    std::string line = loggable->stringToLog();
    std::fstream logfile("gamelog.txt", std::ios::app);
    logfile << line << std::endl;
    logfile.close();

    std::cout << line << std::endl;
}
