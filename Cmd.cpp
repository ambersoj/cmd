#include <iostream>
#include <thread>
#include <atomic>
#include "Cmd.hpp"

std::atomic<bool> running(true);

void Cmd::run() {
    std::cout << "Cmd is running in a separate thread." << std::endl;
    while (running.load()) {
        // Simulating work - could be listening for commands, processing messages, etc.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Cmd thread is stopping." << std::endl;
}

void Cmd::start() {
    cmdThread = std::thread(&Cmd::run, this);
}

void Cmd::stop() {
    running.store(false);
    if (cmdThread.joinable()) {
        cmdThread.join();  // Ensures the thread exits cleanly
    }
}
