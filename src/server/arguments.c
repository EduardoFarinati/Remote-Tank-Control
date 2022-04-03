#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "server.h"
#include "../debug.h"
#include "../comm.h"


void usage(char* program_path) {
    write_log(CRITICAL, "Usage: %s [-p port][-d][-g]\n", basename(program_path));
    exit(EXIT_FAILURE);
}

void parse_cli_arguments(cli_arguments* arguments, int argc, char* argv[]) {
    *arguments = (cli_arguments) {
        .port = DEFAULT_PORT,
        .debug_flag = 0,
        .dont_clear_graph_flag = 0
    };
    int opt;

    // Loops over optional arguments
    while((opt = getopt(argc, argv, "p:dg")) != -1) {
        switch(opt) {
            case 'p':
                arguments->port = atoi(optarg);
                if(arguments->port <= 0) {
                    usage(argv[0]);
                }
                break;
            case 'd':
                arguments->debug_flag = 1;
                break;
            case 'g':
                arguments->dont_clear_graph_flag = 1;
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
}
