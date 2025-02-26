#include "Cmd.hpp"
#include <iostream>
#include <cassert>

int main() {
    Cmd testCommand("another_test");
    assert(testCommand.getName() == "another_test");
    std::cout << "test_command passed." << std::endl;
    return 0;
}
