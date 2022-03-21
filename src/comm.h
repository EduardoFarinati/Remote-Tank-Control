#ifndef COMM_H
#define COMM_H

// IP client / server constants
#define DEFAULT_PORT 8201
#define MAX_CONNECTIONS 3  // Maximum requests for connections on the server

// Communication constants
typedef enum protocol_keyword_t {
    OPEN_VALVE,
    OPEN_VALVE_RESPONSE,
    CLOSE_VALVE,
    CLOSE_VALVE_RESPONSE,
    GET_LEVEL,
    GET_LEVEL_RESPONSE,
    COMM_TEST,
    COMM_TEST_RESPONSE,
    SET_MAX,
    SET_MAX_RESPONSE,
    START,
    START_RESPONSE,
    ERROR,
    ERROR_RESPONSE
} protocol_keyword;

#define OPEN_VALVE_STR "OpenValve"
#define OPEN_VALVE_RESPONSE_STR "Open"
#define CLOSE_VALVE_STR "CloseValve"
#define CLOSE_VALVE_RESPONSE_STR "Close"
#define GET_LEVEL_STR "GetLevel"
#define GET_LEVEL_RESPONSE_STR "Level"
#define COMM_TEST_STR "CommTest"
#define COMM_TEST_RESPONSE_STR "Comm"
#define SET_MAX_STR "SetMax"
#define SET_MAX_RESPONSE_STR "Max"
#define START_STR "Start"
#define START_RESPONSE_STR "Start"

#define ERROR_RESPONSE_STR "Err"

#define OK_COMPLEMENT "OK"

#define KEYWORD_TOKEN "#"
#define MESSAGE_TOKEN "!"

#define KEYWORD_MAX_LENGTH 10
#define VALUE_MAX_LENGTH 3
#define BUFFER_LENGTH 16


// Communication function headers
int create_socket();

int set_passive_socket(int socket_id, int port, int max_connections);

int send_message(char *message, int socket_id);

int receive_message(char *message, int socket_id);

void parse_command(char* message, protocol_keyword* keyword, int* value);

void parse_response(char* message, protocol_keyword* keyword, int* value);

void format_message(char* message, protocol_keyword keyword, int value);


#endif