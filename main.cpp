#include "Cmd.hpp"
#include "Command.hpp"
#include <iostream>

void testCommandFunction() {
    std::cout << "Test command executed successfully!" << std::endl;
}

int main() {
    Cmd myCmd;
    myCmd.addCommand("Test command", testCommandFunction);

    std::cout << "Executing stored command..." << std::endl;
    myCmd.executeCommand("Test command");

    return 0;
}
