#ifndef DCE_HPP
#define DCE_HPP

#include <algorithm>
#include "Observer.hpp"
#include <pcap.h>
#include <libnet.h>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include "EthernetFrame.hpp"

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

    static void packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
        auto dce = reinterpret_cast<DCE*>(userData);
        std::vector<uint8_t> packetData(packet, packet + pkthdr->caplen);
        dce->notify(packetData);
    }

public:
    explicit DCE(const std::string& tap) : tapName(tap), capturing(false) {
        pcapHandle = pcap_open_live(tapName.c_str(), BUFSIZ, 1, 1000, errbuf);
        if (!pcapHandle) {
            throw std::runtime_error("Failed to open TAP device for pcap: " + std::string(errbuf));
        }

        lnet = libnet_init(LIBNET_LINK, tapName.c_str(), errbuf);
        if (!lnet) {
            throw std::runtime_error("Failed to initialize libnet: " + std::string(errbuf));
        }
    }

    ~DCE() {
        stopCapture();
        if (pcapHandle) {
            pcap_close(pcapHandle);
        }
        if (lnet) {
            libnet_destroy(lnet);
        }
    }

    void attach(std::shared_ptr<IObserver> observer) override {
        observers.push_back(observer);
    }

    void detach(std::shared_ptr<IObserver> observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notify(const std::vector<uint8_t>& packet) override {
        for (auto& observer : observers) {
            observer->update(packet);
        }
    }

    void startCapture() {
        running = true;
        captureThread = std::thread([this]() {
            pcap_loop(pcapHandle, 0, packetHandler, reinterpret_cast<u_char*>(this));
        });
    }

    void stopCapture() {
        running = false;
        pcap_breakloop(pcapHandle);
        if (captureThread.joinable()) {
            captureThread.join();
        }
    }

    void transmitFrame(const EthernetFrame& frame) {
        libnet_clear_packet(lnet);  // Ensure a clean buffer before writing
    
        libnet_ptag_t ethernetTag = libnet_build_ethernet(
            frame.getDstMac().data(), frame.getSrcMac().data(),
            ETHERTYPE_IP, frame.getPayload().data(), frame.getPayload().size(),
            lnet, 0
        );

        if (ethernetTag == -1) {
            throw std::runtime_error("Failed to build Ethernet frame: " + std::string(libnet_geterror(lnet)));
        }

        int bytesWritten = libnet_write(lnet);
        if (bytesWritten == -1) {
            throw std::runtime_error("Failed to send Ethernet frame: " + std::string(libnet_geterror(lnet)));
        }
    }
};

#endif // DCE_HPP
