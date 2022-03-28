#include <string.h>

#include "comm.h"


int min(int a, int b, int c) {
    if(a < b) {
        if(b < c) {
            return c;
        }
        else {
            return b;
        }
    }
    else {
        if(a < c) {
            return c;
        }
        else {
            return a;
        }
    }
}

int levenshtein_distance(const char* str_1, const char* str_2) {
    // Degenerate cases
    if(strlen(str_1) == 0) {
        return strlen(str_2);
    }
    else if(strlen(str_2) == 0) {
        return strlen(str_1);
    }
    else if(str_1[0] == str_2[0]) {
        return levenshtein_distance(str_1 + 1, str_2 + 1);
    }
    else {
        return min(
            levenshtein_distance(str_1 + 1, str_2),
            levenshtein_distance(str_1, str_2 + 1),
            levenshtein_distance(str_1 + 1, str_2 + 1)
        );
    }
}

int is_packet_similar(char* buffer, protocol_packet expected_packet) {
    char expected_buffer[BUFFER_LENGTH+1];

    // What should be received
    format_message(expected_buffer, expected_packet);

    if(levenshtein_distance(buffer, expected_buffer) <= CORRUPTION_TOLERANCE) {
        return 0;
    }
    else {
        return 1;
    }
}
