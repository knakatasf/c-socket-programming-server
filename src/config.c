#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/config.h"

int PRE_TCP_PORT;
int POST_TCP_PORT;
int CLIENT_UDP_PORT;
int SERVER_UDP_PORT;

int UNCORP_TCP_HEAD;
int UNCORP_TCP_TAIL;

char CLIENT_IP[17];
char SERVER_IP[17];

int UDP_SIZE;
int INTER_MEASUREMENT_TIME;
int NUM_OF_UDP_PACKETS;
int TTL;

void load_config(const char* json_data)
{
    cJSON *json = cJSON_Parse(json_data);
    if (!json) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return;
    }

    PRE_TCP_PORT = cJSON_GetObjectItem(json, "pre_tcp_port")->valueint;
    POST_TCP_PORT = cJSON_GetObjectItem(json, "post_tcp_port")->valueint;
    CLIENT_UDP_PORT = cJSON_GetObjectItem(json, "client_udp_port")->valueint;
    SERVER_UDP_PORT = cJSON_GetObjectItem(json, "server_udp_port")->valueint;

    UNCORP_TCP_HEAD = cJSON_GetObjectItem(json, "uncorp_tcp_head")->valueint;
    UNCORP_TCP_TAIL = cJSON_GetObjectItem(json, "uncorp_tcp_tail")->valueint;

    const char *c_ip = cJSON_GetObjectItem(json, "client_ip")->valuestring;
    strncpy(CLIENT_IP, c_ip, sizeof(CLIENT_IP) - 1);
    CLIENT_IP[sizeof(CLIENT_IP) - 1] = '\0';

    const char *s_ip = cJSON_GetObjectItem(json, "server_ip")->valuestring;
    strncpy(SERVER_IP, s_ip, sizeof(SERVER_IP) - 1);
    SERVER_IP[sizeof(SERVER_IP) - 1] = '\0';

    UDP_SIZE = cJSON_GetObjectItem(json, "udp_payload_size")->valueint;
    INTER_MEASUREMENT_TIME = cJSON_GetObjectItem(json, "inter_measurement_time")->valueint;
    NUM_OF_UDP_PACKETS = cJSON_GetObjectItem(json, "num_of_udp_packets")->valueint;
    TTL = cJSON_GetObjectItem(json, "ttl")->valueint;

    cJSON_Delete(json);
}