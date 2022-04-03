#include <pthread.h>


int _connected = 0;
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;


int is_connected() {
    int connected;

	pthread_mutex_lock(&connection_mutex);
    connected = _connected;
	pthread_mutex_unlock(&connection_mutex);

    return connected;
}

void connection_estabilished() {
	pthread_mutex_lock(&connection_mutex);
    _connected = 1;
	pthread_mutex_unlock(&connection_mutex);
}

void connection_lost() {
	pthread_mutex_lock(&connection_mutex);
    _connected = 0;
	pthread_mutex_unlock(&connection_mutex);
}
