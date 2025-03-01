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

    startCapture();

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

void COM::sendPing(std::shared_ptr<COM> com) {
    libnet_t* lnet = com->getLibnetHandle();  // Get libnet handle from COM

    std::array<uint8_t, 6> srcMac = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
    std::array<uint8_t, 6> dstMac = {0x02, 0x00, 0x00, 0x00, 0x01, 0x01};

    uint32_t srcIP = libnet_name2addr4(lnet, (char*)"192.168.100.1", LIBNET_DONT_RESOLVE);
    uint32_t dstIP = libnet_name2addr4(lnet, (char*)"192.168.100.2", LIBNET_DONT_RESOLVE);

    libnet_clear_packet(lnet);

    // 🏗 Build ICMP Packet
    uint8_t payload[] = "ICMP PING TEST";
    uint32_t payloadSize = sizeof(payload) - 1;

    libnet_ptag_t icmpTag = libnet_build_icmpv4_echo(
        8, 0, 0, 1234, 1,  // Type, Code, Checksum, ID, Sequence
        payload, payloadSize,
        lnet, 0
    );
    if (icmpTag == -1) {
        throw std::runtime_error("ICMP build error: " + std::string(libnet_geterror(lnet)));
    }

    // 🏗 Build IP Header
    libnet_ptag_t ipTag = libnet_build_ipv4(
        LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payloadSize, 
        0, 12345, 0, 64, IPPROTO_ICMP, 0, srcIP, dstIP,
        NULL, 0, lnet, 0
    );
    if (ipTag == -1) {
        throw std::runtime_error("IP build error: " + std::string(libnet_geterror(lnet)));
    }

    // 🔥 Get raw packet data
    uint8_t* packetData = nullptr;
    uint32_t packetSize = 0;
    packetData = libnet_getpbuf(lnet, packetSize);
    if (!packetData) {
        throw std::runtime_error("Failed to get packet buffer: " + std::string(libnet_geterror(lnet)));
    }

    // 🏗 Wrap into EthernetFrame
    std::vector<uint8_t> ethPayload(packetData, packetData + packetSize);
    EthernetFrame frame(srcMac, dstMac, ethPayload);

    // 🚀 Transmit!
    com->transmitFrame(frame);
    std::cout << "ICMP Ping sent!" << std::endl;
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

