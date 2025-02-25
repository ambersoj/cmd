## Upon startup each component must: ##

- create 6 taps assigned as per the table below.
- initialize each tap with libnet and pcap.
- be in an operating state to store pcap captured EthernetFrames in the receive buffer of each tap as they arrive.
- Have 4 commands tx, rx, quiet and exit to respond to.
- When a command is entered the Command pattern facilitates not only normal Command pattern command execution but by default it also serializes the Command object of the command that was entered and broadcasts it on all three of it's DTE ports.  The command transmission behavior can be suppressed by a command: "quiet" in a toggled manner (another quiet command will actually make it broadcast each serialized command again, like the default state).  The quiet command is never broadcast.  If a tx command is issued and quiet hasn't stopped the command broadcast behvior then the command is broadcast first, then the normal data EthernetFrame is transmitted to the dst_tap.
- When a tx <dst_tap> <data> is entered the data is sent in an EthernetFrame to dst_tap.
- When an rx command is entered the data of the first EthernetFrame entry in the rx buffer (just the data, the payload) in the pcap rx buffer will be returned, if there are any.


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


