#include "./DCE.hpp"
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
#include "./Observer.hpp"
#include <pcap.h>
#include <libnet.h>
#include <conio.h/conio.h>

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
        std::string tap1 = createTapDevice("tap0");
        std::string tap2 = createTapDevice("tap1");

        // Initialize libnet
        char errbuf[LIBNET_ERRBUF_SIZE];
        libnet_t *lnet1 = libnet_init(LIBNET_RAW4, tap1.c_str(), errbuf);
        libnet_t *lnet2 = libnet_init(LIBNET_RAW4, tap2.c_str(), errbuf);
        if (!lnet1 || !lnet2) {
            throw std::runtime_error("Failed to initialize libnet: " + std::string(errbuf));
        }

        // Create DCE instances for each TAP device
        auto dce1 = std::make_shared<DCE>(tap1);
        auto dce2 = std::make_shared<DCE>(tap2);

        // Create Observers for each DCE instance
        auto observer1 = std::make_shared<RxObserver>();
        auto observer2 = std::make_shared<RxObserver>();

        // Attach Observers
        dce1->attach(observer1);
        dce2->attach(observer2);

        std::vector<uint8_t> packet1 = observer1->getNextPacket();

        std::cout << "DCE is running. Press Enter to exit..." << std::endl;

        // Start packet capture
        dce1->startCapture();
        dce2->startCapture();

        bool running = true;
        int i = 100;
        while(--i)
        {
            sleep(1);
            std::vector<uint8_t> packet1 = observer1->getNextPacket();

            if (!packet1.empty()) {
                std::cout << "Processing packet from TAP 1, size: " << packet1.size() << " bytes" << std::endl;
            }

            std::vector<uint8_t> packet2 = observer2->getNextPacket();
            if (!packet2.empty()) {
                std::cout << "Processing packet from TAP 2, size: " << packet2.size() << " bytes" << std::endl;
            }

        }

        // Stop packet capture
        dce1->stopCapture();
        dce2->stopCapture();

        // Cleanup libnet
        libnet_destroy(lnet1);
        libnet_destroy(lnet2);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
