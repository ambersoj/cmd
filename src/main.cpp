#include "COM.hpp"
#include "Cmd.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    try {
        // Initialize DCE instances
        std::vector<std::shared_ptr<COM>> coms;
        coms.push_back(std::make_shared<COM>("tap0", "00:11:22:33:44:00"));
        coms.push_back(std::make_shared<COM>("tap1", "00:11:22:33:44:01"));
        coms.push_back(std::make_shared<COM>("tap2", "00:11:22:33:44:02"));
        coms.push_back(std::make_shared<COM>("tap3", "00:11:22:33:44:03"));
        coms.push_back(std::make_shared<COM>("tap4", "00:11:22:33:44:04"));
        coms.push_back(std::make_shared<COM>("tap5", "00:11:22:33:44:05"));

        // Initialize command interface
        Cmd cmd(coms);
        
        // Run the command loop
        cmd.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
