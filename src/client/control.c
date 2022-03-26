#include <math.h>

#include "client.h"


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


// Maintains controller between 0 and 100
void controller_saturation(int* u) {
   if(*u > max_u) {
      *u = max_u;
   }
   else if(*u < min_u) {
      *u = min_u;
   }
}

// Discrete PID controller calculation, tustin approximation
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
