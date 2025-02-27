#include "Cmd.hpp"
#include <iostream>
#include <sstream>

void Cmd::addCommand(const std::string& name, Command::CommandFunction func) {
    auto [it, inserted] = commands.try_emplace(name, name, std::move(func));
    if (!inserted) {
        std::cerr << "Command '" << name << "' already exists.\n";
    }
}

std::vector<std::string> parseCommand(const std::string& input) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string token;
    bool inQuotes = false;
    std::string quotedString;

    while (std::getline(iss, token, ' ')) {  // Read tokens by space
        if (!token.empty()) {
            if (token.front() == '"' && !inQuotes) {
                inQuotes = true;
                quotedString = token.substr(1);  // Remove leading quote
            } else if (token.back() == '"' && inQuotes) {
                inQuotes = false;
                quotedString += " " + token.substr(0, token.length() - 1);  // Remove trailing quote
                args.push_back(quotedString);
            } else if (inQuotes) {
                quotedString += " " + token;
            } else {
                args.push_back(token);
            }
        }
    }

    return args;
}

void Cmd::executeCommand(const std::string& name) const {
    std::vector<std::string> args = parseCommand(name);
    
    if (args.empty()) {
        std::cerr << "Error: Empty command string" << std::endl;
        return;
    }

    std::string commandName = args[0];  // First argument is the command name

    auto it = commands.find(commandName);
    if (it != commands.end()) {
        args.erase(args.begin());  // Remove command name from args before passing
        it->second.execute(args);
    } else {
        std::cerr << "Command not found: " << commandName << std::endl;
    }
}

