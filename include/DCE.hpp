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

class DCE : public ISubject {
private:
    std::thread captureThread;
    std::atomic<bool> running;

    std::string tapName;
    pcap_t* pcapHandle;
    char errbuf[PCAP_ERRBUF_SIZE];
    std::vector<std::shared_ptr<IObserver>> observers;
    bool capturing;
    libnet_t* lnet;

    static void packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet);

public:
    explicit DCE(const std::string& tap);
    ~DCE();

    void attach(std::shared_ptr<IObserver> observer) override;
    void detach(std::shared_ptr<IObserver> observer) override;
    void notify(const std::vector<uint8_t>& packet) override;
    void startCapture();
    void stopCapture();
    void transmitFrame(const EthernetFrame& frame);
};

#endif // DCE_HPP
