#include <stdlib.h>
#include <pthread.h>

#include "client.h"
#include "connection_sync.h"
#include "../time.h"
#include "../run_sync.h"
#include "../graphics.h"
#include "../debug.h"


void* send_ip_packets();
void* control_tank_level();
void* insert_tank_data_in_graph();
void* draw_graph_periodically();


int main(int argc, char* argv[]) {
    cli_arguments arguments;
    pthread_t ip_thread, control_thread, graph_data_thread, graph_draw_thread;
    int ret1, ret2, ret3, ret4;

    // Parses cli arguments
    parse_cli_arguments(&arguments, argc, argv);
    if(arguments.debug_flag) {
        set_debug_level(INFO);
    }
    if(arguments.dont_clear_graph_flag) {
        clear_graph_on_overflow = 0;
    }
    if(arguments.no_restart_server_flag) {
        restart_server_flag = 0;
    }

    // Creates ip thread to communicate with server
    ret1 = pthread_create(&ip_thread, NULL, send_ip_packets, (void*) &arguments);
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
    
    // Creates graph data thread to store current tank level and supposed input valve status
    ret3 = pthread_create(&graph_data_thread, NULL, insert_tank_data_in_graph, NULL);
    if(ret3) {
	    write_log(CRITICAL,"Error: unable to create graph data thread, return code: %d\n", ret3);
	    exit(EXIT_FAILURE);
    }

    // Creates graph draw thread to redraw graph with current stored values
    ret4 = pthread_create(&graph_draw_thread, NULL, draw_graph_periodically, NULL);
    if(ret4) {
	    write_log(CRITICAL,"Error: unable to create graph draw thread, return code: %d\n", ret4);
	    exit(EXIT_FAILURE);
    }

    // Opens new graph window
    new_graph();
    set_program_running(1);

    // Waits for the window to be closed
    while(!window_closed()) {
        sleep_ms(MAIN_SLEEP_MS);
    }
    write_log(CRITICAL, "Window closed, stopping program...\n");

    // Sets flag to stop program
    set_program_running(0);

    // Waits for all threads to finish to close the program.
    pthread_join(ip_thread, NULL);
    pthread_join(control_thread, NULL);
    pthread_join(graph_data_thread, NULL);
    pthread_join(graph_draw_thread, NULL);
     
    exit(EXIT_SUCCESS);
}

void* send_ip_packets(void* args) {
    // Waits for the program to start running
    while(!get_program_running()) {
        sleep_ms(IP_SLEEP_MS);
    }

    int port = ((cli_arguments*) args)->port;
    char* server_ip_address = ((cli_arguments*) args)->server_ip_address;
    int server_port = ((cli_arguments*) args)->server_port;

    // Creates socket and setart communicating
    start_client_socket(port);
    set_server_address(server_ip_address, server_port);

    while(get_program_running()) {
        // Updates tank variables
        update_tank();
        sleep_ms(IP_SLEEP_MS);
    }

    // Closes connection and tries again
    close_client_socket();

    return NULL;
}

void* control_tank_level() {
    // Waits for the program to start running
    while(!get_program_running()) {
        sleep_ms(CONTROL_SLEEP_MS);
    }

    // Waits connection
    while(!is_connected() && get_program_running()) {
        sleep_ms(CONTROL_SLEEP_MS);
    }

    if(get_program_running()) {
        reset_time();
        sleep_ms(CONTROL_SLEEP_MS);

        while(get_program_running()) {
            update_controller();
            sleep_ms(CONTROL_SLEEP_MS);
        }
    }

    return NULL;
}

void* insert_tank_data_in_graph() {
    // Waits for the program to start running
    while(!get_program_running()) {
        sleep_ms(GRAPH_DATA_SLEEP_MS);
    }

    while(get_program_running()) {
        TankState tank = get_tank();
        update_graph_data(tank.t, tank.level, tank.input, -1);

        sleep_ms(GRAPH_DATA_SLEEP_MS);
    }

    return NULL;
}

void* draw_graph_periodically() {
    // Waits for the program to start running
    while(!get_program_running()) {
        sleep_ms(GRAPH_DRAW_SLEEP_MS);
    }

    while(get_program_running()) {
        draw_graph();

        sleep_ms(GRAPH_DRAW_SLEEP_MS);
    }   
    cleanup_sdl();

    return NULL;
}
