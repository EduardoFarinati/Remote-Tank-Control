// Convert int macro to string macro
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comm.h"
#include "time.h"


// Dealing with incomplete packets
char incomplete[BUFFER_LENGTH+1] = "\0";
int was_complete = 1;
struct timespec started_receiving;


protocol_keyword get_command_keyword(char* keyword_str) {
    // Returns a command keyword constant represented by keyword_str 
    if(strcmp(keyword_str, OPEN_VALVE_STR) == 0) {
        return OPEN_VALVE;
    }
    else if(strcmp(keyword_str, CLOSE_VALVE_STR) == 0) {
        return CLOSE_VALVE;
    }
    else if(strcmp(keyword_str, GET_LEVEL_STR) == 0) {
        return GET_LEVEL;
    }
    else if(strcmp(keyword_str, COMM_TEST_STR) == 0) {
        return COMM_TEST;
    }
    else if(strcmp(keyword_str, SET_MAX_STR) == 0) {
        return SET_MAX;
    }
    else if(strcmp(keyword_str, START_STR) == 0) {
        return START;
    }
    // Invalid
    else {
        return ERROR;
    }
}

protocol_keyword get_response_keyword(char* keyword_str) {
    // Returns a response keyword constant represented by keyword_str 
    if(strcmp(keyword_str, OPEN_VALVE_RESPONSE_STR) == 0) {
        return OPEN_VALVE_RESPONSE;
    }
    else if(strcmp(keyword_str, CLOSE_VALVE_RESPONSE_STR) == 0) {
        return CLOSE_VALVE_RESPONSE;
    }
    else if(strcmp(keyword_str, GET_LEVEL_RESPONSE_STR) == 0) {
        return GET_LEVEL_RESPONSE;
    }
    else if(strcmp(keyword_str, COMM_TEST_RESPONSE_STR) == 0) {
        return COMM_TEST_RESPONSE;
    }
    else if(strcmp(keyword_str, SET_MAX_RESPONSE_STR) == 0) {
        return SET_MAX_RESPONSE;
    }
    else if(strcmp(keyword_str, START_RESPONSE_STR) == 0) {
        return START_RESPONSE;
    }
    else if(strcmp(keyword_str, ERROR_RESPONSE_STR) == 0){
        return ERROR_RESPONSE;
    }
    // Invalid
    else {
        return ERROR;
    }
}

protocol_value get_value(char* value_str) {
    // Tries to parse as an unsigned integer
    protocol_value value = atoi(value_str);

    if(value < 0) {
        value = INVALID_VALUE;
    }
    else if(value == 0 && (strcmp(value_str, "0") != 0)) {
        // Input is not an int
        if(strcmp(value_str, OK_COMPLEMENT) == 0) {
            value = OK_VALUE;
        }
        else if(strcmp(value_str, "") == 0) {
            value = NO_VALUE;
        }
    }

    return value;
}


void get_keyword_value(char* message, char* keyword_str, char* value_str) {
    int tokens_found;
    int correct_format = 0;

    if(strstr(message, MESSAGE_TOKEN)) {
        if(strstr(message, KEYWORD_TOKEN)) {
            // Message type "KEYWORD#VALUE!"
            tokens_found = sscanf(
                message,
                "%" STR(KEYWORD_MAX_LENGTH) "[^" KEYWORD_TOKEN "]" KEYWORD_TOKEN "%" STR(VALUE_MAX_LENGTH) "[^" MESSAGE_TOKEN "]" MESSAGE_TOKEN,
                keyword_str,
                value_str
            );
            if(tokens_found == 2) {
                correct_format = 1;
            }
        }
        else {
            // Message type "KEYWORD!"
            tokens_found = sscanf(
                message, "%" STR(KEYWORD_MAX_LENGTH) "[^" MESSAGE_TOKEN "]" MESSAGE_TOKEN,
                keyword_str
            );
            value_str[0] = '\0';

            if(tokens_found == 1) {
                correct_format = 1;
            }
        }
    }

    if(!correct_format) {
        // Error values
        keyword_str[0] = '\0';
        value_str[0] = '\0';
    }
}

int is_packet_done(char* message) {
    if (!was_complete) {
        if(get_time_delta(started_receiving) > MESSAGE_TIMEOUT_S) {
            was_complete = 1;

            return 1;
        }
        else {
            int is_big_enough = (strlen(message) + strlen(incomplete) < BUFFER_LENGTH);
            if(is_big_enough) {
                strcat(incomplete, message);
            }
            else {
                strcpy(incomplete, message);
                started_receiving = get_current_time();
            }
        }
    }

    // Waits for "!" at the end
    if(strstr(message, MESSAGE_TOKEN)) {
        if(!was_complete) {
            strcpy(message, incomplete);
            was_complete = 1;
        }

        return 1;
    }
    else {
        if(was_complete && (strcmp(message, "") != 0)) {
            strcpy(incomplete, message);
            started_receiving = get_current_time();
            was_complete = 0;
        }

        return 0;
    }
}

int is_packet_valid(protocol_packet packet) {
    switch(packet.keyword) {
        // Keywords with int values
        case OPEN_VALVE:
        case OPEN_VALVE_RESPONSE:
        case CLOSE_VALVE:
        case CLOSE_VALVE_RESPONSE:
        case GET_LEVEL_RESPONSE:
        case SET_MAX:
        case SET_MAX_RESPONSE:
            if((MIN_VALUE <= packet.value) && (packet.value <= MAX_VALUE)) {
                return 1;
            }
            else {
                return 0;
            }

        // Keywords with no values
        case GET_LEVEL:
        case COMM_TEST:
        case START:
        case ERROR_RESPONSE:
            if(packet.value == NO_VALUE) {
                return 1;
            }
            else {
                return 0;
            }

        // Keywords with OK values
        case COMM_TEST_RESPONSE:
        case START_RESPONSE:
            if(packet.value == OK_VALUE) {
                return 1;
            }
            else {
                return 0;
            }

        // Error on formatting
        case ERROR:
        default:
            return 0;
    }
}

protocol_packet parse_command(char* message) {
    protocol_packet packet;
    char keyword_str[KEYWORD_MAX_LENGTH+1], value_str[VALUE_MAX_LENGTH+1];

    // Get keyword and value as strings
    get_keyword_value(message, keyword_str, value_str);

    // Parse packet
    packet = (protocol_packet) {
        .keyword = get_command_keyword(keyword_str),
        .value = get_value(value_str)
    };

    if(is_packet_valid(packet)) {
        return packet;
    }
    else {
        return (protocol_packet) { 
            .keyword = ERROR, 
            .value = INVALID_VALUE
        };
    }
}

protocol_packet parse_response(char* message) {
    protocol_packet packet;
    char keyword_str[KEYWORD_MAX_LENGTH+1], value_str[VALUE_MAX_LENGTH+1];

    // Get keyword and value as strings
    get_keyword_value(message, keyword_str, value_str);

    // Parse packet
    packet = (protocol_packet) {
        .keyword = get_response_keyword(keyword_str),
        .value = get_value(value_str)
    };

    if(is_packet_valid(packet)) {
        return packet;
    }
    else {
        return (protocol_packet) { 
            .keyword = ERROR, 
            .value = INVALID_VALUE
        };
    }
}

void get_keyword_str(protocol_keyword keyword, char* keyword_str) {
    switch (keyword) {
        case OPEN_VALVE:
            strcpy(keyword_str, OPEN_VALVE_STR);
            break;

        case OPEN_VALVE_RESPONSE:
            strcpy(keyword_str, OPEN_VALVE_RESPONSE_STR);
            break;

        case CLOSE_VALVE:
            strcpy(keyword_str, CLOSE_VALVE_STR);
            break;

        case CLOSE_VALVE_RESPONSE:
            strcpy(keyword_str, CLOSE_VALVE_RESPONSE_STR);
            break;

        case GET_LEVEL:
            strcpy(keyword_str, GET_LEVEL_STR);
            break;

        case GET_LEVEL_RESPONSE:
            strcpy(keyword_str, GET_LEVEL_RESPONSE_STR);
            break;

        case COMM_TEST:
            strcpy(keyword_str, COMM_TEST_STR);
            break;

        case COMM_TEST_RESPONSE:
            strcpy(keyword_str, COMM_TEST_RESPONSE_STR);
            break;

        case SET_MAX:
            strcpy(keyword_str, SET_MAX_STR);
            break;

        case SET_MAX_RESPONSE:
            strcpy(keyword_str, SET_MAX_RESPONSE_STR);
            break;

        case START:
            strcpy(keyword_str, START_STR);
            break;

        case START_RESPONSE:
            strcpy(keyword_str, START_RESPONSE_STR);
            break;

        case ERROR_RESPONSE:
            strcpy(keyword_str, ERROR_RESPONSE_STR);
            break;
        
        case ERROR:
        default:
            // Shouldnt be reached
            strcpy(keyword_str, ERROR_RESPONSE_STR);
            break;
    }
}

void format_message(char* message, protocol_packet packet) {
    // Formats string as specified for communication
    char keyword_str[KEYWORD_MAX_LENGTH+1];
    get_keyword_str(packet.keyword, keyword_str);

    // BUFFER LENGTH ensures string is long enough
    if(packet.value == OK_VALUE) {
        // "OK" value
        snprintf(message, BUFFER_LENGTH, "%s"KEYWORD_TOKEN"%s"MESSAGE_TOKEN, keyword_str, OK_COMPLEMENT);
    }
    else if(packet.value == NO_VALUE) {
        // "" value
        snprintf(message, BUFFER_LENGTH, "%s"MESSAGE_TOKEN, keyword_str);
    }
    else {
        // 0-100 value
        snprintf(message, BUFFER_LENGTH, "%s"KEYWORD_TOKEN"%d"MESSAGE_TOKEN, keyword_str, packet.value);
    }
}
