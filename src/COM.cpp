#include "COM.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_tun.h>

// Constructor
COM::COM(const std::string& tapName, const std::string& macAddress)
    : tapName(tapName), macAddress(macAddress), tapFd(-1), lnet(nullptr), pcapHandle(nullptr) {

    if (!initializeTAP()) {
        throw std::runtime_error("Failed to initialize TAP device " + tapName);
    }
    if (!initializeLibnet()) {
        throw std::runtime_error("Failed to initialize libnet: " + std::string(errbuf));
    }
    if (!initializePcap()) {
        throw std::runtime_error("Failed to initialize pcap for " + tapName);
    }

    std::cout << "COM initialized for " << tapName << " with MAC " << macAddress << "\n";
}

// Initialize TAP device
bool COM::initializeTAP() {
    tapFd = open("/dev/net/tun", O_RDWR);
    if (tapFd < 0) {
        std::cerr << "Failed to open /dev/net/tun\n";
        return false;
    }

    struct ifreq ifr = {};
    strncpy(ifr.ifr_name, tapName.c_str(), IFNAMSIZ);
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (ioctl(tapFd, TUNSETIFF, &ifr) < 0) {
        std::cerr << "Failed to create TAP device " << tapName << "\n";
        close(tapFd);
        return false;
    }

    // Bring the interface up
    std::string cmd = "ip link set " + tapName + " up";
    if (std::system(cmd.c_str()) != 0) {
        std::cerr << "Failed to bring TAP device up: " << tapName << "\n";
        close(tapFd);
        return false;
    }

    std::cout << "Created TAP device: " << tapName << std::endl;
    return true;
}

// Initialize libnet
bool COM::initializeLibnet() {
    lnet = libnet_init(LIBNET_LINK, tapName.c_str(), errbuf);
    if (!lnet) {
        std::cerr << "Libnet init failed for " << tapName << ": " << errbuf << "\n";
        return false;
    }
    std::cout << "Libnet initialized for " << tapName << "\n";
    return true;
}

// Initialize pcap
bool COM::initializePcap() {
    pcapHandle = pcap_open_live(tapName.c_str(), BUFSIZ, 1, 1000, errbuf);
    if (!pcapHandle) {
        std::cerr << "Failed to initialize pcap: " << errbuf << "\n";
        return false;
    }
    std::cout << "Pcap initialized for " << tapName << "\n";
    return true;
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

