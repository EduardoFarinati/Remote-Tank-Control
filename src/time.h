#ifndef TIME_H
#define TIME_H


#include <time.h>


void sleep_ms(int ms);

struct timespec get_current_time();

double get_time_delta_ms(struct timespec time_init);


#endif