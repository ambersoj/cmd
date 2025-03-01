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
    explicit Cmd(std::vector<std::shared_ptr<COM>>& coms);
    void addCommand(const std::string& name, std::function<void(const std::vector<std::string>&)> func);
    void run();
    void executeCommand(const std::string& input);
    std::vector<std::shared_ptr<COM>> getComList() { return comList; }

private:
    std::unordered_map<std::string, Command> commands;
    std::vector<std::shared_ptr<COM>> comList;

};

#endif // CMD_HPP
