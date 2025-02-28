#ifndef DCE_HPP
#define DCE_HPP

#include "ISubject.hpp"
#include "EthernetFrame.hpp"
#include <pcap.h>
#include <libnet.h>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <stdexcept>

class DCE {
public:
    DCE(const std::string& tapName, const std::string& macAddress);
    bool initializeTAP();
    bool initializeLibnet();
    bool initializePcap();
    void transmitFrame(const std::string &dstMac, const std::string &data);
    std::string getNextPacket();
    std::string getMacAddress() const;
private:
    std::string tapName;
    std::string macAddress;
};

#endif // DCE_HPP
