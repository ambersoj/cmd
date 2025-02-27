#ifndef ISUBJECT_HPP
#define ISUBJECT_HPP

#include "IObserver.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <queue>
#include <mutex>


// Subject Interface
class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void attach(std::shared_ptr<IObserver> observer) = 0;
    virtual void detach(std::shared_ptr<IObserver> observer) = 0;
    virtual void notify(const std::vector<uint8_t>& packet) = 0;
};

#endif
