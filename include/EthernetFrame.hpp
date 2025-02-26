// EthernetFrame.hpp
#ifndef ETHERNETFRAME_HPP
#define ETHERNETFRAME_HPP

#include <array>
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

class EthernetFrame {
public:
    EthernetFrame();
    EthernetFrame(const std::array<uint8_t, 6>& src, const std::array<uint8_t, 6>& dst, const std::vector<uint8_t>& data);
    std::vector<uint8_t> serialize() const;
    EthernetFrame deserialize(const std::vector<uint8_t>& data);
    void setSrcMac(const std::array<uint8_t, 6>& mac);
    void setDstMac(const std::array<uint8_t, 6>& mac);
    void setPayload(const std::vector<uint8_t>& data);
    
    const std::array<uint8_t, 6>& getSrcMac() const;
    const std::array<uint8_t, 6>& getDstMac() const;
    std::vector<uint8_t>& getPayload();
    
    std::string macToString(const std::array<uint8_t, 6>& mac) const;
    static std::array<uint8_t, 6> stringToMac(const std::string& macStr);

private:
    std::array<uint8_t, 6> srcMac;
    std::array<uint8_t, 6> dstMac;
    std::vector<uint8_t> payload;
};

#endif // ETHERNETFRAME_HPP
