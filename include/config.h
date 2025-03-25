#ifndef CONFIG_H
#define CONFIG_H

#include <cjson/cJSON.h>

#define SOCKET_TIMEOUT 60 // The timeout for the udp socket to stop listening to the packet train

extern int PRE_TCP_PORT; // The server's port for pre-probing; default 7777
extern int POST_TCP_PORT; // The server's port for post-probing; default 6666
extern int CLIENT_UDP_PORT; // The client's port for probing; default 9876
extern int SERVER_UDP_PORT; // The server's port for probing; default 8765

extern int UNCORP_TCP_HEAD; // The server's port for the head SYN packet; default 9999
extern int UNCORP_TCP_TAIL; // The server's port for the tail SYN packet; default 8888

extern char CLIENT_IP[17]; // The client's IP address; "192.168.64.2"
extern char SERVER_IP[17]; // The server's IP address; "192.168.64.7"

extern int UDP_SIZE; // The payload size of the packet in the packet train; default 1000B
extern int INTER_MEASUREMENT_TIME; // 15 secs
extern int NUM_OF_UDP_PACKETS; // The number of the packets in the packet train; default 6000
extern int TTL; // The TTL for the udp packets in the train; default 255

void load_config(const char*);

#endif