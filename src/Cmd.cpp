#include "Cmd.hpp"
#include <iostream>

Cmd::Cmd(const std::string& name) : name(name) {}

std::string Cmd::getName() const {
    return name;
}

void Cmd::execute() const {
    std::cout << "Executing command: " << name << std::endl;
}
