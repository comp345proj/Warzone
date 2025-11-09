#include "LoggingObserver/LoggingObserver.h"
#include <fstream>
#include <iostream>

// Observer implementation
Observer::Observer() = default;
Observer::~Observer() = default;

// Subject implementation
Subject::Subject() {
    _observers = new std::list<Observer*>();
}

Subject::~Subject() {
    delete _observers;
}

void Subject::Attach(Observer* o) {
    _observers->push_back(o);
}

void Subject::Detach(Observer* o) {
    _observers->remove(o);
}

void Subject::Notify(ILoggable* loggable) {
    std::list<Observer*>::iterator i = _observers->begin();
    for (; i != _observers->end(); ++i) {
        (*i)->Update(loggable);
    }
}

// LogObserver implementation
bool LogObserver::fileCleared = false;

LogObserver::LogObserver() {
    // Clear the log file on first LogObserver creation
    if (!fileCleared) {
        std::ofstream logfile("gamelog.txt", std::ios::trunc);
        logfile.close();
        fileCleared = true;
    }
}

void LogObserver::Update(ILoggable* loggable) {
    std::string line = loggable->stringToLog();
    std::fstream logfile("gamelog.txt", std::ios::app);
    logfile << line << std::endl;
    logfile.close();

    std::cout << line << std::endl;
}
