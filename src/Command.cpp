#include "Command.hpp"

Command::Command(const std::string& name, CommandFunction func)
    : name(name), func(func) {}

void Command::execute(const std::vector<std::string>& args) const {
    func(args);
}

const std::string& Command::getName() const {
    return name;
}
