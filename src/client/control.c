#include <math.h>

#include "client.h"
#include "../time.h"


// PID controller definition
const double Kp = 3.5;
const double Ki = 0.2;
const double Kd = 0;
const double T = CONTROL_SLEEP_MS * 1e-3; // Controller time constant

// PID variables
double P;
double I = 0;
double D = 0;
int e_p = 40; // Past measure of e

// Dynamic saturation for lost packages
int max_u = 100;
int min_u = 0;

// Controller reference
int r = 80;

// Time delta calculation
struct timespec time_before;


// Maintains controller between 0 and 100
void controller_saturation(int* u) {
   if(*u > max_u) {
      *u = max_u;
   }
   else if(*u < min_u) {
      *u = min_u;
   }
}

// Discrete PID controller calculation
int controller_output(int r, int y) {
   int u;
   int e = r - y;

   P = Kp * e;
   I += Ki * T * e;
   D = Kd * ((double) (e - e_p)) / T;

   u = round(P + I + D);

   // Controller saturation
   controller_saturation(&u);

   // Past error set
   e_p = e;

   return u;
}


void reset_time() {
   time_before = get_current_time();
}

double get_tank_time_delta() {
   struct timespec time_now = get_current_time();

   double dt = get_time_delta(time_before);
   time_before = time_now;

   return dt;
}

// Update controller output
void update_controller() {
   TankState tank;

   lock_tank_state(&tank);
   int y = tank.level;

   // Update output
   int u = controller_output(r, y);
   int u_p = tank.input;
   tank.delta = u - u_p;

   // Update time
   tank.t += get_tank_time_delta();
   
   unlock_tank_state(&tank);
}

void set_max_min_opening(int value) {
   // Fix limits when we dont know by how much
   // the valve is opened
   if(value > 0) {
      max_u -= value;
   }
   else if (value < 0) {
      min_u += -value;
   }
}
