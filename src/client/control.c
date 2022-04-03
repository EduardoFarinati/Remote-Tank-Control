#include <stdlib.h>
#include <math.h>

#include "client.h"
#include "../time.h"


// PID controller definition
const double Kp = 3.5;
const double Ki = 1;
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
int controller_saturation(int u) {
   if(u > max_u) {
      return max_u;
   }
   else if(u < min_u) {
      return min_u;
   }
   else {
      return u;
   }
}

double anti_windup(int e) {
   double ideal = Ki * T * e;

   // Smaller on large errors
   if(abs(e) <= 5) {
      return ideal;
   }
   else if(abs(e) <= 10) {
      return 0.5 * ideal;
   }
   else if(abs(e) <= 15) {
      return 5e-2 * ideal;
   }
   else {
      return 1e-2 * Ki * T * e;
   }
} 

// Discrete PID controller calculation
int controller_output(int r, int y) {
   int u;
   int e = r - y;

   P = Kp * e;
   I += anti_windup(e);
   D = Kd * ((double) (e - e_p)) / T;

   u = round(P + I + D);

   // Controller saturation
   u = controller_saturation(u);

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

const int max_delta = 3;
int get_delta(int u, int u_p) {
   int delta = u - u_p;

   if(delta > max_delta) {
      delta = max_delta;
   }
   else if(delta < -max_delta) {
      delta = -max_delta;
   }

   return delta;
}

// Update controller output
void update_controller() {
   TankState tank;

   lock_tank_state(&tank);
   int y = tank.level;

   // Update output
   int u = controller_output(r, y);
   int u_p = tank.input;
   tank.delta = get_delta(u, u_p);

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
