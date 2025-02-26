#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

class Command {
protected:
    std::string name;
public:
    Command(const std::string& name);
    virtual ~Command() = default;
    virtual void run() = 0; // Pure virtual function
    std::string getName() const { return name; } // Getter for the name
};

#endif // COMMAND_HPP
