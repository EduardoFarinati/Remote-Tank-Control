#include <math.h>

#include "time.h"


void sleep_ms(int ms) {
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    nanosleep(&ts, NULL);
}


struct timespec get_current_time() {
    struct timespec time;

    // Get the current time from monotonic clock
    clock_gettime(CLOCK_MONOTONIC_RAW, &time); 

    return time;
}

double get_time_delta_ms(struct timespec time_before) {
    // Calculate the actual time after a delay
    double dt;

    // Check if must carry over the nanosecond subtraction
    struct timespec time_now = get_current_time();
    if((time_now.tv_nsec - time_before.tv_nsec) > 0) {
        dt = (time_now.tv_sec - time_before.tv_sec) + (time_now.tv_nsec - time_before.tv_nsec) * 1e-9;
    }
    else {
        dt = (time_now.tv_sec - time_before.tv_sec - 1) + (time_now.tv_nsec - time_before.tv_nsec + 1000000000) * 1e-9;
    }

    // Rounds to ms
    return 1e-3 * round(dt * 1e3);
}
