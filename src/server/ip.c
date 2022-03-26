#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "server.h"
#include "../comm.h"


int server_socket;
struct sockaddr_in client_address;

char command[BUFFER_LENGTH];
protocol_keyword keyword;
int value;


int start_server_socket_p(int port) {
    server_socket = create_socket();
    if(server_socket) {
        return bind_port(server_socket, port);
    }
    else {
        return -1;
    }
}

int start_server_socket() {
    return start_server_socket_p(DEFAULT_PORT);
}

int receive_command() {
    // Tries to receive a message
    int return_code = receive_message(command, server_socket, &client_address);

    if(return_code == 0) {
        // Tries parsing message
        parse_command(command, &keyword, &value);
    }

    return return_code;
}

void command_action() {
    protocol_keyword response_keyword;
    char response[BUFFER_LENGTH];

    switch(keyword) {
        case OPEN_VALVE:
            open_valve(value);
            response_keyword = OPEN_VALVE_RESPONSE;
            break;
        
        case CLOSE_VALVE:
            close_valve(value);
            response_keyword = CLOSE_VALVE_RESPONSE;
            break;

        case GET_LEVEL:
            value = get_level();
            response_keyword = GET_LEVEL_RESPONSE;
            break;

        case COMM_TEST:
            response_keyword = COMM_TEST_RESPONSE;
            break;

        case SET_MAX:
            set_max(value);
            response_keyword = SET_MAX_RESPONSE;
            break;

        case START:
            start_tank();
            response_keyword = START_RESPONSE;
            break;

        default:
            response_keyword = ERROR_RESPONSE;
            break;
    }

    // Sends formatted message to client
    format_message(response, response_keyword, value);
    send_message(response, server_socket, &client_address);
}

void close_server_socket() {
    close_socket(server_socket);
}