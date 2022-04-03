#include <pthread.h>


int _program_running = 0;
pthread_mutex_t program_mutex = PTHREAD_MUTEX_INITIALIZER;


int get_program_running() {
    int program_running;

	pthread_mutex_lock(&program_mutex);
    program_running = _program_running;
	pthread_mutex_unlock(&program_mutex);

    return program_running;
}

void set_program_running(int program_running) {
	pthread_mutex_lock(&program_mutex);
    _program_running = program_running;
	pthread_mutex_unlock(&program_mutex);
}
