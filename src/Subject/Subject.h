#pragma once
#include "Observer/Observer.h"
#include <list>

class Observer;

class Subject {
  public:
    virtual void Attach(Observer* o);
    virtual void Detach(Observer* o);
    virtual void Notify(ILoggable* loggable);
    Subject();
    ~Subject();

  private:
    std::list<Observer*>* _observers;
};
