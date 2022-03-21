#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "client.h"
#include "../time.h"


void *control_tank_level();
void *generate_graphics();


int _program_running = 1;
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

int main() {
    pthread_t control_thread, graphics_thread;
    int ret1, ret2;

    // Creates control thread to control the tank system
    ret1 = pthread_create(&control_thread, NULL, control_tank_level, NULL);
    if(ret1) {
	    fprintf(stderr,"Error: unable to create control thread, return code: %d\n", ret1);
	    exit(EXIT_FAILURE);
    }
    
    // Creates graphics thread to show current tank level and supposed input valve status
    ret2 = pthread_create(&graphics_thread, NULL, generate_graphics, NULL);
    if(ret2) {
	    fprintf(stderr,"Error: unable to create graphics thread, return code: %d\n", ret2);
	    exit(EXIT_FAILURE);
    }

    // Waits for the window to be closed
    while(!window_closed()) {
        sleep_ms(MAIN_SLEEP_MS);
    }

    // Sets flag to stop program
    set_program_running(0);

    // Waits for all threads to finish to close the program.
    pthread_join(control_thread, NULL);
    pthread_join(graphics_thread, NULL);
     
    exit(EXIT_SUCCESS);
}

void *control_tank_level() {
    int is_connected = 0;

    while(get_program_running()) {
        start_client_socket();

        while(!is_connected && get_program_running()) {
            if(!connect_to_server("127.0.0.1")) {
                is_connected = !comm_test();
            };

            sleep_ms(CONTROL_SLEEP_MS);
        }

        is_connected = !start_tank();
        while(is_connected && get_program_running()) {
            // Updates tank variables
            is_connected = !update_tank();
            sleep_ms(CONTROL_SLEEP_MS);
        }

        // Closes connection and tries again
        close_client_socket();
    }

    return NULL;
}

void *generate_graphics() {
    create_graphics_window();
    sleep_ms(GRAPHICS_SLEEP_MS);

    while(get_program_running()) {
        update_graph();
        sleep_ms(GRAPHICS_SLEEP_MS);
    }

    return NULL;
}
