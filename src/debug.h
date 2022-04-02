#ifndef DEBUG_H
#define DEBUG_H


typedef enum message_severity_t {
    INFO,
    FAILURE,
    CRITICAL
} message_severity;


void write_log(message_severity severity, const char* message, ...);

void set_debug_level(message_severity level);


#endif