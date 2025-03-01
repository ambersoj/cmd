#ifndef ETHERNETFRAME_HPP
#define ETHERNETFRAME_HPP

#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstdint>

class EthernetFrame {
public:
    EthernetFrame(const std::array<uint8_t, 6>& srcMac, 
                  const std::array<uint8_t, 6>& dstMac, 
                  const std::vector<uint8_t>& payload);

    std::vector<uint8_t> serialize() const;
    static EthernetFrame deserialize(const std::vector<uint8_t>& data);

    void setSrcMac(const std::array<uint8_t, 6>& srcMac);
    void setDstMac(const std::array<uint8_t, 6>& dstMac);
    void setPayload(const std::vector<uint8_t>& payload);
    
    const std::array<uint8_t, 6>& getSrcMac() const;
    const std::array<uint8_t, 6>& getDstMac() const;
    const std::vector<uint8_t>& getPayload() const;  // Prevent accidental modification
    
    std::string macToString(const std::array<uint8_t, 6>& mac) const;
    static std::array<uint8_t, 6> stringToMac(const std::string& macStr);

private:
    std::array<uint8_t, 6> srcMac;
    std::array<uint8_t, 6> dstMac;
    std::vector<uint8_t> payload;
};

#endif
