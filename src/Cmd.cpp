#include "Cmd.hpp"
#include "COM.hpp"
#include <libnet.h>
#include <iostream>
#include <sstream>

std::vector<std::string> parseCommand(const std::string& input) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string token;
    bool inQuotes = false;
    std::string quotedString;

    while (std::getline(iss, token, ' ')) {
        if (!token.empty()) {
            if (token.front() == '"' && !inQuotes) {
                inQuotes = true;
                quotedString = token.substr(1);
            } else if (token.back() == '"' && inQuotes) {
                inQuotes = false;
                quotedString += " " + token.substr(0, token.length() - 1);
                args.push_back(quotedString);
            } else if (inQuotes) {
                quotedString += " " + token;
            } else {
                args.push_back(token);
            }
        }
    }
    return args;
}

void parseMacAddress(const std::string& macStr, std::array<uint8_t, 6>& dstMac) {
    std::stringstream ss(macStr);
    std::string byte;
    int i = 0;

    while (std::getline(ss, byte, ':')) {
        if (i >= 6) {
            throw std::runtime_error("Invalid MAC address format.");
        }
        dstMac[i++] = static_cast<uint8_t>(std::stoi(byte, nullptr, 16));
    }

    if (i != 6) {
        throw std::runtime_error("Incomplete MAC address.");
    }
}

Cmd::Cmd(std::vector<std::shared_ptr<COM>>& coms) : comList(coms) {

    std::cout << "Cmd initialized with existing COM instances.\n";

    // Register built-in commands
    addCommand("send", [this](const std::vector<std::string>& args) {
        if (args.size() < 4) {  // 6 bytes src MAC, 6 bytes dst MAC, at least 1-byte payload
            std::cerr << "Usage: send <dce_index> <dst_mac> <src_mac> <hex_data...>" << std::endl;
            return;
        }

        int dceIndex = std::stoi(args[0]);
        if (dceIndex < 0 || dceIndex >= comList.size()) {
            std::cerr << "Invalid DCE index." << std::endl;
            return;
        }

        std::array<uint8_t, 6> dstMac;
        std::array<uint8_t, 6> srcMac;

        // Parse destination MAC
        parseMacAddress(args[1], dstMac);

        // Parse source MAC
        parseMacAddress(args[2], srcMac);

        std::vector<uint8_t> payload;
        std::stringstream ss(args[3]);
        std::string byte;

        while (std::getline(ss, byte, ':')) {
            payload.push_back(static_cast<uint8_t>(std::stoi(byte, nullptr, 16)));
        }

        // Construct the EthernetFrame correctly
        EthernetFrame frame(srcMac, dstMac, payload);

        // Now call transmitFrame() with the correct type
//        comList[dceIndex]->transmitFrame(frame);
        comList[dceIndex]->sendPing(comList[0]);
        
    });

    addCommand("recv", [this](const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: recv <dce_index>" << std::endl;
            return;
        }
        int dceIndex = std::stoi(args[0]);
        if (dceIndex < 0 || dceIndex >= comList.size()) {
            std::cerr << "Invalid DCE index." << std::endl;
            return;
        }
        std::vector<uint8_t> packet = comList[dceIndex]->getNextPacket();
        std::cout << "Received packet: ";
        for (uint8_t byte : packet) {
            printf("%02X ", byte);
        }
        std::cout << std::endl;
    });
}

void Cmd::addCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> func) {
    commands.emplace(name, Command(name, func));
}

void Cmd::executeCommand(const std::string& input) {
    std::vector<std::string> args = parseCommand(input);

    if (args.empty()) {
        std::cerr << "Error: Empty command string" << std::endl;
        return;
    }

    std::string commandName = args[0];  // First argument is the command name
    auto it = commands.find(commandName);
    if (it != commands.end()) {
        args.erase(args.begin());  // Remove command name from args before passing
        it->second.execute(args);
    } else {
        std::cerr << "Command not found: " << commandName << std::endl;
    }
}

void Cmd::run() {
    std::cout << "Cmd loop running... Type 'exit' to quit." << std::endl;
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (input == "exit") break;
        executeCommand(input);
    }
}
