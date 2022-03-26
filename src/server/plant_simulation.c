#include <math.h>

#include "server.h"


double get_time_delta(TankState* tank) {
    // TODO: Get exact time delta
    double dt = 0.01;

    tank->t += dt;
    return dt;
}

void update_input_angle(TankState* tank, double dt) {
    double delta = tank->delta;

    if(delta > 0) {
        if(delta < 10*dt) {
            tank->input += delta;
            tank->delta = 0;
        }
        else {
            tank->input += 10*dt;
            tank->delta -= 10*dt;
        }
    }
    else if(delta < 0) {
        if(delta > -10*dt) {
            tank->input += delta;
            tank->delta = 0;
        }
        else {
            tank->input -= 10*dt;
            tank->delta += 10*dt;
        }
    }
}

void update_output_angle(TankState* tank) {
    double t = tank->t;

    if(t<=0) {
        tank->output = 0;
    }
    else if (t < 20) {
        tank->output = 50 + 2.5*t;
    }
    else if (t < 30) {
        tank->output = 100;
    }
    else if (t < 50) {
        tank->output = 100 - 4*(t - 30);
    }
    else if (t < 70) {
        tank->output = 20 + (t - 50);
    }
    else if (t < 100) {
        tank->output = 40 + 20*cos((t - 70) * 2*M_PI/10);
    }
    else {
        tank->output = 100;
    }
}

void tank_level_saturation(TankState* tank) {
    if(tank->level > 100) {
        tank->level = 100;
    }
    else if(tank->level < 0) {
        tank->level = 0;
    }
}

void tank_time_step() {
    TankState tank;
    double influx, outflux;
    double dt;

    // Gets current tank state
    lock_tank_state(&tank);

    // Calclates plant time step
    dt = get_time_delta(&tank);
    update_input_angle(&tank, dt);
    update_output_angle(&tank);

    // Flow calculation
    influx = 1*sin(M_PI/2 * tank.input/100);
    outflux = ((double) tank.max)/100 * (tank.level/125 + 0.2)*sin(M_PI/2 * tank.output/100);

    // Tank level calculation
    tank.level += 2*dt*(influx - outflux);
    tank_level_saturation(&tank);

    // Sets new state
    unlock_tank_state(&tank);
}
