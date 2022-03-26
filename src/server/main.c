#include <stdlib.h>
#include <pthread.h>

#include "server.h"
#include "../time.h"
#include "../debug.h"


void *simulate_plant();
void *generate_graphics();
void *receive_ip_packets();


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

    // Sets flag to stop program
    set_program_running(0);

    // Waits for all threads to finish to close the program.
    pthread_join(plant_thread, NULL);
	pthread_join(graphics_thread, NULL);
	pthread_join(ip_server_thread, NULL);
	 
	exit(EXIT_SUCCESS);
}

void *simulate_plant() {
    int i = 1;

    write_log(INFO, "Starting tank...");
    start_tank();
    print_tank_state();

    while(get_program_running()) {
        sleep_ms(SIMULATION_SLEEP_MS);
        tank_time_step();

        if(i >= 300) {
            print_tank_state();
            i = 1;
        }
        else {
            i++;
        }
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

// TODO:
// Se o numero de bytes nao bater
// Chamar Recvfrom multiplas vezes! RECVFROM MSG_DONTWAIT
// Remover saturacao do servidor
// Tratar limites quando pacote de resposta
// for perdido
// Modelo simples da planta
// Usar porta 9E9E -> E numero do grupo
// Para tempo clock_gettime, CLOCK_MONOTONIC_RAW
// Clear dos graficos.
// Trylock para nao travar a espera de pacotes
void *receive_ip_packets() {
    start_server_socket();
    
    while(get_program_running()) {
        sleep_ms(IP_SERVER_SLEEP_MS);
        receive_command();
        command_action();
    }

    close_server_socket();

    return NULL;
}