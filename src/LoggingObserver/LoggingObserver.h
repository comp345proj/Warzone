#pragma once
#include <list>
#include <string>

// Forward declarations
class Observer;

// ILoggable interface - inherited by all loggable classes
class ILoggable {
  public:
    virtual std::string stringToLog() = 0;
};

// Observer abstract class
class Observer {
  public:
    Observer();
    virtual ~Observer();
    virtual void Update(ILoggable* loggable) = 0;
};

// A subject (game engine) that manages observers
class Subject {
  public:
    virtual void Attach(Observer* o);
    virtual void Detach(Observer* o);
    virtual void Notify(ILoggable* loggable);
    Subject();
    virtual ~Subject();

  private:
    std::list<Observer*>* observers;
};

// LogObserver - concrete observer that logs to gamelog.txt
class LogObserver : public Observer {
  private:
    static bool fileCleared; // Track if file has been cleared this run

  public:
    LogObserver();
    virtual ~LogObserver();
    void Update(ILoggable* loggable) override;
};
