#pragma once
#include <string>

// Design Pattern: Observer
class IObserver {
public:
    virtual void onEvent(const std::string& event) = 0;
    virtual ~IObserver() = default;
};

class ISubject {
public:
    virtual void addObserver(IObserver* obs) = 0;
    virtual void notifyObservers(const std::string& event) = 0;
    virtual ~ISubject() = default;
};