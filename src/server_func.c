#include "../include/server_func.h"
#include "../include/config.h"

/*
 *  Creates a welcoming socket, specifying the server's port. Waits for a client request.
 *  Upon acceptance of the client request, open a client socket; return both sockets.
 */
struct socket_pair open_client_tcp_socket(const int server_port)
{
    struct socket_pair socks;
    socks.server_sock = socket(PF_INET, SOCK_STREAM, 0);

    /* Need this so the server socket can quickly reuse the same address and port */
    int opt = 1;
    if (setsockopt(socks.server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Error setting SO_REUSEADDR");
        close(socks.server_sock);
        exit(EXIT_FAILURE);
    }
    
    /* Prepares the server's address and port */
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);
    
    /* Binds the socket with the specified port */
    if (bind(socks.server_sock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        perror("Couldn't bind socket to the address");
        close(socks.server_sock);
        exit(EXIT_FAILURE);
    }
    
    if (listen(socks.server_sock, 5) < 0)
    {
        perror("Error listening");
        close(socks.server_sock);
        exit(EXIT_FAILURE);
    }
    
    //printf("Server listening to port %d...\n", server_port);
    
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    socks.client_sock = accept(socks.server_sock, (struct sockaddr*)&addr, &addr_len);
    if (socks.client_sock < 0)
    {
        perror("Error accepting connection");
        close(socks.server_sock);
        exit(EXIT_FAILURE);
    }
    //printf("Client connected!\n");
    return socks;
}

/* 
 * Receives the json data from the client for the configuration
 * Calls load_config so that the program can access environment variables
 */
void receive_json_via_tcp(const int server_port)
{
    struct socket_pair socks = open_client_tcp_socket(server_port);
    
    char buffer[JSON_DATA_SIZE];
    int byte_received = recv(socks.client_sock, buffer, sizeof(buffer) - 1, 0);
    if (byte_received < 0)
        perror("Error receiving data");
    else
    {
        buffer[byte_received] = '\0';
        // printf("Received json data:\n%s", buffer);
        load_config(buffer); // Populate the environment variables
    }
    close(socks.client_sock);
    close(socks.server_sock);
}

long receive_packet_trains(const int server_port, const int num_of_packets, const int packet_size)
{
    struct sockaddr_in server_addr, client_addr;
    int sock = socket(PF_INET, SOCK_DGRAM, 0);

    int receive_buffer_size = RECEIVE_SOCK_SIZE; 
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &receive_buffer_size, sizeof(receive_buffer_size)) < 0)
    {
        perror("Failed to set receive buffer size");
    }

    /*
     * recvfrom() will block until receiving a packet; we want to set the socket's timeout.
     * For a certain time, if the socket doesn't receive any packet, stop waiting.
     */
    struct timeval timeout;
    timeout.tv_sec = SOCKET_TIMEOUT; // 60 secs is a resonable value for timeout
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) // Specifies the server's port
    {
        perror("Bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    /* Allocates enough buffer to receive the packet trains */
    char* buffer = malloc(packet_size * 2);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(sock);
        exit(EXIT_FAILURE);
    }

    long time_diff = measure_time_for_packet_trains(sock, buffer, packet_size, &client_addr);

    free(buffer);
    close(sock);

    return time_diff;
}

/*
 *  Uses a while loop to indefinitely waiting for a packet to arrive.
 *  Escape the loop when the socket waits for a certain time: timeout without receiving any packet.
 */
long measure_time_for_packet_trains(const int sock, char buffer[], const int packet_size, struct sockaddr_in* client_addr_ptr)
{   
    socklen_t addr_len = sizeof(*client_addr_ptr);
    struct timeval first_packet_time_for_zero_train, last_packet_time_for_zero_train,
                   first_packet_time_for_random_train, last_packet_time_for_random_train;
    int first_packet_received_for_zero_train = 0;
    int first_packet_received_for_random_train = 0;
    int packet_count = 0;

    /* 
     * This while loop is for receiving the zero-packet train.
     * When receiving the first random packet, breaks the loop.
     */
    while (1) 
    {
        ssize_t recv_size = recvfrom(sock, buffer, packet_size, 0, (struct sockaddr*)client_addr_ptr, &addr_len);
        if (recv_size > 0)
        {
            packet_count++;
            /* Checks the first 10 bytes (skip the first 2 bytes since it is for IDs) is 0x0 */
            if (memcmp(buffer + 2, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 10) != 0)
            {
                //printf("Received the first packet of the random-packet train.\n");
                //printf("Received %d packets so far.\n", packet_count);
                first_packet_received_for_random_train = 1;
                gettimeofday(&first_packet_time_for_random_train, NULL);
                gettimeofday(&last_packet_time_for_random_train, NULL);
                break; // Breaks the loop, move on to the next while loop
            }

            gettimeofday(&last_packet_time_for_zero_train, NULL); // Always updates the time for last packet.
            if (!first_packet_received_for_zero_train) // If this is the first packet of the zero-packet train,
            {
                //printf("Received the first packet of the zero-packet train.\n");
                first_packet_received_for_zero_train = 1;
                first_packet_time_for_zero_train = last_packet_time_for_zero_train; // Sets the first_packet_time.
            }
        }
    }

     /* This while loop is for receiving the random-packet train. When the timeout reaches, it breaks the loop. */
    while (first_packet_received_for_random_train) 
    {
        ssize_t recv_size = recvfrom(sock, buffer, packet_size, 0, (struct sockaddr*)client_addr_ptr, &addr_len);
        if (recv_size > 0)
        {
            packet_count++;
            gettimeofday(&last_packet_time_for_random_train, NULL);
        }
        else if (recv_size == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            //printf("Socket timeout reached. Received %d packets in total.\n", packet_count);
            break;
        }
    }

    if (first_packet_received_for_zero_train && first_packet_received_for_random_train)
    {
        /* Need to convert the duration to millisecond */
        long duration_for_zero_train = (last_packet_time_for_zero_train.tv_sec - first_packet_time_for_zero_train.tv_sec) * 1000 + 
                                       (last_packet_time_for_zero_train.tv_usec - first_packet_time_for_zero_train.tv_usec) / 1000;
        long duration_for_random_train = (last_packet_time_for_random_train.tv_sec - first_packet_time_for_random_train.tv_sec) * 1000 + 
                                         (last_packet_time_for_random_train.tv_usec - first_packet_time_for_random_train.tv_usec) / 1000;
        return labs(duration_for_zero_train - duration_for_random_train);
    }
    else // When the socket receives NO packets
    {
        printf("No packets received within the timeout.\n");
        return -1;
    }
}

void send_result(const int server_port, const long result)
{
    struct socket_pair socks = open_client_tcp_socket(server_port);

    int bytes_sent = send(socks.client_sock, &result, sizeof(long), 0); // The result is data type long.
    if (bytes_sent < 0)
    {
        perror("send() failed.\n");
        close(socks.client_sock);
        close(socks.server_sock);
        exit(EXIT_FAILURE);
    }
    //printf("Sent result to the client.\n");
    close(socks.client_sock);
    close(socks.server_sock);
}