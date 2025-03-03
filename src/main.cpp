#include "COM.hpp"
#include "Cmd.hpp"
#include <iostream>
#include <memory>
#include <vector>

void enterNamespace(const std::string& nsName) {
    std::string nsPath = "/var/run/netns/" + nsName;  // Path to the network namespace
    int fd = open(nsPath.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("Failed to open network namespace");
        exit(1);
    }
    if (setns(fd, CLONE_NEWNET) == -1) {
        perror("Failed to enter network namespace");
        close(fd);
        exit(1);
    }
    close(fd);
}

int main() {
    try {
        enterNamespace("mynetns");

        // Initialize COM instances for CMD
        std::vector<std::shared_ptr<COM>> coms;
        coms.push_back(std::make_shared<COM>("cmd0", "02:00:00:00:00:01"));
/*        coms.push_back(std::make_shared<COM>("cmd1", "02:00:00:00:00:02"));
        coms.push_back(std::make_shared<COM>("cmd2", "02:00:00:00:00:03"));
        coms.push_back(std::make_shared<COM>("cmd3", "02:00:00:00:00:04"));
        coms.push_back(std::make_shared<COM>("cmd4", "02:00:00:00:00:05"));
        coms.push_back(std::make_shared<COM>("cmd5", "02:00:00:00:00:06"));
*/
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
