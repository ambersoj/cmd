#include <sstream>
#include <vector>
#include <string>
#include <iostream>

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

int main() {
    std::string input = R"(dce_transmit 11:22:33:44:55:66 aa:bb:cc:dd:ee:ff "Hello World!!")";
    std::vector<std::string> args = parseCommand(input);
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << "args[" << i << "] = " << args[i] << std::endl;
    }
}
