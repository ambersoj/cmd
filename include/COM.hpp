#ifndef COM_HPP
#define COM_HPP

#include "EthernetFrame.hpp"
#include "IObserver.hpp"
#include <pcap.h>
#include <libnet.h>
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>

constexpr std::array<const char*, 6> MAC_ADDRESSES = {
    "00:11:22:33:44:00", "00:11:22:33:44:01", "00:11:22:33:44:02",
    "00:11:22:33:44:03", "00:11:22:33:44:04", "00:11:22:33:44:05"
};

class COM {
public:
    COM(const std::string& tapName, const std::string& macAddress);

    void transmitFrame(const EthernetFrame& frame);  // Updated signature

    void attach(std::shared_ptr<IObserver> observer);
    void detach(std::shared_ptr<IObserver> observer);
    void notify(const std::vector<uint8_t>& packet);

    void startCapture();
    void stopCapture();

    std::vector<uint8_t> getNextPacket();
    std::string getMacAddress() const;

private:
    int tapFd;
    std::string tapName;
    std::string macAddress;
    pcap_t* pcapHandle;
    libnet_t* lnet;
    char errbuf[PCAP_ERRBUF_SIZE];

    std::queue<std::vector<uint8_t>> rxBuffer;
    std::mutex bufferMutex;
    std::vector<std::shared_ptr<IObserver>> observers;
    bool running;
    std::thread captureThread;

    bool initializeTAP();
    bool initializeLibnet();
    bool initializePcap();

    static void packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet);

    static void getNextPacket(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet);
};

#endif // COM_HPP
