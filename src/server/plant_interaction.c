#include <math.h>

#include "server.h"
#include "../debug.h"


// External interactions functions
void start_tank() {
    TankState tank = {
        .input = 50,
        .output = 0,
        .level = 40,
        .delta = 0,
        .max = 100,
        .t = 0
    };

    // Override current state with initial state
    set_tank(tank);
    reset_time();

    // Debug state message
    print_tank_state();
}

void open_valve(int value) {
    TankState tank;

    write_log(INFO, "Opening valve by: %d%%\n", value);
    lock_tank_state(&tank);
    tank.delta += value;
    unlock_tank_state(&tank);
}

void close_valve(int value) {
    TankState tank;

    write_log(INFO, "Closing valve by: %d%%\n", value);
    lock_tank_state(&tank);
    tank.delta -= value;
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

    // Only print if time is close to an integer
    if(fabs(tank.t - floor(tank.t)) < 11e-3) {
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
}
