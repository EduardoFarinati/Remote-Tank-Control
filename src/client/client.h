#ifndef CLIENT_H
#define CLIENT_H


// Thread sleep constants
#define MAIN_SLEEP_MS 500
#define CONTROL_SLEEP_MS 20
#define IP_SLEEP_MS 25
#define GRAPH_DATA_SLEEP_MS 50
#define GRAPH_DRAW_SLEEP_MS 150

#define NO_ANSWER_TIMEOUT  250e-3


// Command line arguments structure
#define ANY_PORT -1
#define MAX_IP_ADDRESS_SIZE 16
typedef struct cli_arguments_t {
    int port;
    char server_ip_address[MAX_IP_ADDRESS_SIZE+1];
    int server_port;
    int debug_flag;
    int dont_clear_graph_flag;
} cli_arguments;

// Tank state for plant simulation
typedef struct TankState_t {
    int level;  // current tank level
    int input;  // current input valve opening percentage (0-100)
    int delta; // input valve angle delta (how much should be changed)
    int max; // maximum outflux
    double t; // state timestamp
} TankState;


// Argument parsing function headers
void parse_cli_arguments(cli_arguments* arguments, int argc, char* argv[]);

// State variable function headers
TankState get_tank();

void set_tank(TankState tank);

void lock_tank_state(TankState* tank);

void unlock_tank_state(TankState* tank);

// Graphics draw function headers
void create_new_graph();

void* insert_tank_data_in_graph();

void* draw_graph_periodically();

// IP client function headers
int start_client_socket(int port);

void set_server_address(char* server_ip_address, int server_port);

void update_tank();

void close_client_socket();

// Control function headers
int controller_output(int r, int y);

void update_controller();

void reset_time();

void set_max_min_opening(int value);


#endif
