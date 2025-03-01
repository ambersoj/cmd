#include <libnet.h>
#include <pcap.h>
#include <iostream>
#include <cstring>

void sendPing(libnet_t* lnet, const uint8_t* srcMac, const uint8_t* dstMac) {
    uint8_t payload[64] = "ICMP PING TEST"; // Just some data
    uint16_t payload_size = strlen((char*)payload);

    // Construct ICMP Echo Request (Type 8, Code 0)
    libnet_ptag_t icmpTag = libnet_build_icmpv4_echo(
        8, 0, 0, 1234, 1,  // Type, Code, Checksum, ID, Sequence Number
        payload, payload_size,
        lnet, 0
    );

    if (icmpTag == -1) {
        std::cerr << "Error building ICMP packet: " << libnet_geterror(lnet) << std::endl;
        return;
    }

    uint32_t src_ip = libnet_name2addr4(lnet, (char*)"192.168.200.1", LIBNET_DONT_RESOLVE);
    uint32_t dst_ip = libnet_name2addr4(lnet, (char*)"192.168.200.2", LIBNET_DONT_RESOLVE);

    // Construct IPv4 Header
    libnet_ptag_t ipv4Tag = libnet_build_ipv4(
        LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload_size,  // Total length
        0, 0, 0, 64, IPPROTO_ICMP,
        0,  // Checksum (0 means libnet calculates it)
        src_ip,
        dst_ip,
        NULL, 0,
        lnet, 0
    );

    if (ipv4Tag == -1) {
        std::cerr << "Error building IP packet: " << libnet_geterror(lnet) << std::endl;
        return;
    }

    // Construct Ethernet Frame
    libnet_ptag_t ethernetTag = libnet_build_ethernet(
        dstMac, srcMac,
        ETHERTYPE_IP, NULL, 0,
        lnet, 0
    );

    if (ethernetTag == -1) {
        std::cerr << "Error building Ethernet packet: " << libnet_geterror(lnet) << std::endl;
        return;
    }

    // Send packet
    int bytesWritten = libnet_write(lnet);
    if (bytesWritten == -1) {
        std::cerr << "Error sending packet: " << libnet_geterror(lnet) << std::endl;
    } else {
        std::cout << "Sent " << bytesWritten << " bytes" << std::endl;
    }
}

int main() {
    char errbuf[LIBNET_ERRBUF_SIZE];
    libnet_t* lnet = libnet_init(LIBNET_LINK, "hud0", errbuf);
    if (!lnet) {
        std::cerr << "libnet_init() failed: " << errbuf << std::endl;
        return 1;
    }

    uint8_t dstMac[6] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 };
    uint8_t srcMac[6] = { 0x02, 0x00, 0x00, 0x00, 0x01, 0x01 };

    sendPing(lnet, srcMac, dstMac);

    libnet_destroy(lnet);
    return 0;
}
