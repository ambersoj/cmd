#ifndef RXOBSERVER_HPP
#define RXOBSERVER_HPP

#include "IObserver.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>

class RxObserver : public IObserver {
private:
    std::queue<std::vector<uint8_t>> rxBuffer;
    std::mutex bufferMutex;

public:
    void update(const std::vector<uint8_t>& packet) override;
    std::vector<uint8_t> getNextPacket();
};

#endif // RXOBSERVER_HPP
