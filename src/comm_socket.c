#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "comm.h"
#include "debug.h"


void print_received_bytes(char* _str, int count) {
    int i;

    write_log(INFO, "'");
    for(i = 0; i < count-1; i++) {
        write_log(INFO, "%d, ", _str[i]);
    }
    write_log(INFO, "%d'\n", _str[i]);
}

int create_socket() {
    int socket_id;

    write_log(CRITICAL, "Opening udp/ip socket...\n");
    socket_id = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);

    if(socket_id < 0) {
        write_log(CRITICAL, "Error: Unable to open udp/ip socket, return code: %d\n", socket_id);

        return -1;
    }

    return socket_id;
}

void close_socket(int socket_id) {
    write_log(CRITICAL, "Closing udp/ip socket id %d...\n", socket_id);
    
    if(close(socket_id) < 0) {
        write_log(CRITICAL, "Error: Unable to close udp/ip socket!\n");
        exit(EXIT_FAILURE);
    }
}


int bind_port(int socket_id, int port) {
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),  // From any ip
        .sin_port = htons(port)
    };

    write_log(CRITICAL, "Binding port %d...\n", port);
    if(bind(socket_id, (struct sockaddr *) &address, sizeof(address)) < 0) {
      write_log(CRITICAL, "Error: Unable to bind port!\n");

      return -1;
    }
    else {
        return 0;
    }
}


int receive_message(char* message, int socket_id, struct sockaddr_in* from_address) {
    unsigned int to_address_len = sizeof(*from_address);

    int count = recvfrom(
        socket_id,
        message,
        BUFFER_LENGTH,
        0,
        (struct sockaddr *) from_address,
        &to_address_len
    );

    if(count > 0) {
        // Set as string
        message[count] = '\0';
        write_log(INFO, "Received message from socket %d: \n", socket_id);
        print_received_bytes(message, count);

        // Clear socket if there's still any data
        char dummy[BUFFER_LENGTH];
        while(count > 0) {
            count = recvfrom(socket_id, dummy, BUFFER_LENGTH, 0, (struct sockaddr *) from_address, &to_address_len);
        }

        return 0;
    }
    else {
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data in the socket
            message[0] = '\0';

            return 0;
        }
        else {
            write_log(CRITICAL, "Error - receive from socket failed, error: %s\n", strerror(errno));
            return -1;
        }
    }
}

int send_message(char* message, int socket_id, struct sockaddr_in* to_address) {
    unsigned int to_address_len = sizeof(*to_address);
    char buffer[BUFFER_LENGTH];
    int i;
    
    // Clears buffer
    for(i = 0; i < BUFFER_LENGTH; i++) {
        buffer[i] = 0;
    }

    // Copies message to buffer
    for(i = 0; (i < BUFFER_LENGTH) && (message[i] != '\0'); i++) {
        buffer[i] = message[i];
    }

    // Send buffer
    write_log(INFO, "Sending message \"%s\" to socket id %d...\n", buffer, socket_id);
    if(sendto(socket_id, buffer, BUFFER_LENGTH, 0, (struct sockaddr *) to_address, to_address_len) < 0) {
        write_log(CRITICAL, "Error: send to socket failed, error: %s\n", strerror(errno));

        return -1;
    }
    else {
        return 0;
    }
}
