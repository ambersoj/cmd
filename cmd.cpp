#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class UDPChannel {
public:
    UDPChannel() = default;
    UDPChannel(const std::string& ip, int port) : ip(ip), port(port) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            exit(1);
        }
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Bind failed");
            exit(1);
        }
        std::cout << "UDPChannel bound to " << ip << ":" << port << " with sockfd " << sockfd << std::endl;
    }

    // Delete copy constructor and assignment operator to prevent copying
    UDPChannel(const UDPChannel&) = delete;
    UDPChannel& operator=(const UDPChannel&) = delete;

    // Allow move semantics
    UDPChannel(UDPChannel&& other) noexcept
        : sockfd(other.sockfd), ip(std::move(other.ip)), port(other.port) {
        other.sockfd = -1;
    }

    UDPChannel& operator=(UDPChannel&& other) noexcept {
        if (this != &other) {
            close(sockfd);
            sockfd = other.sockfd;
            ip = std::move(other.ip);
            port = other.port;
            other.sockfd = -1;
        }
        return *this;
    }

    void send(const std::string& dst_ip, int dst_port, const std::string& message) {
        if (sockfd < 0) {
            std::cerr << "Socket not initialized properly!" << std::endl;
            return;
        }
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(dst_ip.c_str());
        dest.sin_port = htons(dst_port);
        std::cout << "Sending to " << dst_ip << ":" << dst_port << " via socket " << sockfd << std::endl;
        ssize_t bytes_sent = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&dest, sizeof(dest));
        if (bytes_sent < 0) {
            perror("sendto failed");
        }
    }

    std::string recv() {
        char buffer[1024] = {0};
        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
        if (len > 0) {
            buffer[len] = '\0';
            return std::string(buffer);
        }
        return "";
    }

    ~UDPChannel() {
        if (sockfd >= 0) {
            close(sockfd);
        }
    }

private:
    int sockfd;
    std::string ip;
    int port;
};

class Command {
public:
    virtual void execute() = 0;
    virtual ~Command() = default;
};

class SendCommand : public Command {
public:
    SendCommand(UDPChannel& channel, const std::string& dst_ip, int dst_port, const std::string& message)
        : channel(channel), dst_ip(dst_ip), dst_port(dst_port), message(message) {}
    
    void execute() override { channel.send(dst_ip, dst_port, message); }
    
private:
    UDPChannel& channel;
    std::string dst_ip;
    int dst_port;
    std::string message;
};

class RecvCommand : public Command {
public:
    RecvCommand(UDPChannel& channel) : channel(channel) {}
    
    void execute() override {
        std::string msg = channel.recv();
        if (!msg.empty()) std::cout << "Received: " << msg << std::endl;
        else std::cout << "No data available" << std::endl;
    }
    
private:
    UDPChannel& channel;
};

void run(std::unordered_map<int, UDPChannel>& channels) {
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "exit") break;
        
        std::vector<std::string> tokens;
        std::istringstream iss(line);
        std::string token;
        while (iss >> token) tokens.push_back(token);
        
        if (tokens.empty()) continue;
        if (tokens[0] == "send" && tokens.size() >= 5) {
            int ch = std::stoi(tokens[1]);
            if (channels.find(ch) != channels.end()) {
                int dst_port = std::stoi(tokens[3]);
                std::string message = line.substr(line.find(tokens[4])); // Capture full message including spaces
                SendCommand cmd(channels[ch], tokens[2], dst_port, message);
                cmd.execute();
            } else {
                std::cerr << "Invalid channel: " << ch << std::endl;
            }
        } else if (tokens[0] == "recv" && tokens.size() == 2) {
            int ch = std::stoi(tokens[1]);
            if (channels.find(ch) != channels.end()) {
                RecvCommand cmd(channels[ch]);
                cmd.execute();
            } else {
                std::cerr << "Invalid channel: " << ch << std::endl;
            }
        } else {
            std::cout << "Invalid command" << std::endl;
        }
    }
}

int main() {
    std::unordered_map<int, UDPChannel> channels;
    for (int i = 0; i < 6; ++i) {
        channels.emplace(i, UDPChannel("127.0.0.1", 5000 + i));
    }
    run(channels);
    return 0;
}
