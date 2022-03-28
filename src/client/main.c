#include <stdlib.h>
#include <pthread.h>

#include "client.h"
#include "../time.h"
#include "../run_sync.h"
#include "../debug.h"


void* send_ip_packets();
void* control_tank_level();
void* generate_graphics();


int main() {
    pthread_t ip_thread, control_thread, graphics_thread;
    int ret1, ret2, ret3;

    // Creates ip thread to communicate with server
    ret1 = pthread_create(&ip_thread, NULL, send_ip_packets, NULL);
    if(ret1) {
	    write_log(CRITICAL,"Error: unable to create ip thread, return code: %d\n", ret1);
	    exit(EXIT_FAILURE);
    }

    // Creates control thread to control the tank system
    ret2 = pthread_create(&control_thread, NULL, control_tank_level, NULL);
    if(ret2) {
	    write_log(CRITICAL,"Error: unable to create control thread, return code: %d\n", ret2);
	    exit(EXIT_FAILURE);
    }
    
    // Creates graphics thread to show current tank level and supposed input valve status
    ret3 = pthread_create(&graphics_thread, NULL, generate_graphics, NULL);
    if(ret3) {
	    write_log(CRITICAL,"Error: unable to create graphics thread, return code: %d\n", ret3);
	    exit(EXIT_FAILURE);
    }

    // Waits for the window to be closed
    while(!window_closed()) {
        sleep_ms(MAIN_SLEEP_MS);
    }
    write_log(CRITICAL, "Window closed, stopping program...\n");

    // Sets flag to stop program
    set_program_running(0);

    // Waits for all threads to finish to close the program.
    pthread_join(control_thread, NULL);
    pthread_join(graphics_thread, NULL);
     
    exit(EXIT_SUCCESS);
}

void* send_ip_packets() {
    while(get_program_running()) {
        start_client_socket();
        set_server_address("127.0.0.1");
 
        while(get_program_running()) {
            // Updates tank variables
            update_tank();
            sleep_ms(IP_SLEEP_MS);
        }

        // Closes connection and tries again
        close_client_socket();
    }

    return NULL;
}

void* control_tank_level() {
    reset_time();
    sleep_ms(CONTROL_SLEEP_MS);

    while(get_program_running()) {
        update_controller();
        sleep_ms(CONTROL_SLEEP_MS);
    }

    return NULL;
}

void* generate_graphics() {
    create_graphics_window();
    sleep_ms(GRAPHICS_SLEEP_MS);

    while(get_program_running()) {
        update_graph();
        sleep_ms(GRAPHICS_SLEEP_MS);
    }

    return NULL;
}
