#include "Cmd.hpp"
#include <iostream>
#include <cassert>

int main() {
    Cmd testCmd("unit_test");
    assert(testCmd.getName() == "unit_test");
    std::cout << "test_cmd passed." << std::endl;
    return 0;
}
