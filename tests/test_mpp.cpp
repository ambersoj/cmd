#include "Cmd.hpp"
#include "Command.hpp"
#include "EthernetFrame.hpp"
#include <cassert>
#include <iostream>

void test_Cmd_execute() {
    Cmd cmd;
    
    // Expected: Command executed: tx
    cmd.execute("tx");
    
    // Expected: Command executed: start capture
    cmd.execute("start capture");
    
    // Expected: Unknown command: badcommand
    cmd.execute("badcommand");
}

void test_Command_execute() {
    Command cmd("test");
    cmd.execute(); // Expected: Executing command: test
}

void test_EthernetFrame() {
    EthernetFrame frame;
    frame.setSrcMac({0xAA,0xBB,0xCC,0xDD,0xEE,0xFF});
    frame.setDstMac({0x11,0x22,0x33,0x44,0x55,0x66});
    frame.setPayload({'H','e','l','l','o'});
    
    std::string serialized = frame.serialize();
    EthernetFrame deserialized = EthernetFrame::deserialize(serialized);
    
    assert(deserialized.getSrcMac() == frame.getSrcMac());
    assert(deserialized.getDstMac() == frame.getDstMac());
    assert(deserialized.getPayload() == frame.getPayload());
    
    std::cout << "EthernetFrame serialization test passed!\n";
}

int main() {
    test_Cmd_execute();
    test_Command_execute();
    test_EthernetFrame();
    
    std::cout << "All tests passed successfully!\n";
    return 0;
}
