#include <cstdio>
#include <stdlib.h>

#include "util.h"

void errif(bool condition, const char* message) {
    if (condition) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}