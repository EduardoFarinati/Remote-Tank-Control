#ifndef CLIENT_H
#define CLIENT_H


// Thread sleep constants
#define MAIN_SLEEP_MS 500
#define CONTROL_SLEEP_MS 20
#define IP_SLEEP_MS 25
#define GRAPHICS_SLEEP_MS 50

#define NO_ANSWER_TIMEOUT  250e-3


// Tank state for plant simulation
typedef struct TankState_t {
    int level;  // current tank level
    int input;  // current input valve opening percentage (0-100)
    int delta; // input valve angle delta (how much should be changed)
    int max; // maximum outflux
    double t; // state timestamp
} TankState;


// State variable function headers
TankState get_tank();

void set_tank(TankState tank);

void lock_tank_state(TankState* tank);

void unlock_tank_state(TankState* tank);

// Graphics draw function headers
void create_graphics_window();

void update_graph();

int window_closed();

void close_window();

// IP client function headers
int start_client_socket();

void set_server_address_p(char* server_ip_address, int server_port);

void set_server_address(char* server_ip_address);

void update_tank();

void close_client_socket();

// Control function headers
int controller_output(int r, int y);

void update_controller();

void reset_time();

void set_max_min_opening(int value);


#endif
