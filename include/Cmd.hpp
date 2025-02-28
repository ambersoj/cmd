#ifndef CMD_HPP
#define CMD_HPP

#include "Command.hpp"
#include "DCE.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <string>

class Cmd {
public:
    explicit Cmd(std::vector<std::shared_ptr<DCE>>& dces);
    void addCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> func);
    void run();

private:
    std::unordered_map<std::string, Command> commands;
    std::vector<std::shared_ptr<DCE>> dceList;
};

#endif // CMD_HPP
