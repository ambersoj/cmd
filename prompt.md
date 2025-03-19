I think git is under control.  At least you know how to handle it, if it acts up more.

Today is way more fun.  I want to move towards specializing the components.  So for now I'd like for us to just talk.

One of the first things I'd like to figure out how to name all of the UDP channels.  When I was using MAC address, I had a tap-to-mac table.  Maybe we could make another one of them.  I had names like cmd_hud_dte, which would be one of the UDP channels used as a cmd DTE, and it's implied that the hud is the DCE and that of that UDP channel the cmd is the originating equipment and therefore the DTE and the DCE's job is to assist the cmd, in this hud-as-DCE context.  So our table would have to make some sense out of the IP addresses along with their ports.  Incidentally I've assigned port bases:

cmd: 5000
hud: 6000
cnl: 7000
net: 8000

I've tested them out a bit and so far those base addresses seem to work  just fine.  Would it make sense for us to create a namespace for mpp?

I'm happy with more or less hard coding the IP, ports and channel names.  But I'd like to do it in a neat way.  Would using #defines somehow be a way?  Should the channel names along with their IPs and ports be classes and created with constructors?  Or perhaps created kind of like the channels themselves, in a for loop?  Should we be using some sort of GoF creational pattern?  I'd like to hear your thoughts on these sorts of things too.

So I guess it's looking to me that moving forward involves figuring out how to meaningfully name the channels with regards to DTE/DCE relationships and then moving forward it's up to the user/programmer to use the channels properly, according to their name.  As you consider these current challenges, please be aware that the very small code base that we currently have is a delight.  This simplicity is very welcome.  As you think of alternatives I'd like you to give simplicity and lightness plenty of weight.

So, let's have fun, partner!!!

//////////////////////////////////////////////////////

## From Final Thoughts: ##

- Do you like the struct approach? Or do you prefer the #define macros for pure minimalism?
- Would you want to print/log channel names when they’re created to help with debugging?
- Do you think there will be runtime-generated channels (e.g., dynamic discovery), or will the predefined set always be enough?

- I like your struct idea a great deal.  Let's go with that.
  
- I think we'll just try to keep the simplicity at a high level and avoid the necessity for logging.  The character and ethos of mpp and an mpp user is that we expect compentency in the users and we won't really think in terms of errors or bugs or mistakes and as long as we keep this code base small, hardcode where appropriate and layer our program a user/programmer should have plenty of debugging help from other means, like the debugger in the IDE, or cerr/cout messages.

- Nah, for now we'll just have 4 components wired up up in a static way.

I was thinking for the names there will be something like cmd_hud_dte and the software would use this as a tx channel from the cmd_hud_dte.  The cmd_hud_dte in the cmd component would go to the hud_cmd_dce in the hud component.  So in this example it looks like this:

cmd port base: 5000
hud port base: 6000
IP address: 127.0.0.1

In the cmd component there would be cmd_hud_dtx at 127.0.0.1:5000 and it would go to 127.0.0.1:6001 and cmd_hud_dce at 127.0.0.1:5001 would go to 127.0.0.1:6000.  In the hud component there wold be hud_cmd_dte at 127.0.0.1:6000 and it would go to 127.0.0.1:5001 and hud_cmd_dce at 127.0.0.1:6001 and it would be connected to 127.0.0.1:5000.  I hope that's showing a pattern that we can use to make our IP/port/name table that we can then code into the program using your brilliant struct method.

I'm eager to hear your questions and comments and I'm grateful for your generosity with your knowledge and capabilities with design, coding and c++.

//////////////////////////////////////////

Uhm, wait, I have to think about this... I've got something wrong in my thinking, but that's what this discussion is for.

I should use 24 ports if I get it set up properly.  I'll try to type out an example component of hud, and what it should have to names and ports.  In our current state the IP is always the same, 127.0.0.1, so we just need names and ports.  Which brings up the point that probably we should figure out a simple way that we can set what we use for an IP address too.  For now though, since it's working, I thought I'd stay with 127.0.0.1 for now.

cmd_hud_dte
5000 - 6001
tx

cmd_hud_dce
5001 - 6000
rx

cmd_cnl_dte
5002 - 7001
tx

cmd_cnl_dce
5003 - 7000
rx

cmd_net_dte
5004 - 8001
tx

cmd_net_dce
5005 - 8000
rx

At this point it kind of takes discipline for the user/programmer to respect the conventions of the DTE/DCE relationship.  So for instance in practice probably the cmd will use the send command with it's port 5000, the dte and use the recv command with it's port 5001.  However, there is an rx direction for port 5000 in the cmd and a tx direction for the port 5001 in cmd but the use/programmer should keep in mind what the purpose for these are and use them only for completing the transfer for the DTE.

Can you make our 4 port to name tables for each of the components based on the cmd example above?

Before when I mentioned namespace, I meant a network namespace... but let's not worry about any of that for now.  Let's just do a good job of our table and structs.

We're really narrowing in on these requirements... we should be coding soon, partner!

////////////////////////////////////////

## From Next Steps & Questions for You ##

1.)
Do we need a lookup function (e.g., findChannel("cmd_hud_dte")), or is this small enough that direct iteration over udpChannels is fine?

2.)
Is this naming scheme locked in, or do you want to refine it before we move forward?

3.)
Are you happy with hardcoding 127.0.0.1, or should we allow for configurable IPs at some later stage?

1.)
I don't foresee a need for any sort of lookup function nor iteration.  In practice each message on each channel will be specialized for it's own particular purpose at the time and not really like in a gang or group.

2.)
The naming scheme isn't locked in, if you have a better idea.

3.)
It would probably be nice to have the IP configurable quite easily.  Not dynamically, I don't think.  Not yet.  But easy to change in just one place.  The net component is going to use libnet to reach the outside world and so when the mpp starts doing things like that it would be nice to be able to set IPs in the 192.168.x.x range.

//////////////////////////////////////////////////

I see in your code snippet:

Refined Struct with Configurable IP

        {"cmd_hud_tx", BASE_IP, CMD_BASE, BASE_IP, HUD_BASE + 1},
        {"cmd_hud_rx", BASE_IP, CMD_BASE + 1, BASE_IP, HUD_BASE},

        {"hud_cnl_tx", BASE_IP, HUD_BASE, BASE_IP, CNL_BASE + 1},
        {"hud_cnl_rx", BASE_IP, HUD_BASE + 1, BASE_IP, CNL_BASE},

        {"cnl_net_tx", BASE_IP, CNL_BASE, BASE_IP, NET_BASE + 1},
        {"cnl_net_rx", BASE_IP, CNL_BASE + 1, BASE_IP, NET_BASE}

I'm not sure if this was just example data but I'd like to give you an idea of how I'd like it to be not neccessarily with regards to iplementation, but the names and ports.  Below is the way it would be for the cmd, unless I made a typo... but this should give you a really good idea:

        {"cmd_hud_dte", BASE_IP, CMD_BASE + 0, BASE_IP, HUD_BASE + 1},
        {"cmd_hud_dce", BASE_IP, CMD_BASE + 1, BASE_IP, HUD_BASE + 0},

        {"cmd_cnl_dte", BASE_IP, CMD_BASE + 2, BASE_IP, CNL_BASE + 1},
        {"cmd_cnl_dce", BASE_IP, CMD_BASE + 3, BASE_IP, CNL_BASE + 0},

        {"cmd_net_dte", BASE_IP, CMD_BASE + 4, BASE_IP, NET_BASE + 1},
        {"cmd_net_dce", BASE_IP, CMD_BASE + 5, BASE_IP, NET_BASE + 0}

In this scheme and convention this would be for hud for example:

        {"hud_cmd_dte", BASE_IP, HUD_BASE + 0, BASE_IP, CMD_BASE + 1},
        {"hud_cmd_dce", BASE_IP, HUD_BASE + 1, BASE_IP, CMD_BASE + 0},

        {"hud_cnl_dte", BASE_IP, HUD_BASE + 2, BASE_IP, CNL_BASE + 3},
        {"hud_cnl_dce", BASE_IP, HUD_BASE + 3, BASE_IP, CNL_BASE + 2},

        {"hud_net_dte", BASE_IP, HUD_BASE + 4, BASE_IP, NET_BASE + 3},
        {"hud_net_dce", BASE_IP, HUD_BASE + 5, BASE_IP, NET_BASE + 2}

For the cnl:

        {"cnl_cmd_dte", BASE_IP, CNL_BASE + 0, BASE_IP, CMD_BASE + 3},
        {"cnl_cmd_dce", BASE_IP, CNL_BASE + 1, BASE_IP, CMD_BASE + 2},

        {"cnl_hud_dte", BASE_IP, CNL_BASE + 2, BASE_IP, HUD_BASE + 3},
        {"cnl_hud_dce", BASE_IP, CNL_BASE + 3, BASE_IP, HUD_BASE + 2},

        {"cnl_net_dte", BASE_IP, CNL_BASE + 4, BASE_IP, NET_BASE + 5},
        {"cnl_net_dce", BASE_IP, CNL_BASE + 5, BASE_IP, NET_BASE + 4}


For the net:

        {"net_cmd_dte", BASE_IP, NET_BASE + 0, BASE_IP, CMD_BASE + 5},
        {"net_cmd_dce", BASE_IP, NET_BASE + 1, BASE_IP, CMD_BASE + 4},

        {"net_hud_dte", BASE_IP, NET_BASE + 2, BASE_IP, HUD_BASE + 5},
        {"net_hud_dce", BASE_IP, NET_BASE + 3, BASE_IP, HUD_BASE + 4},

        {"net_cnl_dte", BASE_IP, NET_BASE + 4, BASE_IP, CNL_BASE + 5},
        {"net_cnl_dce", BASE_IP, NET_BASE + 5, BASE_IP, CNL_BASE + 4}

Does that look about right and make sense, partner?  I have a purpose for the DTE/DCE designations so I'm keeping them.

Is it necessary to have the namespace?  Earlier when I mentioned namespace I was refering to a network namespace, but let's not concern ourselves with any namespace things for now, unless you see a really good reason to.

We're getting very close.  Any further questions or comments?

//////////////////////////////////////

That code won't be useful for me.  There needs to be separate code for each component.  First let's do the cmd.  I'll send you the cmd code and I'd like you to refactor it implementing the name, IP and port assignments that we've agreed upon.

// cmd.cpp

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <cstdlib>

#define PORT_BASE 5000

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

    UDPChannel(const UDPChannel&) = delete;
    UDPChannel& operator=(const UDPChannel&) = delete;

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
        else std::cout << "> No data available" << std::endl;
    }
    
private:
    UDPChannel& channel;
};

class CommandInvoker {
public:
    void addCommand(Command* cmd) {
        commandQueue.push(cmd);
    }
    
    void executeCommands() {
        while (!commandQueue.empty()) {
            Command* cmd = commandQueue.front();
            commandQueue.pop();
            cmd->execute();
            std::cout << "> ";
            delete cmd;
        }
    }
    
private:
    std::queue<Command*> commandQueue;
};

void run(std::unordered_map<int, UDPChannel>& channels, CommandInvoker& invoker) {
    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    std::cout << "> ";
    while (true) {
        std::cout.flush();
        invoker.executeCommands();
        if (poll(&pfd, 1, 100) > 0) {
            std::string line;
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
                    std::string message = line.substr(line.find(tokens[4]));
                    invoker.addCommand(new SendCommand(channels[ch], tokens[2], dst_port, message));
                } else {
                    std::cerr << "> Invalid channel: " << ch << std::endl;
                }
            } else if (tokens[0] == "recv" && tokens.size() == 2) {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end()) {
                    invoker.addCommand(new RecvCommand(channels[ch]));
                } else {
                    std::cerr << "> Invalid channel: " << ch << std::endl;
                }
            } else {
                std::cout << "> Invalid command" << std::endl;
            }
        }
    }
}

int main() {
    int base_port = PORT_BASE;
    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;
    for (int i = 0; i < 6; ++i) {
        channels.emplace(i, UDPChannel("127.0.0.1", base_port + i));
    }
    run(channels, invoker);
    return 0;
}

/////////////  END OF cmd.cpp CODE  ///////////////////////


So then the cmd program only sets these:

    {"cmd_hud_dte", BASE_IP, CMD_BASE + 0, BASE_IP, HUD_BASE + 1},
    {"cmd_hud_dce", BASE_IP, CMD_BASE + 1, BASE_IP, HUD_BASE + 0},

    {"cmd_cnl_dte", BASE_IP, CMD_BASE + 2, BASE_IP, CNL_BASE + 1},
    {"cmd_cnl_dce", BASE_IP, CMD_BASE + 3, BASE_IP, CNL_BASE + 0},

    {"cmd_net_dte", BASE_IP, CMD_BASE + 4, BASE_IP, NET_BASE + 1},
    {"cmd_net_dce", BASE_IP, CMD_BASE + 5, BASE_IP, NET_BASE + 0},

right partner?

We'll refactor the cmd now first and then after we get it done we'll move on to cnl and then net.

///////////////////////////////////////////////////

Howdy partner.  It looks like there's an issue with the '>' prompt and it thinks that 0 is an invalid channel.

About the '>' prompt issue, take a look at how I handle the '>' prompt in the cmd.cpp code that I sent you.  I don't know it would survive real software design purist scrutiny but I took out the '>' in the program command feedback/output and added a cout of '>' in the command invoker:

    void executeCommands() {
        while (!commandQueue.empty()) {
            Command* cmd = commandQueue.front();
            commandQueue.pop();
            cmd->execute();
            std::cout << "> ";
            delete cmd;
        }

Whether that's a good or proper way to get desired behavior I'm not sure, but it's better than how it's working now so you should consider copying it.

So right off the bat there are a couple problems for you to troubleshoot:

1) invalid channel
2) '>' prompt

////////////////////////////////////

First off, I don't know how I'm expected to enter commands anymore, please advise.  Below I've used command examples of from before these latest changes.  Hopefully they adequately demonstrate the '>' prompt issue.

The prompt issues persist.  In the debugger I see the cout "> " executed but there's something about the new way of doing things where the '>' characters just won't show... not when they're supposed, just later when they're not wanted anymore.  Replacing that bit of code I suggested you look at obviously wasn't the answer.  Here's some output.  Maybe it'll help you figure what might be the issue with the prompt and what you might be able to do to fix it:

send 0 127.0.0.1 6001 Hello hud, from cmd!!
> Invalid command or channel: 0
send 0 127.0.0.1 6001 Hello hud, from cmd!!
Invalid command or channel: 0

shold be something more like:

>send 0 127.0.0.1 6001 Hello hud, from cmd!!
Invalid command or channel: 0
>send 0 127.0.0.1 6001 Hello hud, from cmd!!
Invalid command or channel: 0

And on another front, the program execution with regards to the send and recv commands.  What would I enter to send the message "Hello hud, from cmd!!!" from the cmd to the hud, for instance.  If I expected a message from hud, what command would I give cmd to check for a message from hud?

//////////////////////////////////////////////////////////////////////

I get this error now.  Hopefully you'll have a quick and easy fix for it:

xecuting task: /usr/bin/g++ -g /usr/local/mpp/cmd/cmd.cpp -o /usr/local/mpp/cmd/cmd 

In file included from /usr/include/c++/13/bits/memory_resource.h:47,
                 from /usr/include/c++/13/string:58,
                 from /usr/include/c++/13/bits/locale_classes.h:40,
                 from /usr/include/c++/13/bits/ios_base.h:41,
                 from /usr/include/c++/13/ios:44,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
                 from /usr/local/mpp/cmd/cmd.cpp:1:
/usr/include/c++/13/tuple: In instantiation of ‘std::pair<_T1, _T2>::pair(std::tuple<_Args1 ...>&, std::tuple<_Args2 ...>&, std::_Index_tuple<_Indexes1 ...>, std::_Index_tuple<_Indexes2 ...>) [with _Args1 = {const std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&}; long unsigned int ..._Indexes1 = {0}; _Args2 = {}; long unsigned int ..._Indexes2 = {}; _T1 = const std::__cxx11::basic_string<char>; _T2 = UDPChannel]’:
/usr/include/c++/13/tuple:2257:63:   required from ‘std::pair<_T1, _T2>::pair(std::piecewise_construct_t, std::tuple<_Args1 ...>, std::tuple<_Args2 ...>) [with _Args1 = {const std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&}; _Args2 = {}; _T1 = const std::__cxx11::basic_string<char>; _T2 = UDPChannel]’
/usr/include/c++/13/bits/new_allocator.h:191:4:   required from ‘void std::__new_allocator<_Tp>::construct(_Up*, _Args&& ...) [with _Up = std::pair<const std::__cxx11::basic_string<char>, UDPChannel>; _Args = {const std::piecewise_construct_t&, std::tuple<const std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&>, std::tuple<>}; _Tp = std::__detail::_Hash_node<std::pair<const std::__cxx11::basic_string<char>, UDPChannel>, true>]’
/usr/include/c++/13/bits/alloc_traits.h:538:17:   required from ‘static void std::allocator_traits<std::allocator<_CharT> >::construct(allocator_type&, _Up*, _Args&& ...) [with _Up = std::pair<const std::__cxx11::basic_string<char>, UDPChannel>; _Args = {const std::piecewise_construct_t&, std::tuple<const std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&>, std::tuple<>}; _Tp = std::__detail::_Hash_node<std::pair<const std::__cxx11::basic_string<char>, UDPChannel>, true>; allocator_type = std::allocator<std::__detail::_Hash_node<std::pair<const std::__cxx11::basic_string<char>, UDPChannel>, true> >]’
/usr/include/c++/13/bits/hashtable_policy.h:1995:36:   required from ‘std::__detail::_Hashtable_alloc<_NodeAlloc>::__node_type* std::__detail::_Hashtable_alloc<_NodeAlloc>::_M_allocate_node(_Args&& ...) [with _Args = {const std::piecewise_construct_t&, std::tuple<const std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&>, std::tuple<>}; _NodeAlloc = std::allocator<std::__detail::_Hash_node<std::pair<const std::__cxx11::basic_string<char>, UDPChannel>, true> >; __node_ptr = std::allocator<std::__detail::_Hash_node<std::pair<const std::__cxx11::basic_string<char>, UDPChannel>, true> >::value_type*]’
/usr/include/c++/13/bits/hashtable.h:307:35:   required from ‘std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::_Scoped_node::_Scoped_node(std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::__hashtable_alloc*, _Args&& ...) [with _Args = {const std::piecewise_construct_t&, std::tuple<const std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&>, std::tuple<>}; _Key = std::__cxx11::basic_string<char>; _Value = std::pair<const std::__cxx11::basic_string<char>, UDPChannel>; _Alloc = std::allocator<std::pair<const std::__cxx11::basic_string<char>, UDPChannel> >; _ExtractKey = std::__detail::_Select1st; _Equal = std::equal_to<std::__cxx11::basic_string<char> >; _Hash = std::hash<std::__cxx11::basic_string<char> >; _RangeHash = std::__detail::_Mod_range_hashing; _Unused = std::__detail::_Default_ranged_hash; _RehashPolicy = std::__detail::_Prime_rehash_policy; _Traits = std::__detail::_Hashtable_traits<true, false, true>; std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::__hashtable_alloc = std::_Hashtable<std::__cxx11::basic_string<char>, std::pair<const std::__cxx11::basic_string<char>, UDPChannel>, std::allocator<std::pair<const std::__cxx11::basic_string<char>, UDPChannel> >, std::__detail::_Select1st, std::equal_to<std::__cxx11::basic_string<char> >, std::hash<std::__cxx11::basic_string<char> >, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<true, false, true> >::__hashtable_alloc]’
/usr/include/c++/13/bits/hashtable_policy.h:818:42:   required from ‘std::__detail::_Map_base<_Key, std::pair<const _Key, _Val>, _Alloc, std::__detail::_Select1st, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits, true>::mapped_type& std::__detail::_Map_base<_Key, std::pair<const _Key, _Val>, _Alloc, std::__detail::_Select1st, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits, true>::operator[](const key_type&) [with _Key = std::__cxx11::basic_string<char>; _Val = UDPChannel; _Alloc = std::allocator<std::pair<const std::__cxx11::basic_string<char>, UDPChannel> >; _Equal = std::equal_to<std::__cxx11::basic_string<char> >; _Hash = std::hash<std::__cxx11::basic_string<char> >; _RangeHash = std::__detail::_Mod_range_hashing; _Unused = std::__detail::_Default_ranged_hash; _RehashPolicy = std::__detail::_Prime_rehash_policy; _Traits = std::__detail::_Hashtable_traits<true, false, true>; mapped_type = UDPChannel; key_type = std::__cxx11::basic_string<char>]’
/usr/include/c++/13/bits/unordered_map.h:987:20:   required from ‘std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>::mapped_type& std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>::operator[](const key_type&) [with _Key = std::__cxx11::basic_string<char>; _Tp = UDPChannel; _Hash = std::hash<std::__cxx11::basic_string<char> >; _Pred = std::equal_to<std::__cxx11::basic_string<char> >; _Alloc = std::allocator<std::pair<const std::__cxx11::basic_string<char>, UDPChannel> >; mapped_type = UDPChannel; key_type = std::__cxx11::basic_string<char>]’
/usr/local/mpp/cmd/cmd.cpp:146:48:   required from here
/usr/include/c++/13/tuple:2268:9: error: no matching function for call to ‘UDPChannel::UDPChannel()’
 2268 |         second(std::forward<_Args2>(std::get<_Indexes2>(__tuple2))...)
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/local/mpp/cmd/cmd.cpp:39:5: note: candidate: ‘UDPChannel::UDPChannel(const std::string&, int)’
   39 |     UDPChannel(const std::string& ip, int port) : ip(ip), port(port) {
      |     ^~~~~~~~~~
/usr/local/mpp/cmd/cmd.cpp:39:5: note:   candidate expects 2 arguments, 0 provided
/usr/local/mpp/cmd/cmd.cpp:37:7: note: candidate: ‘UDPChannel::UDPChannel(const UDPChannel&)’
   37 | class UDPChannel {
      |       ^~~~~~~~~~
/usr/local/mpp/cmd/cmd.cpp:37:7: note:   candidate expects 1 argument, 0 provided

//////////////////////////////////////////////

I ended up putting the default constructor in.

I've tried > send cmd_hud_dte Hello hud, from cmd!!! and with this command running:

sudo tcpdump -i any udp port 5000 -X

I would have expected to see the traffic.  Am I wrong about that?

Stepping through it with the debugger I noticed this:

    void send(const std::string& dst_ip, int dst_port, const std::string& message) {
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(dst_ip.c_str());
        dest.sin_port = htons(dst_port);
        sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&dest, sizeof(dest));
    }

instead of how it used to be:

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

so now when I step through it I don't get to see the error test of this:

        if (bytes_sent < 0) {
            perror("sendto failed");
        }

and that hampers troubleshooting a bit.  Consider reimplementing the ssize_t bytes_sent test in the future.

If we can get it working though, the command format is very nice!

///////////////////////////////////////////////////

Thanks for the updated send() method.

In the send command there's this in run():

                    invoker.addCommand(new SendCommand(ch, BASE_IP, CMD_BASE, message.substr(1)));

But SendCommand looks like this:

SendCommand(UDPChannel& channel, const std::string& dst_ip, int dst_port, const std::string& message)

implying that the dst_port is always the CMD_BASE when really the dst_port will never be the cmd including CMD_BASE.  The destination port was given in the program by this:

    {"cmd_hud_dte", BASE_IP, CMD_BASE + 0, BASE_IP, HUD_BASE + 1},

and so in the SendCommand the dst_port for this command:

> send cmd_hud_dte Hello hud, from cmd!!!

should, in some form be, HUD_BASE+1, but not hardcoded into the run() method but retrieved from the initialization as shown above and here:

    {"cmd_hud_dte", BASE_IP, CMD_BASE + 0, BASE_IP, HUD_BASE + 1},

Do you see a problem here?  I hope you understand what's wrong and can fix it.

//////////////////////////////////////////////

Ok this code is getting out of control and way off.  Maybe we should start again.  Remember, the code is working great as it was before all of this screwing around with the names, IPs and ports.  I'll give you cmd.cpp prior to the messing around of today.  This time please don't mess with anything in run() or main() but somehow implement this table:

    {"cmd_hud_dte", BASE_IP, CMD_BASE + 0, BASE_IP, HUD_BASE + 1},
    {"cmd_hud_dce", BASE_IP, CMD_BASE + 1, BASE_IP, HUD_BASE + 0},

    {"cmd_cnl_dte", BASE_IP, CMD_BASE + 2, BASE_IP, CNL_BASE + 1},
    {"cmd_cnl_dce", BASE_IP, CMD_BASE + 3, BASE_IP, CNL_BASE + 0},

    {"cmd_net_dte", BASE_IP, CMD_BASE + 4, BASE_IP, NET_BASE + 1},
    {"cmd_net_dce", BASE_IP, CMD_BASE + 5, BASE_IP, NET_BASE + 0},

with the base ports being:

cmd - 5000
hud - 6000
cnl - 7000
net - 8000

Even if you have to hard code it, that's ok.  I like the way cmd is working in the way it is below so I want to keep it like that but in the very least intrusive of manners I'd like for the connections given above to be incorporated.

// cmd.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <cstdlib>

#define PORT_BASE 6000

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

    UDPChannel(const UDPChannel&) = delete;
    UDPChannel& operator=(const UDPChannel&) = delete;

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
        else std::cout << "> No data available" << std::endl;
    }
    
private:
    UDPChannel& channel;
};

class CommandInvoker {
public:
    void addCommand(Command* cmd) {
        commandQueue.push(cmd);
    }
    
    void executeCommands() {
        while (!commandQueue.empty()) {
            Command* cmd = commandQueue.front();
            commandQueue.pop();
            cmd->execute();
            std::cout << "> ";
            delete cmd;
        }
    }
    
private:
    std::queue<Command*> commandQueue;
};

void run(std::unordered_map<int, UDPChannel>& channels, CommandInvoker& invoker) {
    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    std::cout << "> ";
    while (true) {
        std::cout.flush();
        invoker.executeCommands();
        if (poll(&pfd, 1, 100) > 0) {
            std::string line;
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
                    std::string message = line.substr(line.find(tokens[4]));
                    invoker.addCommand(new SendCommand(channels[ch], tokens[2], dst_port, message));
                } else {
                    std::cerr << "> Invalid channel: " << ch << std::endl;
                }
            } else if (tokens[0] == "recv" && tokens.size() == 2) {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end()) {
                    invoker.addCommand(new RecvCommand(channels[ch]));
                } else {
                    std::cerr << "> Invalid channel: " << ch << std::endl;
                }
            } else {
                std::cout << "> Invalid command" << std::endl;
            }
        }
    }
}

int main() {
    int base_port = PORT_BASE;
    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;
    for (int i = 0; i < 6; ++i) {
        channels.emplace(i, UDPChannel("127.0.0.1", base_port + i));
    }
    run(channels, invoker);
    return 0;
}

///////////////////////////////////////////////////

I'm going to send you my latest cmd code.  Since you last saw it I've created a way to name and assign the UDP channels that works pretty simply and easily.  I've also add a class Cmd and two files, Cmd.cpp and Cmd.h.

For now my hope is that after you study this latest code base that I'm going to send you this message you'll have an answer for my question about getting the Cmd::run() method sort of isolated so that you and I can leave what code we've done already alone and move ahead working pretty much only in files outside of main.cpp.  I was thinking that maybe you could help me start a new thread for Cmd::run().  Would it be a good idea?  Is that a good strategy for decoupling or in other words getting away so I can't break it?  Then I could work all of my Cmd-specific stuff over in whatever spawns out of the new thread and Cmd.cpp and Cmd.hpp and hud could do its Hud-specific stuff in whatever spawns out of its new thread and Hud.cpp and Hud.hpp and so on for cnl and net.

Or dabbling in threads a mistake to get into considering the extreme simplicity of these components up to this point.  Would starting into threads kind of ruin the simplicity where practically speaking it's unlikely that processing power or performance will be an issue because of the micro-service/IPC model where the CPU is shared generously between my components, at least theoretically, right?

Either way, new thread or not it's a question how I'm going to get my component-specific code split away from the main.cpp stuff.  Would a GoF Facade pattern work well here?  But remember, simplicity is becoming a real hallmark of this project so let's treat simplicity as a pretty highly prioritized characteristic.

And while we're talking about the future, partner, what do you think of making a GoF Observer pattern where the Observers are something along the lines of computer issued recv commands for whatever observer is notified and has its update() method triggered and the subject is the dce ports of each component?  It's just an idea, let's not make any code yet, just talk for now.

I'm eager to hear your thoughts and questions and comments.  See the code below:

// main.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <cstdlib>
#include "mpp_defines.h"
#include "Cmd.hpp"

class UDPChannel
{
public:
    UDPChannel() = default;
    UDPChannel(const std::string &ip, int port) : ip(ip), port(port)
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("Socket creation failed");
            exit(1);
        }
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("Bind failed");
            exit(1);
        }
        std::cout << "UDPChannel bound to " << ip << ":" << port << " with sockfd " << sockfd << std::endl;
    }

    UDPChannel(const UDPChannel &) = delete;
    UDPChannel &operator=(const UDPChannel &) = delete;

    UDPChannel(UDPChannel &&other) noexcept
        : sockfd(other.sockfd), ip(std::move(other.ip)), port(other.port)
    {
        other.sockfd = -1;
    }

    UDPChannel &operator=(UDPChannel &&other) noexcept
    {
        if (this != &other)
        {
            close(sockfd);
            sockfd = other.sockfd;
            ip = std::move(other.ip);
            port = other.port;
            other.sockfd = -1;
        }
        return *this;
    }

    void send(const std::string &dst_ip, int dst_port, const std::string &message)
    {
        if (sockfd < 0)
        {
            std::cerr << "Socket not initialized properly!" << std::endl;
            return;
        }
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(dst_ip.c_str());
        dest.sin_port = htons(dst_port);
        std::cout << "Sending to " << dst_ip << ":" << dst_port << " via socket " << sockfd << std::endl;
        ssize_t bytes_sent = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&dest, sizeof(dest));
        if (bytes_sent < 0)
        {
            perror("sendto failed");
        }
    }

    std::string recv()
    {
        char buffer[1024] = {0};
        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT, (struct sockaddr *)&sender, &sender_len);
        if (len > 0)
        {
            buffer[len] = '\0';
            return std::string(buffer);
        }
        return "";
    }

    ~UDPChannel()
    {
        if (sockfd >= 0)
        {
            close(sockfd);
        }
    }

private:
    int sockfd;
    std::string ip;
    int port;
};

class Command
{
public:
    virtual void execute() = 0;
    virtual ~Command() = default;
};

class SendCommand : public Command
{
public:
    SendCommand(UDPChannel &channel, const std::string &dst_ip, int dst_port, const std::string &message)
        : channel(channel), dst_ip(dst_ip), dst_port(dst_port), message(message) {}

    void execute() override { channel.send(dst_ip, dst_port, message); }

private:
    UDPChannel &channel;
    std::string dst_ip;
    int dst_port;
    std::string message;
};

class RecvCommand : public Command
{
public:
    RecvCommand(UDPChannel &channel) : channel(channel) {}

    void execute() override
    {
        std::string msg = channel.recv();
        if (!msg.empty())
            std::cout << "Received: " << msg << std::endl;
        else
            std::cout << "> No data available" << std::endl;
    }

private:
    UDPChannel &channel;
};

class CommandInvoker
{
public:
    void addCommand(Command *cmd)
    {
        commandQueue.push(cmd);
    }

    void executeCommands()
    {
        while (!commandQueue.empty())
        {
            Command *cmd = commandQueue.front();
            commandQueue.pop();
            cmd->execute();
            std::cout << "> ";
            delete cmd;
        }
    }

private:
    std::queue<Command *> commandQueue;
};

void run(std::unordered_map<int, UDPChannel> &channels, CommandInvoker &invoker)
{
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    std::cout << "> ";
    invoker.addCommand(new SendCommand(channels[CMD_HUD_DTE], IP_ADDRESS, HUD_CMD_DCE, "Hello to hud from cmd!!!")); 
    while (true)
    {
        std::cout.flush();
        invoker.executeCommands();
        if (poll(&pfd, 1, 100) > 0)
        {
            std::string line;
            std::getline(std::cin, line);
            if (line == "exit")
                break;

            std::vector<std::string> tokens;
            std::istringstream iss(line);
            std::string token;
            while (iss >> token)
                tokens.push_back(token);

            if (tokens.empty())
                continue;
            if (tokens[0] == "send" && tokens.size() >= 5)
            {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end())
                {
                    int dst_port = std::stoi(tokens[3]);
                    std::string message = line.substr(line.find(tokens[4]));
                    invoker.addCommand(new SendCommand(channels[ch], tokens[2], dst_port, message));
                }
                else
                {
                    std::cerr << "> Invalid channel: " << ch << std::endl;
                }
            }
            else if (tokens[0] == "recv" && tokens.size() == 2)
            {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end())
                {
                    invoker.addCommand(new RecvCommand(channels[ch]));
                }
                else
                {
                    std::cerr << "> Invalid channel: " << ch << std::endl;
                }
            }
            else
            {
                std::cout << "> Invalid command" << std::endl;
            }
        }
    }
}

int main()
{
    Cmd cmd;
    cmd.run();
    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;
    for (int i = CMD_PORT_BASE; i < CMD_PORT_BASE + 6; ++i)
    {
        channels.emplace(i, UDPChannel(IP_ADDRESS, i));
    }
    run(channels, invoker);
    return 0;
}

// Cmd.hpp
class Cmd {
    private:

    public:
        void run();
};

// Cmd.cpp
#include "Cmd.hpp"
#include <iostream>

void Cmd::run() {
    std::cout << "In run()\n";
}

// mpp_defines.h

#define IP_ADDRESS "127.0.0.1"
#define CMD_PORT_BASE 5000
#define HUD_PORT_BASE 6000
#define CNL_PORT_BASE 7000
#define NET_PORT_BASE 8000
#define CMD_HUD_DTE 5000
#define CMD_HUD_DCE 5001
#define CMD_CNL_DTE 5002
#define CMD_CNL_DCE 5003
#define CMD_NET_DTE 5004
#define CMD_NET_DCE 5005
#define HUD_CMD_DTE 6000
#define HUD_CMD_DCE 6001
#define HUD_CNL_DTE 6002
#define HUD_CNL_DCE 6003
#define HUD_NET_DTE 6004
#define HUD_NET_DCE 6005
#define CNL_CMD_DTE 7000
#define CNL_CMD_DCE 7001
#define CNL_HUD_DTE 7002
#define CNL_HUD_DCE 7003
#define CNL_NET_DTE 7004
#define CNL_NET_DCE 7005
#define NET_CMD_DTE 8000
#define NET_CMD_DCE 8001
#define NET_HUD_DTE 8002
#define NET_HUD_DCE 8003
#define NET_CNL_DTE 8004
#define NET_CNL_DCE 8005

/////////////////////////////////////

Alright parter, correct me if I'm wrong, but it looks like we made a potentially pretty powerfull little platform here.  There are 6 UDP channels per component and they're all organized in a very light and simple way contained pretty much in under 300 lines in a single file where there's a thread and a class just waiting to be used for a specialized functionality set.  If I'm right about that then that's pretty cool!

Another nice thing is how easy it is to turn one kind of component into any other kind of component just by changing a few letters.  This code base is so small and there's so much commonality between the components that git is pretty much not needed anymore too.

Now that cmd has it's own class and thread this current form of cmd will be the basic core of all of the components.  Create components based on our latest cmd component of cnl, hud and net.  They will have unique port addresses and they'll have unique classes such as Cmd, Cnl, Hud and Net.

- cmd
  The cmd component is a long term componet which will be a command processing component.  Even though it's the one we've used as our starting point, we're going to do work mostly on the other three components before we develope cmd anymore.

- net
  The net component uses libnet and pcap to handle sending and receiving real-world ethernet frames or IP packets or whatever network traffic out of in in from the NIC, eno1.  The net component in this first incarnation will simply pass traffic from the NIC to the cnl through NET_CNL_DTE.

- cnl
  The cnl component is the channel component and it monitors the traffic that it sees coming from net on CNL_NET_DCE (what NET_CNL_DTE will use as the dst_port) and for particular conditions it'll alarm hud through an alarm message sent on channel CNL_HUD_DTE.
  
- hud
  The hud component stands for Heads-Up Display and it's a network traffic visualizatin tool.  When it receives alarm messages from the cnl on HUD_CNL_DCE (which is the dst_port for CNL_HUD_DTE) it'll advance/update the ascii graphics and network related data it displaying in it's ncurses window.

Eventually Cmd will orchestrate proceedings of the 4 mpp components but for now in these early times I'd like to just orchestrate a very simple and well-defined connection between the components where for now cmd won't really do very much.

Are you getting the picture?  Got any questions or comments or ideas, partner?

/////////////////////////////////////////////

Let me run this by you that pretty much our four components are simple little machines that have quite a bit of processing power each and they are equipt with almost infinite communmications potential with a combined 24 UDP channels.  Eventually I want the cmd to kind of be the brains of the operation and handle user-defined commands, a DSL and scripting with Lua, phthon and scapy.  It'll allow a user/programmer to define network scenarios and monitor, interact, measure, log, report or whatever the user/programmer wants.  The net I want to pass NIC traffic captured by pcap from the NIC to cnl and transmit with libnet what the cnl sends to it.  The cnl component will react to the network traffic it receives from net depending on how it was defined it should react by it's directives given by cmd.  For now, however, I want to just simulate the network traffic with network and orchestrate very controlled and predictable test traffic.  The cnl component will send messages to the Heads-up Display, hud, which will display ascii graphics according to the messages sent by cnl.  Its purpose is to visualize network scenarios as they transpire.  I'll share some ascii art of what a hud will look like.

The shape below is what I call a channel.  It represents visually whats happening in cnl (cnl means channel).  If you can't see it very good I'll describe it as being made with the pipe character, the forward slash and the baskslash and it looks like a pipe character at the top which branches off to each side with the slash characters on either side of the pipe on the very top line.  Then in line three it's like under the slash characters on the second line staighten out and go straight down but using pipe characters that go down for ten lines 3 spaces apart before they use the slash characters again to this time join together the pipe characters three spaces apart to a single pipe character at the bottom of the ascii graphic.

            |
           / \
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
           \ /
            |

So that's the basic channel ascii graphic.  Now add to it a spinner.  You know a spinner?  One of those ascii graphics that looks like a character space has a bar spinning in it by alternating from /, -, \, |, /, -, \ etc.  There will be a spinner in the channel and I'll mark it with '/'.



            |
           / \
          | / |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
           \ /
            |

Now imagine as cnl discovers network traffic that fits certain criteria it'll report on it by send json to the hud.  The hud class will be made so that the state of elements of it itself and also the state of the channel displayed in the hud will be defined by the json it receives from hud (or maybe cmd, not sure yet, we'll need to discuss it and decide).

Imagine the hud being a class that has methods that'll just blindly run according to what json is in the messages it receives.  This information will include whether the spinner should be on or off, what color it should be and row it should be on.  Other class attributes will be set by the json and methods will manage the colors of the pipe and slash characters of the pipe.  And that's plain and simply all I want the hud to do.  The hud class will be able to run a main loop that'll get messages of the configuration json and simply load the configuration so that the hud looks as it's ordered to - spinner on or off, proper color and the pipes and slashes on each row are colored as instructed by the json.

I'm wondering if we should involve the cmd component in the first test and I'm slightly inclined to think we should let the cmd component act out the role of commander but in reality in our test environment it'll be all orchestrated.

Is it making sense?

////////////////////////////////////////////

We have lots of comms channels so we can simulate traffic very easily to orchestrate a test scenario.

I agree with your takes on things.  Yes to the active cmd.  I'd like to pass everything through cmd for now.  I cringe a little at the centralization and the bottle-necking but there's a place for everything in certain circumstances and I have a hunch that in the future the user/programmers are going to find some facility in a convention that traffic and commands and whatnot tend to go throught the cmd.  But, if not, what the heck, who cares because with 24 UDP comms channels there's tons of bandwidth to use channels as a user/programmer sees fit at any time.  So, we're not really under great pressure.  I think we have lots of simplicity and flexibility on our hands so we're in the driver's seat, so to speak, would you agree?  I agree with your take on the simplicity of the channel to start off with.  How much detail should the test simulation have you ask?  The suggestion of yours for "Basic: Just a simple on/off state for the spinner." sounds just right, for now.

It's nice now that we have our classes for each component just sitting there with a place holder for some functionality running in its own thread.  All the components have access to the send command and the recv command and can use the macros in mpp_defines.hpp to define messages.  It should be a fairly simple matter of adding whatever it is that each component needs to do in this test to the class that's been made named after the components that it is to provide functionality for in its own thread.

It's all coming together pretty well, wouldn't you say?

I'll put some definition to the test below:

- for now because each component is just like cmd when we branched off we know that we can use the net component's send command to simulate traffic to the cnl component.  I'm going to go back on my earlier inclination to include the cmd.  I now want to leave cmd out of things and just run a net component of which I just use the repl to send messages to the cnl of NET_CNL_DTE (cnl receive it on its CNL_NET_DCE) as if it was real network messages.  These orchestrated messages will trigger the cnl to send messages to the hud over CNL_HUD_DTE (hud receive it on its HUD_CNL_DCE) and unpack the json, update the hud class and see the reaction in the visualization of the hud component.

We're getting closer.  Any more questions or concerns or comments or ideas?

///////////////////////////////////////////////

I've put together a rough draft of some pseudo-code, in no particular format - just something I made up and I hope you'll understand it so let me know if it's nonsense to you.  It shows file names as though they are classes sort of and roughs-in some class methods and attributes:

Cmd.cpp {
    run() {



    }
};
// This component won't do anything for now.

Hud.hpp {
    private:
        bool spinner_on;

    public:
        void run_spinner();
        void run();

};

Hud.cpp {
    Hud::Hud()
    {
        spinner_on = false;
    }

    Hud::~Hud()
    {
        spinner_on = false;
    }

    run()
    {
        while(running)
        {
            if(msg = receive_msg_from_cnl() => read from HUD_CNL_DCE)
            {
                msg json contents assigned to Hud attributes
            }
        }
    }
};

Cnl.hpp {
    private:

    public:
        void run();

};

Cnl.cpp {

    Cnl::Cnl()
    {
    }

    Cnl::~Cnl()
    {
    }

    run()
    {
        while(running)
        {
            if(msg = receive_msg_from_net() => read from CNL_NET_DCE)
            {
                if(msg == trigger 1)
                {
                    write_trigger1_to_hud() => write json for spinner_on: true to CNL_HUD_DTE
                }
                else if(msg == trigger 2)
                {
                    write_trigger2_to_hud() => write json for spinner_on: false to CNL_HUD_DTE
                }
                else if(msg == exit)
                {
                    running = false;
                }

            }
        }
    }

    run_spinner()
    {
        if(spinner_on)
        {
            cycle through the characters of /, -, \, |
        }
        else
        {
            spinner character = '/';
        }

    }
};

Net.cpp
    run() {


        
    }
// This class won't use its run() method.  I'll just orchestrate appropriate
// >send commands from the component's built in command invoker.  I'll send
//messages to cnl like "trigger 1", "trigger 2" etc. and cnl will subsequently
// send a json message to hud with just one thing in it like:
//
// trigger 1:
// {
//     "spinner_on": true
// }

Would you like to add anything or comment or ask questions?  What are your thoughts about how this sort of arrangement might work for our current purposes?

////////////////////////////////////////////////


