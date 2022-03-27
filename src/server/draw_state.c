#include "../graphics.h"
#include "server.h"


// Canvas to draw state variables
Tdataholder* data;


void create_graphics_window() {
    data = datainit(SCREEN_W, SCREEN_H, 55, 110, 40, 50, 0);
}

// force input in an acceptable range to prevent crashes
double input_range_limiter(double input) {
    int int_part = (int) input;

    if(int_part >= 0) {
        return (int_part % 100) + (input - int_part);
    }
    else {
        return 100 - (-int_part % 100) + (input - int_part);
    }
}

void update_graph() {
    TankState tank = get_tank();

    if(tank.t < -10) {
        data = datainit(SCREEN_W, SCREEN_H, 55, 110, 40, 50, 0);
    }
    datadraw(data, tank.t, tank.level, input_range_limiter(tank.input), tank.output);
}

int window_closed() {
    return quit_event();
}

void close_window() {
    quit();
}