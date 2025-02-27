#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <vector>
#include <memory>
#include <iostream>

// Observer Interface
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const std::vector<uint8_t>& packet) = 0;
};

// Subject Interface
class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void attach(std::shared_ptr<IObserver> observer) = 0;
    virtual void detach(std::shared_ptr<IObserver> observer) = 0;
    virtual void notify(const std::vector<uint8_t>& packet) = 0;
};

// Concrete Observer
class RxObserver : public IObserver {
public:
    void update(const std::vector<uint8_t>& packet) override {
        // Process received packet
        std::cout << "RxObserver received packet of size: " << packet.size() << " bytes" << std::endl;
    }
};

#endif // OBSERVER_HPP
