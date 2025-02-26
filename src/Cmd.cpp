#include "Cmd.hpp"
#include <iostream>

void Cmd::addCommand(const std::string& name, Command::CommandFunction func) {
    auto [it, inserted] = commands.try_emplace(name, name, std::move(func));
    if (!inserted) {
        std::cerr << "Command '" << name << "' already exists.\n";
    }
}

void Cmd::executeCommand(const std::string& name) const {
    if (auto it = commands.find(name); it != commands.end()) {
        it->second.execute();
    } else {
        std::cerr << "Command not found: " << name << std::endl;
    }
}