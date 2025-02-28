#include "Cmd.hpp"
#include <iostream>

Cmd::Cmd(std::vector<std::shared_ptr<COM>>& coms) : dceList(coms) {}

void Cmd::addCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> func) {
    commands.emplace(name, Command(name, func));
}

void Cmd::run() {
    std::cout << "Cmd loop running..." << std::endl;
    // Implement command execution loop here
}
