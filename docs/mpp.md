## Upon startup each component must: ##

- Create 6 taps assigned as per the table below.
- Initialize each tap with libnet and pcap.
- Be in an operating state to store pcap captured EthernetFrames in the receive buffer of each tap as they arrive.
- Have 4 commands tx, rx, quiet and exit to respond to.
- When a command is entered the Command pattern facilitates not only normal Command pattern command execution but by default it also serializes the Command object of the command that was entered and broadcasts it on all three of it's DTE ports.  The command transmission behavior can be suppressed by a command: "quiet" in a toggled manner (another quiet command will actually make it broadcast each serialized command again, like the default state).  The quiet command is never broadcast.  If a tx command is issued and quiet hasn't stopped the command broadcast behavior then the command is broadcast first, then the normal data EthernetFrame is transmitted to the dst_tap.
- When a tx <dte_tap> <dce_tap> <data> is entered the data is sent in an EthernetFrame from dst_tap.
- When an rx <dce_tap> command is entered the data of the first EthernetFrame entry in the rx buffer (just the data, the payload) in the pcap rx buffer will be returned, if there are any.


## TAP-to-MAC Mapping

| Component | TX TAP Name (DTE) | Destination (DCE) | TX MAC Address | RX TAP Name (DCE) | Source (DTE) | RX MAC Address |
|-----------|------------------|-------------------|----------------|------------------|--------------|----------------|
| HUD       | hud_tx_cmd       | CMD               | 02:00:00:00:01 | hud_rx_cmd       | CMD          | 02:00:00:00:02 |
| HUD       | hud_tx_net       | NET               | 02:00:00:00:03 | hud_rx_net       | NET          | 02:00:00:00:04 |
| HUD       | hud_tx_cnl       | CNL               | 02:00:00:00:07 | hud_rx_cnl       | CNL          | 02:00:00:00:08 |
| CMD       | cmd_tx_hud       | HUD               | 02:00:00:00:02 | cmd_rx_hud       | HUD          | 02:00:00:00:01 |
| CMD       | cmd_tx_net       | NET               | 02:00:00:00:05 | cmd_rx_net       | NET          | 02:00:00:00:06 |
| CMD       | cmd_tx_cnl       | CNL               | 02:00:00:00:09 | cmd_rx_cnl       | CNL          | 02:00:00:00:10 |
| NET       | net_tx_hud       | HUD               | 02:00:00:00:04 | net_rx_hud       | HUD          | 02:00:00:00:03 |
| NET       | net_tx_cmd       | CMD               | 02:00:00:00:06 | net_rx_cmd       | CMD          | 02:00:00:00:05 |
| NET       | net_tx_cnl       | CNL               | 02:00:00:00:11 | net_rx_cnl       | CNL          | 02:00:00:00:12 |
| CNL       | cnl_tx_hud       | HUD               | 02:00:00:00:08 | cnl_rx_hud       | HUD          | 02:00:00:00:07 |
| CNL       | cnl_tx_cmd       | CMD               | 02:00:00:00:10 | cnl_rx_cmd       | CMD          | 02:00:00:00:09 |
| CNL       | cnl_tx_net       | NET               | 02:00:00:00:12 | cnl_rx_net       | NET          | 02:00:00:00:11 |

## General Comments Answers
### 1. TAP Creation & Initialization

- Each component owns its own taps; no shared taps.  In the future bridges might be used strategically.
- Each coponent uses its own MAC to identify messages for it.  It'll purge messages wanted or unwanted manually by calling rx <dce_tap>.  For this reason in common practice the very first command a user will issue is quiet.  But the initial broadcast scheme gives the component a heartbeat at startup and could serve as a start point for registration in the system and if it's not needed a user can switch it off with the quiet command (toggle broadcast off).
- An analogy for a component is of a computer that starts up and runs its BIOS and after the BIOS/UEFI/GRUB process it produces a command prompt and waits, with all the peripheral devices initialized and ready.  After that point a user can then customize the actions of the computer, or in our case component.
- Question for you, Chat GPT partner: will we create the taps with the ioctl call?  Or the system call?  If I recall correctly we never got the ioctl call method of tap creating working.  The ioctl call is my preferred methog but can use the system calls for now as far as I'm concerned.
  
### 2. Handling Incoming Ethernet Frames (RX)
- The RX buffer will be FIFO and there will be a FIFO per DCE tap.
- The generic call to a receive FIFO will return just the data of the oldest frame in the FIFO and remove that entire frame from the FIFO and discard it.  Differnet reception schemes can be put in place by the user as an MPP session progresses.
- On startup the components are ready to receive rx <dce_tap> commands and that's the only way immediately after initial startup that the receive FIFOs are accessed.  A user manually enters rx <dce_tap> to retreive the oldest frame payload from the dce_tap FIFO and the rest of the frame is discarded.  The rx command returns an empty string if there are no messages.

### 3. Command Processing
- Each component implements a Command pattern where every command issued gets serialized in the JSON format and broadcast on all three DTE ports unless the quiet mode is toggled on.
- Question for you, Chat GPT partner: would you like to implement formal GoF Command and Observer patterns or are you inclined towards some kind of sort of hybrid pattern?  My inclination is towards patterns that are formal GoF patterns but I'd like you to realise that I'm admittedly naive about the matter and my inclination towards formal GoF patterns is guided to an appreciable degree by a lack of confidence and experience.  But if you're still inclined towards some relaxed or hybrid or otherwise modified GoF patterns, feeling that it's for the best overall then I would be grateful to benefit from your wisdom and decision making.
- Should the recipient store and process received command objects immediately, or does it just log them?  The recipient component just puts received frames in its corresponding DCEs FIFO and forgets about them.  When user issued rx <dce_tap> commands are executed the data of the first EthernetFrame is returned and the rest of the EthernetFrame is discarded.
- Can multiple commands be processed at the same time?  I wasn't thinking that multiple commands could be processed at one time, but I'm not opposed to the idea?  Can you expand on the genesis of your question?  It's very interesting.

### 4. Command Transmission Flow
- Commands are serialized into the JSON format.


### 5. Quiet Mode Behavior
- The quiet mode toggle affects all the DTEs of a component so all DTEs stop their default command broadcast when a quiet command is user issued.
- Commands affect only the issuing component.  The user can decide what to do with received messages whether they are direct transmissions of commands, direct transmission of any data, broadcast commands, broadcasts of any data or any other received EthernetFrames.

### 6. CNL Component Introduction
- The CNL is a filtering component, but it's also part of the HUD visualization.  HUD will configure CNL through hud_tx_cnl during normal operation.  We don't have an exact scheme, protocol, process or system or anything in particular for the HUD/CNL configuration and operation presently.  But keep in mind that the CNL, or any component is free to communicate through the DTEs and DCEs to accomplish and functionality or configuration that the user can make to acheive the desired results.  But initially we're just making all the coponents the exact same, like this initial version of the CMD component.  Later the specific functionality of each block will be added.
- - Question for you, Chat GPT partner: Can you see the benefits of this scheme and approach?  Are there some industry norms for startup conditions like these (like perhaps the BIOS/UEFI/GRUB example) that you're attracted to and would prefer to implement here?  But if so, let's try not to add a dependency outside of what can be gotten through apt.

### 7. Error Handling and Logging
- There is no lgging nor error handling nor debugging mechanism.
- What happens if a tx destination is invalid?  I don't know.  That's the user's concern, not ours and not the component's.

///////////////////////////

Hey do you think you could help with this output from 
g++ -o demo demo.cpp -Iusr/include/jsoncpp/json -lnet -lpcap -ljsoncpp

root@PRED:/usr/local/cmd# g++ -o demo demo.cpp -Iusr/include/jsoncpp/json -lnet -lpcap -ljsoncpp
In file included from demo.cpp:1:
./DCE.hpp: In member function ‘virtual void DCE::detach(std::shared_ptr<IObserver>)’:
./DCE.hpp:46:52: error: cannot convert ‘std::vector<std::shared_ptr<IObserver> >::iterator’ to ‘const char*’
   46 |         observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
      |                                     ~~~~~~~~~~~~~~~^~
      |                                                    |
      |                                                    std::vector<std::shared_ptr<IObserver> >::iterator
In file included from /usr/include/c++/13/cstdio:42,
                 from /usr/include/c++/13/ext/string_conversions.h:45,
                 from /usr/include/c++/13/bits/basic_string.h:4109,
                 from /usr/include/c++/13/string:54,
                 from /usr/include/c++/13/bits/locale_classes.h:40,
                 from /usr/include/c++/13/bits/ios_base.h:41,
                 from /usr/include/c++/13/ios:44,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
                 from ./Observer.hpp:6,
                 from ./DCE.hpp:4:
/usr/include/stdio.h:158:32: note:   initializing argument 1 of ‘int remove(const char*)’
  158 | extern int remove (const char *__filename) __THROW;
      |                    ~~~~~~~~~~~~^~~~~~~~~~
root@PRED:/usr/local/cmd# 


