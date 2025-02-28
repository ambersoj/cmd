#include "DCE.hpp"
#include "Cmd.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    try {
        // Initialize DCE instances
        std::vector<std::shared_ptr<DCE>> dces;
        dces.push_back(std::make_shared<DCE>("tap0", "AA:BB:CC:DD:EE:01"));
        dces.push_back(std::make_shared<DCE>("tap1", "AA:BB:CC:DD:EE:02"));

        // Initialize command interface
        Cmd cmd(dces);
        
        // Run the command loop
        cmd.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
