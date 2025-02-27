#ifndef CMD_HPP
#define CMD_HPP

#include "Command.hpp"
#include <unordered_map>
#include <string>

class Cmd {
public:
    void addCommand(const std::string& name, Command::CommandFunction func);
    void executeCommand(const std::string& name) const;

private:
    std::unordered_map<std::string, Command> commands;
};
#endif
