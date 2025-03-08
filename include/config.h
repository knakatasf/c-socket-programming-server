#ifndef CONFIG_H
#define CONFIG_H

#include <cjson/cJSON.h>

extern int PRE_TCP_PORT; // 7777
extern int POST_TCP_PORT; // 6666
extern int CLIENT_UDP_PORT; //9876
extern int SERVER_UDP_PORT; // 8765

extern int UNCORP_TCP_HEAD; // 9999
extern int UNCORP_TCP_TAIL; // 8888

extern char SERVER_IP[17]; // "192.168.64.7"

extern int UDP_SIZE; // 1000B
extern int INTER_MEASUREMENT_TIME; // 15 secs
extern int NUM_OF_UDP_PACKETS; // 6000
extern int TTL; // 255

void load_config(const char*);

#endif