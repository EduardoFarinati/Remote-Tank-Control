#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "server.h"
#include "../comm.h"


int server_socket;
struct sockaddr_in client_address;

protocol_packet packet;


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
    char buffer[BUFFER_LENGTH+1];

    // Tries to receive a message
    if(receive_message(buffer, server_socket, &client_address) == 0) {
        // Tries parsing message
        if(is_packet_done(buffer)) {
            packet = parse_command(buffer);

            return 0;
        }
        else {
            return -1;
        }
    }
}

void command_action() {
    protocol_packet response_packet;
    char response[BUFFER_LENGTH];

    switch(packet.keyword) {
        case OPEN_VALVE:
            open_valve(packet.value);
            response_packet = (protocol_packet) {
                .keyword = OPEN_VALVE_RESPONSE,
                .value = packet.value
            };
            break;
        
        case CLOSE_VALVE:
            close_valve(packet.value);
            response_packet = (protocol_packet) {
                .keyword = CLOSE_VALVE_RESPONSE,
                .value = packet.value
            };
            break;

        case GET_LEVEL:
            response_packet = (protocol_packet) {
                .keyword = GET_LEVEL_RESPONSE,
                .value = get_level()
            };
            break;

        case COMM_TEST:
            response_packet = (protocol_packet) {
                .keyword = COMM_TEST_RESPONSE,
                .value = OK_VALUE
            };
            break;

        case SET_MAX:
            set_max(packet.value);
            response_packet = (protocol_packet) {
                .keyword = SET_MAX_RESPONSE,
                .value = packet.value
            };
            break;

        case START:
            start_tank();
            response_packet = (protocol_packet) {
                .keyword = START_RESPONSE,
                .value = OK_VALUE
            };
            break;

        default:
            response_packet = (protocol_packet) {
                .keyword = ERROR_RESPONSE,
                .value = NO_VALUE
            };
            break;
    }

    // Sends formatted message to client
    format_message(response, response_packet);
    send_message(response, server_socket, &client_address);
}

void close_server_socket() {
    close_socket(server_socket);
}