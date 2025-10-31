#pragma once
#include "Observer/Observer.h"

class LogObserver : public Observer {
	void Update(ILoggable* loggable) override;
};