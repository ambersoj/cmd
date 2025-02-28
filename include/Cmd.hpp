#ifndef CMD_HPP
#define CMD_HPP

#include "Command.hpp"
#include "COM.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <string>

class Cmd {
public:
    explicit Cmd(std::vector<std::shared_ptr<COM>>& dces);
    void addCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> func);
    void run();
    void executeCommand(const std::string& input);


private:
    std::unordered_map<std::string, Command> commands;
    std::vector<std::shared_ptr<COM>> dceList;
};

#endif // CMD_HPP
