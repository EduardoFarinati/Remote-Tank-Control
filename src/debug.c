#include <stdio.h>
#include <stdarg.h>

#include "debug.h"


// Debug message flag
#define DEBUG_LEVEL CRITICAL


void write_log(message_severity severity, const char* message, ...) {
    va_list args;
    va_start(args, message);

    if(severity >= DEBUG_LEVEL) {
        vprintf(message, args);
    }

    va_end(args);
}