#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "client.h"
#include "connection_sync.h"
#include "../comm.h"
#include "../time.h"
#include "../debug.h"
#include "../graphics.h"


int client_socket;
struct sockaddr_in server_address;

char buffer[BUFFER_LENGTH+1];
protocol_packet command_packet, response_packet, expected_packet;

int waiting_answer;

int received_valve_opening_with_wrong_value = 0;

int restart_server_flag = 1;

typedef enum tank_update_state_t {
    TESTING_COMM,
    STARTING_TANK,
    GETTING_LEVEL,
    SETTING_INPUT_VALVE,
} tank_update_state;

tank_update_state update_state = TESTING_COMM;

// Time when last message was sent
struct timespec sent_time;

// Tank when tried to update input valve 
TankState tank_before;


int start_client_socket(int port) {
    client_socket = create_socket();
    if(client_socket) {
        if (port == ANY_PORT) {
            return 0;
        }
        else {
            if(bind_port(client_socket, port)) {
                return 0;
            }
            else {
                return -1;
            }
        }
    }
    else {
        return -1;
    }
}

void set_server_address(char* server_ip_address, int server_port) {
    server_address = (struct sockaddr_in) {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(server_ip_address),
        .sin_port = htons(server_port)
    };
    
    // Maybe set a return value for invalid addresses?
    write_log(CRITICAL, "Server address %s:%d set...\n", server_ip_address, server_port);
}


int send_command() {
    char command[BUFFER_LENGTH];

    // Tries to send a message
    format_message(command, command_packet);

    if(send_message(command, client_socket, &server_address)) {
        return -1;
    }
    else {
        // Send OK
        sent_time = get_current_time();

        return 0;
    }
}

int check_valid_response() {
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

    if(response_packet.value == expected_packet.value) {
        if(response_packet.keyword == expected_packet.keyword) {
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        if(response_packet.keyword == OPEN_VALVE_RESPONSE || response_packet.keyword == CLOSE_VALVE_RESPONSE) {
            received_valve_opening_with_wrong_value = 1;
        }

        return -1;
    }
}


void set_valve_packet_setup() {
    TankState tank = get_tank();

    if(tank.delta >= 0) {
        command_packet = (protocol_packet) {
            .keyword = OPEN_VALVE,
            .value = tank.delta
        };
    }
    else {
        command_packet = (protocol_packet) {
            .keyword = CLOSE_VALVE,
            .value = -tank.delta
        };
    }

    // Sets tank to compare when message
    // is received
    tank_before = tank;
}

void treat_answer() {
    TankState tank;

    if(check_valid_response() == 0) {
        switch (update_state) {
            case TESTING_COMM:
                connection_estabilished();
                if(restart_server_flag) {
                    update_state = STARTING_TANK;
                }
                else {
                    update_state = GETTING_LEVEL;
                }
                break;

            case STARTING_TANK:
                reset_time();
                set_recreate_graph();
                update_state = GETTING_LEVEL;
                break;

            case GETTING_LEVEL:
                lock_tank_state(&tank);
                tank.level = response_packet.value;
                unlock_tank_state(&tank);
                update_state = SETTING_INPUT_VALVE;
                break;

            case SETTING_INPUT_VALVE:
                lock_tank_state(&tank);
                tank.input += tank_before.delta;
                tank.delta -= tank_before.delta;
                unlock_tank_state(&tank);
                update_state = GETTING_LEVEL;
                break;
            
            default:
                break;
        }
    }
    else {
        if(response_packet.keyword == ERROR_RESPONSE) {
            // Packet dropped by server
            // Try sending again
        }
        else if(is_packet_similar(buffer, expected_packet) || received_valve_opening_with_wrong_value) {
            // Packet may have been received
            if(update_state == SETTING_INPUT_VALVE) {
                if(command_packet.keyword == OPEN_VALVE) {
                    set_max_min_opening(command_packet.value);
                }
                else if(command_packet.keyword == CLOSE_VALVE) {
                    set_max_min_opening(-command_packet.value);
                }

                // Clear flag
                received_valve_opening_with_wrong_value = 0;
            }
        }
        else {
            // Assume may have been OK, adapt state machine
            switch (update_state) {
                case TESTING_COMM:
                    break;
                
                case STARTING_TANK:
                    reset_time();
                    set_recreate_graph();
                    update_state = GETTING_LEVEL;
                    break;

                case GETTING_LEVEL:
                    update_state = SETTING_INPUT_VALVE;
                    break;

                case SETTING_INPUT_VALVE:
                    lock_tank_state(&tank);
                    tank.input += tank_before.delta;
                    tank.delta -= tank_before.delta;
                    unlock_tank_state(&tank);
                    update_state = GETTING_LEVEL;
                    break;
                
                default:
                    break;
            }
        }
    }
}

void check_answer() {
    // Tries to receive the response
    if(receive_message(buffer, client_socket, &server_address) == 0) {
        // Tries parsing message
        if(has_received_datagram(buffer) || (get_time_delta(sent_time) > NO_ANSWER_TIMEOUT)) {
            response_packet = parse_response(buffer);
            waiting_answer = 0;
            treat_answer();
        }
    }
}

void update_tank() {
    if (!waiting_answer) {
        switch (update_state) {
            case TESTING_COMM:
                command_packet = (protocol_packet) {
                    .keyword = COMM_TEST,
                    .value = NO_VALUE
                };
                break;

            case STARTING_TANK:
                command_packet = (protocol_packet) {
                    .keyword = START,
                    .value = NO_VALUE
                };
                break;

            case GETTING_LEVEL:
                command_packet = (protocol_packet) {
                    .keyword = GET_LEVEL,
                    .value = NO_VALUE
                };
                break;

            case SETTING_INPUT_VALVE:
                set_valve_packet_setup();
                break;

            default:
                break;
        }

        // Tries to send
        if(send_command() == 0) {
            waiting_answer = 1;
        }
    }
    else {
        check_answer();
    }
}

void close_client_socket() {
    close_socket(client_socket);
}