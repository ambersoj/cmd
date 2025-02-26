#ifndef CMD_HPP
#define CMD_HPP

#include <string>

class Cmd {
public:
    Cmd(const std::string& name);
    std::string getName() const;
    void execute() const;
private:
    std::string name;
};

#endif // CMD_HPP
