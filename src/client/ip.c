#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "../comm.h"
#include "client.h"


int client_socket;

protocol_keyword keyword;
int value;

char response[BUFFER_LENGTH];


int start_client_socket() {
    client_socket = create_socket();
    if(client_socket) {
        return 0;
    }
    else {
        return -1;
    }
}

int connect_to_server_p(char* server_ip_address, int server_port) {
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(server_ip_address),
        .sin_port = htons(server_port)
    };
    
    printf("Connecting to server at %s:%d...\n", server_ip_address, server_port);
    if(connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printf("Error - failed to connect, error: %s\n", strerror(errno));

        return -1;
    }
    else {
        return 0;
    }
}

int connect_to_server(char* server_ip_address) {
    return connect_to_server_p(server_ip_address, DEFAULT_PORT);
}

int send_command() {
    char command[BUFFER_LENGTH];

    // Tries to send a message
    format_message(command, keyword, value);

    if(send_message(command, client_socket)) {
        return -1;
    }
    else {
        // Send OK, receive response
        return receive_message(response, client_socket);
    }
}

int check_response() {
    protocol_keyword response_keyword, expected_keyword;
    int response_value = 0, expected_value;

    parse_response(response, &response_keyword, &value);

    switch(keyword) {
        case OPEN_VALVE:
            expected_keyword = OPEN_VALVE_RESPONSE;
            expected_value = value;
            break;
        
        case CLOSE_VALVE:
            expected_keyword = CLOSE_VALVE_RESPONSE;
            expected_value = value;
            break;

        case GET_LEVEL:
            expected_keyword = GET_LEVEL_RESPONSE;
            expected_value = response_value;
            break;

        case COMM_TEST:
            expected_keyword = COMM_TEST_RESPONSE;
            expected_value = response_value;
            break;

        case SET_MAX:
            expected_keyword = SET_MAX_RESPONSE;
            expected_value = value;
            break;

        case START:
            expected_keyword = START_RESPONSE;
            expected_value = response_value;
            break;

        default:
            expected_keyword = ERROR_RESPONSE;
            expected_value = response_value;
            break;
    }

    if((response_keyword == expected_keyword) && (response_value == expected_value)) {
        return 0;
    }
    else {
        return -1;
    }
}

int comm_test() {
    keyword = COMM_TEST;
    if(!send_command() && !check_response()) {
        return 0;
    }
    else {
        return -1;
    }
}

int start_tank() {
    keyword = START;
    if(!send_command() && !check_response()) {
        return 0;
    }
    else {
        return -1;
    }
}

int get_level(TankState* tank) {
    keyword = GET_LEVEL;
    if(!send_command() && !check_response()) {
        tank->level = value;

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
        keyword = OPEN_VALVE;
        value = delta;
    }
    else {
        keyword = CLOSE_VALVE;
        value = -delta;
    }

    if(!send_command() && !check_response()) {
        tank->input += delta;

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

int update_tank() {
    TankState previous_tank, tank;

    lock_tank_state(&previous_tank);

    // Only modify local variable, might fail
    // If no copy is used
    tank = previous_tank;
    set_time(&tank);

    if(!get_level(&tank) && !set_input_valve(&tank)) {
        unlock_tank_state(&tank);

        return 0;
    }
    else {
        // No variables modified
        unlock_tank_state(&previous_tank);

        return 1;
    }
}

void close_client_socket() {
    printf("Closing client tcp/ip socket...\n");
    close(client_socket);
}