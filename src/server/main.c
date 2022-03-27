#include <stdlib.h>
#include <pthread.h>

#include "server.h"
#include "../time.h"
#include "../run_sync.h"
#include "../debug.h"


void* simulate_plant();
void* generate_graphics();
void* receive_ip_packets();


int main() {
    pthread_t plant_thread, graphics_thread, ip_server_thread;
    int ret1, ret2, ret3;

    // Creates plant thread to simulate the tank system
    ret1 = pthread_create(&plant_thread, NULL, simulate_plant, NULL);
    if(ret1)
    {
	    write_log(CRITICAL, "Error: unable to create plant thread, return code: %d\n", ret1);
	    exit(EXIT_FAILURE);
    }
    
    // Creates graphics thread to show current tank level and valve status
    ret2 = pthread_create(&graphics_thread, NULL, generate_graphics, NULL);
    if(ret2)
    {
	    write_log(CRITICAL, "Error: unable to create graphics thread, return code: %d\n", ret2);
	    exit(EXIT_FAILURE);
    }

    // Creates ip server thread to handle control commands to the tank system
    ret3 = pthread_create(&ip_server_thread, NULL, receive_ip_packets, NULL);
    if(ret3)
    {
	    write_log(CRITICAL, "Error: unable to create ip server thread, return code: %d\n", ret3);
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
    pthread_join(plant_thread, NULL);
	pthread_join(graphics_thread, NULL);
	pthread_join(ip_server_thread, NULL);
	 
	exit(EXIT_SUCCESS);
}

void* simulate_plant() {
    write_log(INFO, "Starting tank...");
    start_tank();
    sleep_ms(SIMULATION_SLEEP_MS);

    while(get_program_running()) {
        tank_time_step();
        sleep_ms(SIMULATION_SLEEP_MS);
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

// TODO:
// Se o numero de bytes nao bater
// Chamar Recvfrom multiplas vezes! RECVFROM MSG_DONTWAIT
// Tratar limites quando pacote de resposta
// for perdido
// Modelo simples da planta
// Para tempo clock_gettime, CLOCK_MONOTONIC_RAW
// Clear dos graficos.
// Trylock para nao travar a espera de pacotes
// Testar mensagens com bytes corrompidos
void* receive_ip_packets() {
    start_server_socket();
    sleep_ms(IP_SERVER_SLEEP_MS);
    
    while(get_program_running()) {
        receive_command();
        command_action();
        sleep_ms(IP_SERVER_SLEEP_MS);
    }

    close_server_socket();

    return NULL;
}