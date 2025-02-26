#include "EthernetFrame.hpp"
#include <cstring>
#include <stdexcept>
#include <vector>

EthernetFrame::EthernetFrame() : srcMac{}, dstMac{}, payload{} {}

void EthernetFrame::setSrcMac(const std::array<uint8_t, 6>& mac) {
    srcMac = mac;
}

void EthernetFrame::setDstMac(const std::array<uint8_t, 6>& mac) {
    dstMac = mac;
}

void EthernetFrame::setPayload(const std::vector<uint8_t>& data) {
    payload = data;
}

const std::array<uint8_t, 6>& EthernetFrame::getSrcMac() const {
    return srcMac;
}

const std::array<uint8_t, 6>& EthernetFrame::getDstMac() const {
    return dstMac;
}

std::vector<uint8_t>& EthernetFrame::getPayload() {
    return payload;
}

std::vector<uint8_t> EthernetFrame::serialize() const {
    std::vector<uint8_t> data;
    data.insert(data.end(), srcMac.begin(), srcMac.end());
    data.insert(data.end(), dstMac.begin(), dstMac.end());
    data.insert(data.end(), payload.begin(), payload.end());
    return data;
}

EthernetFrame EthernetFrame::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 12) {
        throw std::runtime_error("Invalid Ethernet frame size");
    }
    
    EthernetFrame frame;
    std::copy(data.begin(), data.begin() + 6, frame.srcMac.begin());
    std::copy(data.begin() + 6, data.begin() + 12, frame.dstMac.begin());
    frame.getPayload().assign(data.begin() + 12, data.end());
    
    return frame;
}
