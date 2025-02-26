#include "EthernetFrame.hpp"
#include <cassert>
#include <iostream>

void test_EthernetFrame() {
    EthernetFrame frame;
    frame.setSrcMac({0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF});
    frame.setDstMac({0x11, 0x22, 0x33, 0x44, 0x55, 0x66});
    frame.setPayload({'H', 'e', 'l', 'l', 'o'});

    std::vector<uint8_t> serialized = frame.serialize();
    EthernetFrame deserialized = frame.deserialize(serialized);

    assert(deserialized.getSrcMac() == frame.getSrcMac());
    assert(deserialized.getDstMac() == frame.getDstMac());
    assert(deserialized.getPayload() == frame.getPayload());

    std::cout << "EthernetFrame tests passed!" << std::endl;
}

int main() {
    test_EthernetFrame();
    return 0;
}
