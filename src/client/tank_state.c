#include <pthread.h>

#include "client.h"


// Tank level, valve openings, maximum outflux and input angle delta
TankState _tank = {
    .input = 50,
    .level = 40,
    .max = 100
};
pthread_mutex_t tank_mutex = PTHREAD_MUTEX_INITIALIZER;


// Setters & getters for shared variables
TankState get_tank() {
    TankState tank;

	pthread_mutex_lock(&tank_mutex);
    tank = _tank;
	pthread_mutex_unlock(&tank_mutex);

    return tank;
} 

void set_tank(TankState tank) {
	pthread_mutex_lock(&tank_mutex);
    _tank = tank;
	pthread_mutex_unlock(&tank_mutex);
}

void lock_tank_state(TankState* tank) {
    // Locks tank state for calculations
    // unlock must be called at the end
    pthread_mutex_lock(&tank_mutex);
    *tank = _tank;
}

void unlock_tank_state(TankState* tank) {
    // Unlocks tank state after calculations
    // lock must be called before
    _tank = *tank;
    pthread_mutex_unlock(&tank_mutex);
}
