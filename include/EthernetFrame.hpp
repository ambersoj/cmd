#ifndef ETHERNETFRAME_HPP
#define ETHERNETFRAME_HPP

#include <libnet.h>
#include <cstdint>
#include <array>

class EthernetFrame {
public:
    EthernetFrame();
    size_t getSize() const { return 0; } // Placeholder for now
    void setData(const uint8_t* data, size_t length);
    const std::array<uint8_t, 1500>& getData() const;
private:
    std::array<uint8_t, 1500> payload;
    size_t payloadLength;
};

#endif // ETHERNETFRAME_HPP
