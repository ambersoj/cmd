#include "EthernetFrame.hpp"

// Proper constructor implementation
EthernetFrame::EthernetFrame(const std::array<uint8_t, 6>& srcMac, 
                             const std::array<uint8_t, 6>& dstMac, 
                             const std::vector<uint8_t>& payload) 
    : srcMac(srcMac), dstMac(dstMac), payload(payload) {}

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

const std::vector<uint8_t>& EthernetFrame::getPayload() const {
    return payload;
}

// Serialize frame to a byte vector
std::vector<uint8_t> EthernetFrame::serialize() const {
    std::vector<uint8_t> data;
    data.insert(data.end(), srcMac.begin(), srcMac.end());
    data.insert(data.end(), dstMac.begin(), dstMac.end());
    data.insert(data.end(), payload.begin(), payload.end());
    return data;
}

// Deserialize raw bytes into an EthernetFrame
EthernetFrame EthernetFrame::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 12) {
        throw std::runtime_error("Invalid Ethernet frame size");
    }

    std::array<uint8_t, 6> src, dst;
    std::copy(data.begin(), data.begin() + 6, src.begin());
    std::copy(data.begin() + 6, data.begin() + 12, dst.begin());
    
    std::vector<uint8_t> payload(data.begin() + 12, data.end());
    return EthernetFrame(src, dst, payload);
}

// Convert MAC address to string
std::string EthernetFrame::macToString(const std::array<uint8_t, 6>& mac) const {
    std::ostringstream oss;
    for (size_t i = 0; i < mac.size(); ++i) {
        if (i > 0) oss << ":";
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
    }
    return oss.str();
}

// Convert string MAC address (e.g., "AA:BB:CC:DD:EE:FF") to array
std::array<uint8_t, 6> EthernetFrame::stringToMac(const std::string& macStr) {
    std::array<uint8_t, 6> mac{};
    unsigned int values[6];
    
    if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", 
               &values[0], &values[1], &values[2], 
               &values[3], &values[4], &values[5]) != 6) {
        throw std::invalid_argument("Invalid MAC address format");
    }
    
    for (size_t i = 0; i < 6; ++i) {
        mac[i] = static_cast<uint8_t>(values[i]);
    }
    return mac;
}
