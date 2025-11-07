#pragma once
#include <string>

class ILoggable {
  public:
	virtual std::string stringToLog() = 0;
};

class Observer {
  public:
	Observer();
	~Observer();
	virtual void Update(ILoggable* loggable) = 0;
};