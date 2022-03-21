#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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

    printf("Opening tcp/ip socket...\n");
    socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socket_id < 0) {
        printf("Error: Unable to open tcp/ip socket, return code: %d\n", socket_id);

        return -1;
    }

    return socket_id;
}

void close_socket(int socket_id) {
    printf("Closing tcp/ip socket id:%d...\n", socket_id);
    
    if(close(socket_id) < 0) {
        printf("Error: Unable to close tcp/ip socket!\n");
        exit(EXIT_FAILURE);
    }
}


int set_passive_socket(int socket_id, int port, int max_connections) {
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),  // From any ip
        .sin_port = htons(port)
    };

    // Tries to bind the socket
    printf("Binding port %d...\n", port);
    if(bind(socket_id, (struct sockaddr *) &address, sizeof(address)) < 0) {
      printf("Error: Unable to bind port!\n");

      return -1;
    }
    
    // Set as listening port
    printf("Set as listening port %d...\n", port);
    if(listen(socket_id, max_connections) < 0) {
      printf("Error: Unable to set as listening port!\n");

      return -1;
    }
}


int receive_message(char* message, int socket_id) {
    int count = recv(socket_id, message, BUFFER_LENGTH, 0);

    if(count > 0) {
        // Set as string
        message[count] = '\0';
        printf("Received message from socket %d: \n", socket_id);
        print_received_bytes(message, count);

        return 0;
    }
    else if(count == 0) {
        // Socket closed
        printf("Connection to socket %d closed.\n", socket_id);

        return 1;
    }
    else {
        printf("Error - receive from socket %d failed, error: %s\n", socket_id, strerror(errno));

        return -1;
    }
}

int send_message(char *message, int socket_id) {
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
    if(send(socket_id, buffer, BUFFER_LENGTH, 0) < 0) {
        printf("Error: failed to send message!\n");

        return -1;
    }
    else {
        return 0;
    }
}
