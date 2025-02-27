#include "Command.hpp"
#include <iostream>

Command::Command(std::string name, CommandFunction func)
    : name(std::move(name)), func(std::move(func)) {
    if (!this->func) {
        throw std::invalid_argument("Command function cannot be null.");
    }
}

void Command::execute() const {
    func(std::vector<std::string>{});  // Provide an empty vector
}

const std::string Command::getName() const {
    return name;
}
