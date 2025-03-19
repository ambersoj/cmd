#ifndef HUD_HPP
#define HUD_HPP

// Hud.hpp
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <jsoncpp/json/json.h>
#include <queue>
#include <mutex>
#include <unordered_map>
#include "CommandInvoker.hpp"
#include "RecvCommand.hpp"
#include "UDPChannel.hpp"

class Hud {
private:
    std::thread hudThread;
    bool spinner_on;
    std::unordered_map<int, UDPChannel>* channels;  // Store a pointer instead of copying
    CommandInvoker* invoker;  // Store a pointer instead of copying
    
public:
    Hud();
    std::string receive_msg_from_cnl();
    void draw();
    void run();
    void start(std::unordered_map<int, UDPChannel>& channels, CommandInvoker& invoker);
    void stop();
};

#endif