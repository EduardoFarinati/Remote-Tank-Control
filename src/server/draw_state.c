#include "../graphics.h"
#include "server.h"


// Canvas to draw state variables
Tdataholder* data;


void create_graphics_window() {
    data = datainit(SCREEN_W, SCREEN_H, 55, 110, 45, 0, 0);
}

void update_graph() {
    TankState tank = get_tank();
    datadraw(data, tank.t, tank.level, tank.input, tank.output);
}

int window_closed() {
    return quit_event();
}

void close_window() {
    quit();
}