#include <math.h>

// Tank state
typedef struct TankState_t {
    double max;  // maximum tank level
    double level;  // current tank level
    double input;  // current input valve opening percentage (0-100)
    double output;  // current output valve opening percentage (0-100)
} TankState;

TankState tank;

// input angle delta
double delta;

// timestamp
double t;

// Possible commands
enum keywords {
    OPEN_VALVE,
    CLOSE_VALVE,
    GET_LEVEL,
    COMM_TEST,
    SET_MAX,
    START
};

void start() {
    // Sets initial tank state
    tank.input = 50;
    tank.level = 0.4;
    t = 0;
    delta = 0;
}

void record_variables() {

}

double get_time_delta() {
    double dt = 0.01;

    t += dt;
    return dt;
}

void calculate_input_angle(double dt) {
    if(delta > 0) {
        if(delta < 0.01*dt) {
            tank.input += delta;
            delta = 0;
        }
        else {
            tank.input += 0.01*dt;
            delta -= 0.01*dt;
        }
    }
    else if(delta < 0) {
        if(delta > -0.01*dt) {
            tank.input += delta;
            delta = 0;
        }
        else {
            tank.input -= 0.01*dt;
            delta += 0.01*dt;
        }
    }
}

void calculate_output_angle() {
    if(t<=0) {
        tank.output = 0;
    }
    else if (t < 20000) {
        tank.output = 50 + t/400;
    }
    else if (t < 30000) {
        tank.output = 100;
    }
    else if (t < 50000) {
        tank.output = 100 - (t - 30000)/250;
    }
    else if (t < 70000) {
        tank.output = 20 + (t - 50000)/1000;
    }
    else if (t < 100000) {
        tank.output = 40 + 20*cos((t - 70000) * 2*M_PI/10000);
    }
    else {
        tank.output = 100;
    }
}

void simulate() {
    double influx, outflux;

    // Plant time step
    float dt = get_time_delta();
    calculate_input_angle(dt);
    calculate_output_angle();

    // Flow calculation
    influx = 1*sin(M_PI/2 * tank.input/100);
    outflux = tank.max/100 * (tank.level/1.25 + 0.2)*sin(M_PI/2 * tank.output/100);

    // Tank level calculation
    tank.level += 0.00002*dt*(influx - outflux);
}