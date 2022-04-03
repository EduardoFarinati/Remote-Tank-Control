#include <stdlib.h>
#include <pthread.h>

#include "server.h"
#include "../time.h"
#include "../run_sync.h"
#include "../debug.h"
#include "../graphics.h"


void* simulate_plant();
void* receive_ip_packets();
void* insert_tank_data_in_graph();
void* draw_graph_periodically();


int main(int argc, char* argv[]) {
    cli_arguments arguments;
    pthread_t plant_thread, ip_server_thread, graph_data_thread, graph_draw_thread;
    int ret1, ret2, ret3, ret4;

    // Parses cli arguments
    parse_cli_arguments(&arguments, argc, argv);
    if(arguments.debug_flag) {
        set_debug_level(INFO);
    }

    // Creates plant thread to simulate the tank system
    ret1 = pthread_create(&plant_thread, NULL, simulate_plant, NULL);
    if(ret1)
    {
	    write_log(CRITICAL, "Error: unable to create plant thread, return code: %d\n", ret1);
	    exit(EXIT_FAILURE);
    }
    
    // Creates ip server thread to handle control commands to the tank system
    ret2 = pthread_create(&ip_server_thread, NULL, receive_ip_packets, (void*) &arguments);
    if(ret2)
    {
	    write_log(CRITICAL, "Error: unable to create ip server thread, return code: %d\n", ret2);
	    exit(EXIT_FAILURE);
    }

    // Creates graph data thread to store current tank level and valve status
    ret3 = pthread_create(&graph_data_thread, NULL, insert_tank_data_in_graph, NULL);
    if(ret3)
    {
	    write_log(CRITICAL, "Error: unable to create graphics thread, return code: %d\n", ret3);
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
    pthread_join(plant_thread, NULL);
	pthread_join(ip_server_thread, NULL);
    pthread_join(graph_data_thread, NULL);
    pthread_join(graph_draw_thread, NULL);
	 
	exit(EXIT_SUCCESS);
}

void* simulate_plant() {
    // Waits for the program to start running
    while(!get_program_running()) {
        sleep_ms(SIMULATION_SLEEP_MS);
    }

    write_log(INFO, "Starting tank...");
    start_tank();
    sleep_ms(SIMULATION_SLEEP_MS);

    while(get_program_running()) {
        tank_time_step();
        sleep_ms(SIMULATION_SLEEP_MS);
    }

    return NULL;
}

// TODO:
// Clear dos graficos.
// Trylock para nao travar a espera de pacotes
void* receive_ip_packets(void* args) {
    // Waits for the program to start running
    while(!get_program_running()) {
        sleep_ms(IP_SERVER_SLEEP_MS);
    }

    int port = ((cli_arguments*) args)->port;

    start_server_socket(port);
    sleep_ms(IP_SERVER_SLEEP_MS);
    
    while(get_program_running()) {
        if(receive_command() == 0) {
            command_action();
        }
        sleep_ms(IP_SERVER_SLEEP_MS);
    }

    close_server_socket();

    return NULL;
}

void* insert_tank_data_in_graph() {
    // Waits for the program to start running
    while(!get_program_running()) {
        sleep_ms(GRAPH_DATA_SLEEP_MS);
    }

    while(get_program_running()) {
        TankState tank = get_tank();
        update_graph_data(tank.t, tank.level, tank.input, tank.output);

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

    return NULL;
}
