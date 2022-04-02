#include <stdio.h>
#include <stdarg.h>

#include "debug.h"


// Debug message flag
message_severity debug_level = CRITICAL;


void write_log(message_severity severity, const char* message, ...) {
    va_list args;
    va_start(args, message);

    if(severity >= debug_level) {
        vprintf(message, args);
    }

    va_end(args);
}

void set_debug_level(message_severity level) {
    debug_level = level;
}