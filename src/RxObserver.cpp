#include "RxObserver.hpp"
#include <string>

void RxObserver::update(const std::vector<uint8_t>& packet) {
    std::lock_guard<std::mutex> lock(bufferMutex);
    rxBuffer.push(packet);
//    std::cout << "RxObserver received packet of size: " << packet.size() << " bytes" << std::endl;
}

std::vector<uint8_t> RxObserver::getNextPacket() {
    std::lock_guard<std::mutex> lock(bufferMutex);
    if (!rxBuffer.empty()) {
        std::vector<uint8_t> packet = rxBuffer.front();
        rxBuffer.pop();
        return packet;
    }
    return {};  // Return empty if no packets available
}
