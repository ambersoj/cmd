#include "Cmd.hpp"
#include <iostream>
#include <cassert>

int main() {
    Cmd testFrame("frame_test");
    assert(testFrame.getName() == "frame_test");
    std::cout << "test_ethernetframe passed." << std::endl;
    return 0;
}