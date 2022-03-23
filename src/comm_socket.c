#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "comm.h"


void print_received_bytes(char* _str, int count) {
    int i;

    printf("'");
    for(i = 0; i < count-1; i++) {
        printf("%d, ", _str[i]);
    }
    printf("%d'\n", _str[i]);
}

int create_socket() {
    int socket_id;

    printf("Opening udp/ip socket...\n");
    socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(socket_id < 0) {
        printf("Error: Unable to open udp/ip socket, return code: %d\n", socket_id);

        return -1;
    }

    return socket_id;
}

void close_socket(int socket_id) {
    printf("Closing udp/ip socket id %d...\n", socket_id);
    
    if(close(socket_id) < 0) {
        printf("Error: Unable to close udp/ip socket!\n");
        exit(EXIT_FAILURE);
    }
}


int bind_port(int socket_id, int port) {
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),  // From any ip
        .sin_port = htons(port)
    };

    printf("Binding port %d...\n", port);
    if(bind(socket_id, (struct sockaddr *) &address, sizeof(address)) < 0) {
      printf("Error: Unable to bind port!\n");

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
        printf("Received message from socket %d: \n", socket_id);
        print_received_bytes(message, count);

        return 0;
    }
    else {
        printf("Error - receive from socket failed, error: %s\n", strerror(errno));

        return -1;
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
    printf("Sending message \"%s\" to socket id %d...\n", buffer, socket_id);
    if(sendto(socket_id, buffer, BUFFER_LENGTH, 0, (struct sockaddr *) to_address, to_address_len) < 0) {
        printf("Error: send to socket failed, error: %s\n", strerror(errno));

        return -1;
    }
    else {
        return 0;
    }
}
