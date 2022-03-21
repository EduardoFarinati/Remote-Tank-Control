#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comm.h"


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

int get_value(protocol_keyword keyword, int* value, char* value_str) {
    switch (keyword) {
        // Messages with integer values
        case OPEN_VALVE:
        case OPEN_VALVE_RESPONSE:
        case CLOSE_VALVE:
        case CLOSE_VALVE_RESPONSE:
        case GET_LEVEL_RESPONSE:
        case SET_MAX:
        case SET_MAX_RESPONSE:
            *value = atoi(value_str);

            // Checks for valid values
            if((0 <= *value) && (*value <= 100)) {
                return 0;
            }
            else {
                return -1;
            }

        // Messages with string values
        case COMM_TEST_RESPONSE:
        case START_RESPONSE:
            *value = 0;
            if(strcmp(value_str, OK_COMPLEMENT) == 0) {
                return 0;
            }
            else {
                return -1;
            }

        // Messages with no values
        case GET_LEVEL:
        case COMM_TEST:
        case START:
        case ERROR_RESPONSE:
            *value = 0;
            if(strcmp(value_str, "") == 0) {
                return 0;
            }
            else {
                return -1;
            }

        // Error
        default:
            *value = 0;
            return -1;
    }
}

void get_keyword_value(char* message, char* keyword_str, char* value_str) {
    int tokens_found;
    int correct_format = 0;

    if(strstr(message, MESSAGE_TOKEN)) {
        if(strstr(message, KEYWORD_TOKEN)) {
            // Message type "KEYWORD#VALUE!"
            tokens_found = sscanf(
                message,
                "%[^" KEYWORD_TOKEN "]" KEYWORD_TOKEN "%[^" MESSAGE_TOKEN "]" MESSAGE_TOKEN,
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
                message, "%[^" MESSAGE_TOKEN "]" MESSAGE_TOKEN,
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

void parse_command(char* message, protocol_keyword* keyword, int* value) {
    char keyword_str[KEYWORD_MAX_LENGTH+1], value_str[VALUE_MAX_LENGTH+1];

    // Get keyword and value as strings
    get_keyword_value(message, keyword_str, value_str);

    // Parse message and sets errors
    *keyword = get_command_keyword(keyword_str);
    if(*keyword != ERROR) {
        if(get_value(*keyword, value, value_str) != 0) {
            *keyword = ERROR;
        }
    }
}

void parse_response(char* message, protocol_keyword* keyword, int* value) {
    char keyword_str[KEYWORD_MAX_LENGTH+1], value_str[VALUE_MAX_LENGTH+1];

    // Get keyword and value as strings
    get_keyword_value(message, keyword_str, value_str);

    // Parse message and sets errors
    *keyword = get_response_keyword(keyword_str);
    if(*keyword != ERROR) {
        if(get_value(*keyword, value, value_str) != 0) {
            *keyword = ERROR;
        }
    }
}

void format_message_with_integer_value(char* message, char* keyword_str, int value) {
    // Formats string as specified for communication
    snprintf(message, BUFFER_LENGTH, "%s"KEYWORD_TOKEN"%d"MESSAGE_TOKEN, keyword_str, value);
    // BUFFER LENGTH ensures string is long enough
}

void format_message_with_str_value(char* message, char* keyword_str, char* value) {
    // Formats string as specified for communication
    snprintf(message, BUFFER_LENGTH, "%s"KEYWORD_TOKEN"%s"MESSAGE_TOKEN, keyword_str, value);
    // BUFFER LENGTH ensures string is long enough
}

void format_message_with_no_value(char* message, char* keyword_str) {
    // Formats string as specified for communication
    snprintf(message, BUFFER_LENGTH, "%s"MESSAGE_TOKEN, keyword_str);
    // BUFFER LENGTH ensures string is long enough
}

void format_message(char* message, protocol_keyword keyword, int value) {
    switch(keyword) {
        // Messages with integer values
        case OPEN_VALVE:
            format_message_with_integer_value(message, OPEN_VALVE_STR, value);
            break;

        case OPEN_VALVE_RESPONSE:
            format_message_with_integer_value(message, OPEN_VALVE_RESPONSE_STR, value);
            break;

        case CLOSE_VALVE:
            format_message_with_integer_value(message, CLOSE_VALVE_STR, value);
            break;

        case CLOSE_VALVE_RESPONSE:
            format_message_with_integer_value(message, CLOSE_VALVE_RESPONSE_STR, value);
            break;

        case GET_LEVEL_RESPONSE:
            format_message_with_integer_value(message, GET_LEVEL_RESPONSE_STR, value);
            break;

        case SET_MAX:
            format_message_with_integer_value(message, SET_MAX_STR, value);
            break;

        case SET_MAX_RESPONSE:
            format_message_with_integer_value(message, SET_MAX_RESPONSE_STR, value);
            break;

        // Messages with string values
        case COMM_TEST_RESPONSE:
            format_message_with_str_value(message, COMM_TEST_RESPONSE_STR, OK_COMPLEMENT);
            break;

        case START_RESPONSE:
            format_message_with_str_value(message, START_STR, OK_COMPLEMENT);
            break;

        // Messages with no values
        case GET_LEVEL:
            format_message_with_no_value(message, GET_LEVEL_STR);
            break;

        case COMM_TEST:
            format_message_with_no_value(message, COMM_TEST_STR);
            break;

        case START:
            format_message_with_no_value(message, START_STR);
            break;

        case ERROR_RESPONSE:
            format_message_with_no_value(message, ERROR_RESPONSE_STR);
            break;

        // Shouldnt be reached
        case ERROR:
            format_message_with_no_value(message, ERROR_RESPONSE_STR);
            break;
    }
}
