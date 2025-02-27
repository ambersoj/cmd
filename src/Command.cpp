#include "Command.hpp"
#include <iostream>

Command::Command(std::string name, CommandFunction func)
    : name(std::move(name)), func(std::move(func)) {
    if (!this->func) {
        throw std::invalid_argument("Command function cannot be null.");
    }
}

void Command::execute(const std::vector<std::string>& args) const {
    func(args);
}

const std::string Command::getName() const {
    return name;
}
