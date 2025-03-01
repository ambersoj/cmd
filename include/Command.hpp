#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <functional>
#include <string>
#include <vector>

using CommandFunction = std::function<void(const std::vector<std::string>&)>;

class Command {
public:
    Command(const std::string& name, CommandFunction func);
    void execute(const std::vector<std::string>& args) const;
    const std::string& getName() const;

private:
    std::string name;
    CommandFunction func;
};

#endif // COMMAND_HPP
