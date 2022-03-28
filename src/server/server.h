#ifndef SERVER_H
#define SERVER_H


// Thread sleep constants
#define MAIN_SLEEP_MS 500
#define SIMULATION_SLEEP_MS 10
#define IP_SERVER_SLEEP_MS 15
#define GRAPHICS_SLEEP_MS 50


// Tank state for plant simulation
typedef struct TankState_t {
    double level;  // current tank level
    double input;  // current input valve opening percentage (0-100)
    double output;  // current output valve opening percentage (0-100)
    double delta; // input valve angle delta (how much should be changed)
    int max; // maximum outflux
    double t; // state timestamp
} TankState;


// Tank state function headers
TankState get_tank();

void set_tank(TankState tank);

void lock_tank_state(TankState* tank);

void unlock_tank_state(TankState* tank);

// Graphics draw function headers
void create_graphics_window();

void update_graph();

int window_closed();

void close_window();

// IP server function headers
int start_server_socket();

int accept_connection();

int receive_command();

void command_action();

void close_client_socket();

void close_server_socket();

// Plant simulation function headers
void print_tank_state();

void tank_time_step();

void reset_time();

// Plant interaction function headers
void start_tank();

void open_valve(int value);

void close_valve(int value);

void set_max(int value);

int get_level();


#endif
