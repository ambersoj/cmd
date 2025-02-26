/*
    cmd.cpp - Command Parsing and TAP Initialization
    - Parses space-delimited commands into JSON
    - Initializes TAPs with libnet and pcap
    - Manages DCEs using Observer pattern
*/

#include <iostream>
#include <sstream>
#include <string>
#include "/usr/include/jsoncpp/json/json.h"  // JSON library
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>  
#include <pcap.h>
#include <libnet.h>

// Mutex for thread-safe output
std::mutex output_mutex;

// Function to parse commands into JSON format
std::string parseCommandToJson(const std::string& command) {
    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    Json::Value jsonCommand;
    jsonCommand["command"] = tokens[0];
    Json::Value args(Json::arrayValue);
    for (size_t i = 1; i < tokens.size(); ++i) {
        args.append(tokens[i]);
    }
    jsonCommand["args"] = args;
    
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, jsonCommand);
}

// TAP Initialization using system calls
void initializeTAP(const std::string& tapName) {
    std::string command = "ip tuntap add dev " + tapName + " mode tap";
    system(command.c_str());
    command = "ip link set " + tapName + " up";
    system(command.c_str());
}

// DCE Class (Observer Pattern)
class DCE {
public:
    DCE(const std::string& name) : tapName(name), running(false) {}

    void start() {
        running = true;
        thread = std::thread(&DCE::run, this);
    }

    void stop() {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
    }

    ~DCE() {
        stop();
    }

private:
    std::string tapName;
    std::thread thread;
    bool running;

    void run() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "DCE " << tapName << " running..." << std::endl;
        }
    }
};

int main() {
    std::cout << "CMD Component Initialized" << std::endl;
    
    // Example TAP initialization
    initializeTAP("tap0");
    
    // Example DCE
    DCE dce1("tap0");
    dce1.start();
    
    // Command loop
    std::string userInput;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);
        if (userInput == "exit") break;
        
        std::string jsonCmd = parseCommandToJson(userInput);
        std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << "Parsed Command: " << jsonCmd << std::endl;
    }
    
    return 0;
}
