#include "COM.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sstream>

COM::COM(const std::string& tapName, const std::string& macAddress) : tapName(tapName), macAddress(macAddress) {
    if (!initializeTAP()) {
        throw std::runtime_error("Failed to initialize TAP device " + tapName);
    }
    if (!initializeLibnet()) {
        throw std::runtime_error("Failed to initialize libnet for " + tapName);
    }
    if (!initializePcap()) {
        throw std::runtime_error("Failed to initialize pcap for " + tapName);
    }
    std::cout << "COM initialized for " << tapName << " with MAC " << macAddress << "\n";
}

bool COM::initializeTAP() {
    std::cout << "Creating TAP device " << tapName << "\n";
    // Simulate a failure case for debugging
    if (tapName.empty()) return false;
    return true;
}

bool COM::initializeLibnet() {
    std::cout << "Initializing libnet for " << tapName << "\n";
    return !tapName.empty(); // Return false if tapName is invalid
}

bool COM::initializePcap() {
    std::cout << "Initializing pcap for " << tapName << "\n";
    return !tapName.empty(); // Return false if tapName is invalid
}

void COM::transmitFrame(const std::string& dstMac, const std::string& data) {
    if (dstMac.empty() || data.empty()) {
        std::cerr << "Error: Invalid destination MAC or empty data\n";
        return;
    }
    std::cout << "Transmitting from " << macAddress << " to " << dstMac << " with data: " << data << "\n";
}

std::string COM::getNextPacket() {
    std::cout << "Receiving packet on " << tapName << "\n";
    return "Mocked packet data";
}

std::string COM::getMacAddress() const {
    return macAddress;
}
