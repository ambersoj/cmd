#include "Cmd.hpp"
#include <iostream>

int main() {
    Cmd myCmd("test_command");
    std::cout << "Command name: " << myCmd.getName() << std::endl;
    myCmd.execute();
    return 0;
}