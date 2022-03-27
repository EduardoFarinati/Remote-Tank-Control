#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "client.h"
#include "../comm.h"
#include "../debug.h"


int client_socket;
struct sockaddr_in server_address;

char buffer[BUFFER_LENGTH+1];
protocol_packet command_packet, response_packet;


int start_client_socket() {
    client_socket = create_socket();
    if(client_socket) {
        return 0;
    }
    else {
        return -1;
    }
}

void set_server_address_p(char* server_ip_address, int server_port) {
    server_address = (struct sockaddr_in) {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(server_ip_address),
        .sin_port = htons(server_port)
    };
    
    // Maybe set a return value for invalid addresses?
    write_log(CRITICAL, "Server address %s:%d set...\n", server_ip_address, server_port);
}

void set_server_address(char* server_ip_address) {
    set_server_address_p(server_ip_address, DEFAULT_PORT);
}

int send_command() {
    char command[BUFFER_LENGTH];

    // Tries to send a message
    format_message(command, command_packet);

    if(send_message(command, client_socket, &server_address)) {
        return -1;
    }
    else {
        // Send OK, receive response
        return receive_message(buffer, client_socket, &server_address);
    }
}

int check_response() {
    protocol_packet expected_packet;

    response_packet = parse_response(buffer);

    switch(command_packet.keyword) {
        case OPEN_VALVE:
            expected_packet = (protocol_packet) {
                .keyword = OPEN_VALVE_RESPONSE,
                .value = command_packet.value
            };
            break;
        
        case CLOSE_VALVE:
            expected_packet = (protocol_packet) {
                .keyword = CLOSE_VALVE_RESPONSE,
                .value = command_packet.value
            };
            break;

        case GET_LEVEL:
            expected_packet = (protocol_packet) {
                .keyword = GET_LEVEL_RESPONSE,
                .value = response_packet.value // Value must only be acceptable
            };
            break;

        case COMM_TEST:
            expected_packet = (protocol_packet) {
                .keyword = COMM_TEST_RESPONSE,
                .value = OK_VALUE
            };
            break;

        case SET_MAX:
            expected_packet = (protocol_packet) {
                .keyword = SET_MAX_RESPONSE,
                .value = command_packet.value
            };
            break;

        case START:
            expected_packet = (protocol_packet) {
                .keyword = START_RESPONSE,
                .value = OK_VALUE
            };
            break;

        default:
            // Shouldnt be reached
            expected_packet = (protocol_packet) {
                .keyword = ERROR_RESPONSE,
                .value = NO_VALUE
            };
            break;
    }

    if((response_packet.keyword == expected_packet.keyword)
        && (response_packet.value == expected_packet.value)) {
        return 0;
    }
    else {
        return -1;
    }
}

int comm_test() {
    command_packet = (protocol_packet) {
        .keyword = COMM_TEST,
        .value = NO_VALUE
    };

    if(!send_command() && !check_response()) {
        return 0;
    }
    else {
        return -1;
    }
}

int start_tank() {
    command_packet = (protocol_packet) {
        .keyword = START,
        .value = NO_VALUE
    };

    if(!send_command() && !check_response()) {
        return 0;
    }
    else {
        return -1;
    }
}

int get_level(TankState* tank) {
    command_packet = (protocol_packet) {
        .keyword = GET_LEVEL,
        .value = NO_VALUE
    };

    if(!send_command() && !check_response()) {
        tank->level = response_packet.value;

        return 0;
    }
    else {
        return -1;
    }
}

int set_input_valve(TankState* tank) {
    // Calculates control input
    int r = 80; // Reference
    int y = tank->level;
    int u_p = tank->input;
    int u = controller_output(r, y);

    // Calculates variation
    int delta = u - u_p;

    if(delta >= 0) {
        command_packet = (protocol_packet) {
            .keyword = OPEN_VALVE,
            .value = delta
        };
    }
    else {
        command_packet = (protocol_packet) {
            .keyword = CLOSE_VALVE,
            .value = -delta
        };
    }

    tank->input += delta;
    if(!send_command() && !check_response()) {

        return 0;
    }
    else {
        return -1;
    }
}

void set_time(TankState* tank) {
    double dt = CONTROL_SLEEP_MS * 1e-3; 

    tank->t += dt;
}

void update_tank() {
    TankState tank = get_tank();

    // New values
    get_level(&tank);
    set_input_valve(&tank);
    set_time(&tank);

    // Update tank state
    set_tank(tank);
}

void close_client_socket() {
    close_socket(client_socket);
}