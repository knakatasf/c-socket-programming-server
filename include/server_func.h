#ifndef PROJECT1_SERVER_FUNC_H
#define PROJECT1_SERVER_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>

struct socket_pair {
    int server_sock;
    int client_sock;
};

struct socket_pair open_client_tcp_socket(const int);

void receive_json_via_tcp(const int);

long receive_packet_trains(const int server_port, const int num_of_packets, const int packet_size);

long measure_time_for_packet_trains(const int sock, char buffer[], const int packet_size, struct sockaddr_in*);

void send_result(const int server_port, const long result);

#endif