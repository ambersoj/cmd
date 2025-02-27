#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <unordered_map>
#include <functional>

class Command {
public:
    using CommandFunction = std::function<void(const std::vector<std::string>&)>;

    Command(std::string name, CommandFunction func);

    void execute() const;

    const std::string getName() const;

private:
    std::string name;
    CommandFunction func;
};
#endif