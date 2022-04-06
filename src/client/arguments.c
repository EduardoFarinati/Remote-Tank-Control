#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "client.h"
#include "../debug.h"
#include "../comm.h"


#define DEFAULT_CLIENT_PORT ANY_PORT
#define DEFAULT_SERVER_IP_ADDRESS "127.0.0.1" // Local ip address
#define DEFAULT_SERVER_PORT DEFAULT_PORT


void usage(char* program_path) {
    write_log(CRITICAL, "Usage: %s [-p port][-a server_address:server_port][-d][-g][-n]\n", basename(program_path));
    exit(EXIT_FAILURE);
}

int parse_address(char* address, char* ip_address, int* port) {
    int i;

    // Check format
    if(strstr(address, ":")) {
        for(i = 0; (address[i] != ':') && (i < MAX_IP_ADDRESS_SIZE); i++) {
            ip_address[i] = address[i];
        }

        if(i <= MAX_IP_ADDRESS_SIZE) {
            ip_address[i] = '\0';
            i++;

            // Supposes the rest of the string is a port
            *port = atoi(address+i);
            if (*port != 0) {
                return 1;
            }
            else {
                return 0;
            }
        } 
        else {
            return 0;
        }
    }
    else {
        return 0;
    }
}

void parse_cli_arguments(cli_arguments* arguments, int argc, char* argv[]) {
    *arguments = (cli_arguments) {
        .port = ANY_PORT,
        .server_ip_address = DEFAULT_SERVER_IP_ADDRESS,
        .server_port = DEFAULT_SERVER_PORT,
        .debug_flag = 0,
        .dont_clear_graph_flag = 0,
        .no_restart_server_flag = 0
    };
    int opt;

    // Loops over optional arguments
    while((opt = getopt(argc, argv, "p:a:dgn")) != -1) {
        switch(opt) {
            case 'p':
                arguments->port = atoi(optarg);
                if(arguments->port <= 0) {
                    usage(argv[0]);
                }
                break;
            case 'a':
                if(!parse_address(optarg, arguments->server_ip_address, &arguments->server_port)) {
                    usage(argv[0]);
                }
                break;
            case 'd':
                arguments->debug_flag = 1;
                break;
            case 'g':
                arguments->dont_clear_graph_flag = 1;
                break;
            case 'n':
                arguments->no_restart_server_flag = 1;
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
}
