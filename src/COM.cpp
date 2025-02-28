#include "COM.hpp"
#include "RxObserver.hpp"
#include "EthernetFrame.hpp"
#include <pcap.h>
#include <libnet.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <mutex>

COM::COM(const std::string& tapName, const std::string& macAddress) 
    : tapName(tapName), macAddress(macAddress) {
    if (!initializeTAP()) {
        throw std::runtime_error("Failed to initialize TAP device " + tapName);
    }
    if (!initializeLibnet()) {
        throw std::runtime_error("Failed to initialize libnet: " + std::string(errbuf));
    }
    if (!initializePcap()) {
        pcapHandle = pcap_open_live(tapName.c_str(), BUFSIZ, 1, 1000, errbuf);
        if (!pcapHandle) {
            throw std::runtime_error("Failed to open TAP device for pcap: " + std::string(errbuf));
        }
    }
    std::cout << "COM initialized for " << tapName << " with MAC " << macAddress << "\n";
}

bool COM::initializeTAP() {
    std::cout << "Creating TAP device " << tapName << "\n";
    return !tapName.empty();
}

bool COM::initializeLibnet() {
    lnet = libnet_init(LIBNET_LINK, tapName.c_str(), errbuf);
    if (!lnet) {
        throw std::runtime_error("Failed to initialize libnet: " + std::string(libnet_geterror(lnet)));
    }
    std::cout << "Initializing libnet for " << tapName << "\n";
    return true;
}

bool COM::initializePcap() {
    std::cout << "Initializing pcap for " << tapName << "\n";
    return !tapName.empty();
}

void COM::transmitFrame(const EthernetFrame& frame) {
    libnet_clear_packet(lnet);

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

void COM::attach(std::shared_ptr<IObserver> observer) {
    observers.push_back(observer);
}

void COM::detach(std::shared_ptr<IObserver> observer) {
    observers.erase(std::remove_if(observers.begin(), observers.end(),
    [&observer](const std::shared_ptr<IObserver>& o) { return o == observer; }),
    observers.end());
}

void COM::notify(const std::vector<uint8_t>& packet) {
    for (auto& observer : observers) {
        observer->update(packet);
    }
}

void COM::startCapture() {
    running = true;
    captureThread = std::thread([this]() {
        pcap_loop(pcapHandle, 0, packetHandler, reinterpret_cast<u_char*>(this));
    });
}

void COM::stopCapture() {
    running = false;
    pcap_breakloop(pcapHandle);
    if (captureThread.joinable()) {
        captureThread.join();
    }
}

void COM::packetHandler(u_char* userData, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    COM* comInstance = reinterpret_cast<COM*>(userData);
    
    std::vector<uint8_t> packetData(packet, packet + pkthdr->caplen);

    {
        std::lock_guard<std::mutex> lock(comInstance->bufferMutex);
        comInstance->rxBuffer.push(packetData);
    }

    comInstance->notify(packetData);
}

std::vector<uint8_t> COM::getNextPacket() {
    std::lock_guard<std::mutex> lock(bufferMutex);
    if (!rxBuffer.empty()) {
        std::vector<uint8_t> packet = rxBuffer.front();
        rxBuffer.pop();
        return packet;
    }
    return {}; // Return empty if no packets available
}

std::string COM::getMacAddress() const {
    return macAddress;
}

