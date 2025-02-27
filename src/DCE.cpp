#include "DCE.hpp"
#include <algorithm>
#include <iostream>

DCE::DCE(const std::string& tap) : tapName(tap), capturing(false), running(false) {
    pcapHandle = pcap_open_live(tapName.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (!pcapHandle) {
        throw std::runtime_error("Failed to open TAP device for pcap: " + std::string(errbuf));
    }

    lnet = libnet_init(LIBNET_LINK, tapName.c_str(), errbuf);
    if (!lnet) {
        throw std::runtime_error("Failed to initialize libnet: " + std::string(errbuf));
    }
}

DCE::~DCE() {
    stopCapture();
    if (pcapHandle) {
        pcap_close(pcapHandle);
    }
    if (lnet) {
        libnet_destroy(lnet);
    }
}

void DCE::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    auto dce = reinterpret_cast<DCE*>(userData);
    std::vector<uint8_t> packetData(packet, packet + pkthdr->caplen);
    dce->notify(packetData);
}

void DCE::attach(std::shared_ptr<IObserver> observer) {
    observers.push_back(observer);
}

void DCE::detach(std::shared_ptr<IObserver> observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void DCE::notify(const std::vector<uint8_t>& packet) {
    for (auto& observer : observers) {
        observer->update(packet);
    }
}

void DCE::startCapture() {
    running = true;
    captureThread = std::thread([this]() {
        pcap_loop(pcapHandle, 0, packetHandler, reinterpret_cast<u_char*>(this));
    });
}

void DCE::stopCapture() {
    running = false;
    pcap_breakloop(pcapHandle);
    if (captureThread.joinable()) {
        captureThread.join();
    }
}

void DCE::transmitFrame(const EthernetFrame& frame) {
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
