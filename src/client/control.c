#include <math.h>

#include "client.h"


// PID controller definition
const double Kp = 0.5;
const double Ki = 0.2;
const double Kd = 0;
const double p = 100; // Derivative action pole
const double T = CONTROL_SLEEP_MS * 1e-3; // Controller time constant

// PID variables
double P;
double I = 0;
double D = 0;
int yp = 50; // Past measure of y


// Maintains controller between 0 and 100
void controller_saturation(int* u) {
   if(*u > 100) {
      *u = 100;
   }
   else if(*u < 0) {
      *u = 0;
   }
}

// Discrete PID controller calculation, tustin approximation
int controller_output(int r, int y) {
   int u;
   int e = r - y;

   P = Kp * e;
   I += Ki * T * ((double) (y + yp))/2.;
   D = D*(2 - p*T)/(2 + p*T) + 2*p*Kd/((2 + p*T)*(y - yp));

   u = round(P + I + D);

   // Controller saturation
   controller_saturation(&u);

   return u;
}
