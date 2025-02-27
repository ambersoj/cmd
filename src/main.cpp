#include "DCE.hpp"
#include "Command.hpp"
#include "Cmd.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>
#include <memory>
#include "RxObserver.hpp"
#include <pcap.h>
#include <libnet.h>
#include "EthernetFrame.hpp"

// Function to create a TAP device
std::string createTapDevice(const std::string& tapName) {
    int tap_fd = open("/dev/net/tun", O_RDWR);
    if (tap_fd < 0) {
        throw std::runtime_error("Failed to open /dev/net/tun");
    }

    struct ifreq ifr = {};
    std::strncpy(ifr.ifr_name, tapName.c_str(), IFNAMSIZ);
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (ioctl(tap_fd, TUNSETIFF, &ifr) < 0) {
        close(tap_fd);
        throw std::runtime_error("Failed to create TAP device " + tapName);
    }

    // Bring the interface up
    std::string cmd = "ip link set " + tapName + " up";
    if (std::system(cmd.c_str()) != 0) {
        close(tap_fd);
        throw std::runtime_error("Failed to bring TAP device up");
    }

    std::cout << "Created TAP device: " << tapName << std::endl;
    return tapName;
}

int main() {
    try {
        std::string tap0 = createTapDevice("tap0");
        std::string tap1 = createTapDevice("tap1");

        // Initialize libnet
        char errbuf[LIBNET_ERRBUF_SIZE];
        libnet_t *lnet0 = libnet_init(LIBNET_LINK, tap0.c_str(), errbuf);
        libnet_t *lnet1 = libnet_init(LIBNET_LINK, tap1.c_str(), errbuf);
        if (!lnet0 || !lnet1) {
            throw std::runtime_error("Failed to initialize libnet: " + std::string(errbuf));
        }

        // Create DCE instances for each TAP device
        auto dce0 = std::make_shared<DCE>(tap0);
        auto dce1 = std::make_shared<DCE>(tap1);

        // Create Observers for each DCE instance
        auto observer0 = std::make_shared<RxObserver>();
        auto observer1 = std::make_shared<RxObserver>();

        // Attach Observers
        dce0->attach(observer0);
        dce1->attach(observer1);

        // Start packet capture
        dce0->startCapture();
        dce1->startCapture();

        // Command execution setup
        Cmd commandProcessor;
        commandProcessor.addCommand("dce_transmit", [&dce0](const std::vector<std::string>& args) {
            if (args.size() < 3) {
                std::cerr << "Error: Insufficient arguments for transmission. Usage: dce_transmit <srcMac> <dstMac> <data>" << std::endl;
                return;
            }
            
            std::array<uint8_t, 6> srcMac;
            std::array<uint8_t, 6> dstMac;
            std::vector<uint8_t> payload(args[2].begin(), args[2].end());
            
            // Convert MAC addresses from string to byte array
            sscanf(args[0].c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &srcMac[0], &srcMac[1], &srcMac[2], &srcMac[3], &srcMac[4], &srcMac[5]);
            sscanf(args[1].c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dstMac[0], &dstMac[1], &dstMac[2], &dstMac[3], &dstMac[4], &dstMac[5]);
            
            EthernetFrame frame(srcMac, dstMac, payload);
            dce0->transmitFrame(frame);
            std::cout << "Transmitted data: " << args[2] << std::endl;
        });

        std::cout << "DCE is running.\nEnter command: " << std::endl;
        std::string input;
        while (std::getline(std::cin, input)) {
            if(input == "exit")
            {
                break;
            }
            commandProcessor.executeCommand(input);
            std::cout << "Enter command: " << std::endl;
        }

        // Stop packet capture
        dce0->stopCapture();
        dce1->stopCapture();

        // Cleanup libnet
        libnet_destroy(lnet0);
        libnet_destroy(lnet1);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
