#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <vector>
#include <memory>
#include <iostream>
#include <queue>
#include <mutex>


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
private:
    std::queue<std::vector<uint8_t>> rxBuffer;
    std::mutex bufferMutex;

public:
    void update(const std::vector<uint8_t>& packet) override {
        std::lock_guard<std::mutex> lock(bufferMutex);
        rxBuffer.push(packet);  // Store the packet in the RX buffer
        std::cout << "RxObserver received packet of size: " << packet.size() << " bytes" << std::endl;
    }

    std::vector<uint8_t> getNextPacket() {
        std::lock_guard<std::mutex> lock(bufferMutex);
        if (!rxBuffer.empty()) {
            std::vector<uint8_t> packet = rxBuffer.front();
            rxBuffer.pop();
            return packet;
        }
        return {};  // Return an empty packet if buffer is empty
    }
};

#endif // OBSERVER_HPP
