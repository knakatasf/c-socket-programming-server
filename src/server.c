#include "../include/server_func.h"
#include "../include/config.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Program argument error: %s <PORT_NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int port_num = atoi(argv[1]);
    receive_json_via_tcp(port_num);
    long result = receive_packet_trains(SERVER_UDP_PORT, NUM_OF_UDP_PACKETS, UDP_SIZE);
    send_result(POST_TCP_PORT, result);
    return 0;
}