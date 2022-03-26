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
    format_message(command, keyword, value);

    if(send_message(command, client_socket, &server_address)) {
        return -1;
    }
    else {
        // Send OK, receive response
        return receive_message(response, client_socket, &server_address);
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