#include "EthernetFrame.hpp"
#include <cstring>

EthernetFrame::EthernetFrame() : payloadLength(0) {}

void EthernetFrame::setData(const uint8_t* data, size_t length) {
    if (length > payload.size()) length = payload.size();
    std::memcpy(payload.data(), data, length);
    payloadLength = length;
}

const std::array<uint8_t, 1500>& EthernetFrame::getData() const {
    return payload;
}
