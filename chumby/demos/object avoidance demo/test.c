#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int main (int argc, char * argv[]) {
    char c;
    int speed_fast=0;
    int speed_slow=0;

    if((c = getopt(argc, argv, "s:")) != -1) {
        if(!optarg)
            exit(0);
        sscanf(optarg, "%d,%d", &speed_fast, &speed_slow);
        fprintf(stderr, "speed_fast: %d\n", speed_fast);
        fprintf(stderr, "speed_slow: %d\n", speed_slow);
    }
    return 0;
}
