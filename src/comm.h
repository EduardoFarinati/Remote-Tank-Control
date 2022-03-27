#ifndef COMM_H
#define COMM_H


#include <netinet/in.h>


// IP client / server constants
#define DEFAULT_PORT 9595  // Port for group E := 9E9E
#define MESSAGE_TIMEOUT_MS 50

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

typedef enum protocol_value_t {
    // Values from 0 to 100 are acceptable
    MIN_VALUE = 0,
    MAX_VALUE = 100,

    // Also some constants
    NO_VALUE = -1,
    OK_VALUE = -2,
    INVALID_VALUE = -3
} protocol_value;

typedef struct protocol_packet_t {
    protocol_keyword keyword;
    protocol_value value;
} protocol_packet;

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
// Socket function headers
int create_socket();

void close_socket(int socket_id);

int bind_port(int socket_id, int port);

int receive_message(char* message, int socket_id, struct sockaddr_in* from_address);

int send_message(char* message, int socket_id, struct sockaddr_in* to_address);

// Parsing function headers
protocol_packet parse_command(char* message);

protocol_packet parse_response(char* message);

void format_message(char* message, protocol_packet packet);


#endif