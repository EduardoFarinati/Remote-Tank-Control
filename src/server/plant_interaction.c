#include <math.h>

#include "server.h"
#include "../debug.h"


// External interactions functions
void start_tank() {
    TankState tank;

    // Gets current state
    lock_tank_state(&tank);

    // Override with initial state
    tank = (TankState) {
        .input = 50,
        .output = 0,
        .level = 40,
        .delta = 0,
        .max = 100,
        .t = 0
    };

    // Sets state
    unlock_tank_state(&tank);
}

void delta_saturation(TankState* tank) {
    double over = tank->delta + tank->input;

    if(over > 100) {
        tank->delta = 100 - tank->input;
    }
    else if(over < 0) {
        tank->delta = -tank->input;
    }
}

void open_valve(int value) {
    TankState tank;

    write_log(INFO, "Opening valve by: %d%%\n", value);
    lock_tank_state(&tank);
    tank.delta += value;
    delta_saturation(&tank);
    unlock_tank_state(&tank);
}

void close_valve(int value) {
    TankState tank;

    write_log(INFO, "Closing valve by: %d%%\n", value);
    lock_tank_state(&tank);
    tank.delta -= value;
    delta_saturation(&tank);
    unlock_tank_state(&tank);
}

void set_max(int value) {
    TankState tank;

    write_log(INFO, "Setting max to: %d\n", value);
    lock_tank_state(&tank);
    tank.max = value;
    unlock_tank_state(&tank);
}

int get_level() {
    TankState tank;
    double level;

    lock_tank_state(&tank);
    level = tank.level;
    unlock_tank_state(&tank);

    return round(level);
}


void print_tank_state() {
    TankState tank = get_tank();

    write_log(
        INFO,
        "TANK-STATE(T:%.2f)={Lvl:%.2f, In:%.2f, dIn:%.2f, Out:%.2f, Max:%d}\n",
        tank.t,
        tank.level, 
        tank.input,
        tank.delta,
        tank.output,
        tank.max
    );
}
